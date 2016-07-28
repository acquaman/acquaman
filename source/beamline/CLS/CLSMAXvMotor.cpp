/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "CLSMAXvMotor.h"

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlStopAction.h"
#include "actions3/AMListAction3.h"
#include "util/AMErrorMonitor.h"

CLSMAXvMotor::~CLSMAXvMotor(){}
CLSMAXvMotor::CLSMAXvMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds, QObject *parent, QString pvUnitFieldName) :
	AMPVwStatusControl(name, hasEncoder ? baseName+pvUnitFieldName+":fbk" : baseName+pvUnitFieldName+":sp", baseName+pvUnitFieldName, baseName+":status", baseName+":stop", parent, tolerance, moveStartTimeoutSeconds, new CLSMAXvControlStatusChecker(), 1, description)
		//AMPVwStatusControl(name, baseName+":mm:fbk", baseName+":mm", baseName+":status", baseName+":stop", parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerStopped(0), 1, description)
{
	pvBaseName_ = baseName;

	hasEncoder_ = hasEncoder;
	usingKill_ = false;
	killPV_ = new AMProcessVariable(baseName+":kill", true, this);

	stepSetpoint_ = new AMReadOnlyPVControl(name+"StepSetpoint", baseName+":step:sp", this);

	stepMotorFeedback_ = new AMReadOnlyPVControl(name+"StepFeedback", baseName+pvUnitFieldName+":sp", this);

	EGUVelocity_ = new AMPVControl(name+"EGUVelocity", baseName+":vel"+pvUnitFieldName+"ps:sp", baseName+":velo"+pvUnitFieldName+"ps", QString(), this, 0.05);
	EGUBaseVelocity_ = new AMPVControl(name+"EGUBaseVelocity", baseName+":vBase"+pvUnitFieldName+"ps:sp", baseName+":vBase"+pvUnitFieldName+"ps", QString(), this, 0.05);
	EGUAcceleration_ = new AMPVControl(name+"EGUAcceleration", baseName+":acc"+pvUnitFieldName+"pss:sp", baseName+":accel"+pvUnitFieldName+"pss", QString(), this, 2);
	EGUCurrentVelocity_ = new AMReadOnlyPVControl(name+"EGUCurrentVelocity", baseName+":vel"+pvUnitFieldName+"ps:fbk", this);
	EGUSetPosition_ = new AMPVControl(name+"EGUSetPosition", baseName+pvUnitFieldName+":setPosn", baseName+pvUnitFieldName+":setPosn", QString(), this, 0.005);
	EGUOffset_ = new AMPVControl(name+"EGUOffset", baseName+pvUnitFieldName+":offset", baseName+pvUnitFieldName+":offset", QString(), this, 0.005);

	step_ = new AMPVControl(name+"Step", baseName+":step:sp", baseName+":step", QString(), this, 20);
	stepVelocity_ = new AMPVControl(name+"StepVelocity", baseName+":velo:sp", baseName+":velo", QString(), this, 2);
	stepBaseVelocity_ = new AMPVControl(name+"StepBaseVelocity", baseName+":vBase:sp", baseName+":veloBase", QString(), this, 2);
	stepAcceleration_ = new AMPVControl(name+"StepAcceleration", baseName+":accel:sp", baseName+":accel", QString(), this, 2);
	stepCurrentVelocity_ = new AMReadOnlyPVControl(name+"StepCurrentVelocity", baseName+":velo:fbk", this);

	cwLimit_ = new AMReadOnlyPVControl(name+"CWLimit", baseName+":cw", this);
	cwLimit_->setTolerance(0.1);
	ccwLimit_ = new AMReadOnlyPVControl(name+"CCWLimit", baseName+":ccw", this);
	ccwLimit_->setTolerance(0.1);

	statusPVControl_ = new AMReadOnlyPVControl("Motor moving status", baseName+":status", this);

	powerState_ = new AMPVControl(name+"PowerState", baseName+":power", baseName+":power", QString(), this, 0.1);
	powerInverted_ = new AMPVControl(name+"PowerInverted", baseName+":invertPower", baseName+":invertPower", QString(), this, 0.1);

	encoderCalibrationSlope_ = new AMPVControl(name+"EncoderCalibrationSlope", baseName+":enc:slope", baseName+":enc:slope", QString(), this, 0.00001);
	stepCalibrationSlope_ = new AMPVControl(name+"StepCalibrationSlope", baseName+":step:slope", baseName+":step:slope", QString(), this, 0.00001);
	encoderCalibrationOffset_ = new AMPVControl(name+"EncoderCalibrationOffset", baseName+":enc:offset", baseName+":enc:offset", QString(), this, 0.001);
	encoderCalibrationAbsoluteOffset_ = new AMPVControl(name+"EncoderCalibrationAbsOffset", baseName+":enc:absOffset", baseName+":enc:absOffset", QString(), this);
	stepCalibrationOffset_ = new AMPVControl(name+"StepCalibrationOffset", baseName+":step:offset", baseName+":step:offset", QString(), this, 0.001);

	motorType_ = new AMPVControl(name+"MotorType", baseName+":motorType:sp", baseName+":motorType", QString(), this, 0.1);

	limitActiveState_ = new AMPVControl(name+"LimitActiveState", baseName+":limitHigh", baseName+":limitHigh", QString(), this, 0.1);
	limitDisabled_ = new AMPVControl(name+"LimitDisabled", baseName+":disableLimits", baseName+":disableLimits", QString(), this, 0.1);

	closedLoopEnabled_ = new AMPVControl(name+"ClosedLoopEnabled", baseName+":closedLoop", baseName+":closedLoop", QString(), this, 0.1);
	servoPIDEnabled_ = new AMPVControl(name+"ServoPIDEnabled", baseName+":hold:sp", baseName+":hold", QString(), this, 0.1);

	encoderTarget_ = new AMPVwStatusControl(name+"EncoderTarget", baseName+":enc:fbk", baseName+":encTarget", baseName+":status", QString(), this, 10, 2.0, new CLSMAXvControlStatusChecker(), 1);
	encoderFeedback_ = new AMReadOnlyPVControl(name+"EncFeedback", baseName+":enc:fbk", this);
	encoderMovementType_ = new AMPVControl(name+"EncoderMovementType", baseName+":encMoveType", baseName+":selEncMvType", QString(), this, 0.1);
	preDeadBand_ = new AMPVControl(name+"PreDeadBand", baseName+":preDBand", baseName+":preDBand", QString(), this, 1);
	postDeadBand_ = new AMPVControl(name+"PostDeadBand", baseName+":postDBand", baseName+":postDBand", QString(), this, 1);
	retries_ = new AMReadOnlyPVControl(name+"Retries", baseName+":retry:fbk", this);
	maxRetries_ = new AMPVControl(name+"MaxRetries", baseName+":maxRetry", baseName+":maxRetry", QString(), this, 1);
	encoderPercentApproach_ = new AMPVControl(name+"EncoderPercentApproach", baseName+":pctApproach", baseName+":pctApproach", QString(), this, 0.01);
	encoderStepSoftRatio_ = new AMPVControl(name+"EncoderStepSoftRatio", baseName+":softRatio", baseName+":softRatio", QString(), this, 0.001);

	connect(EGUVelocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(EGUVelocity_, SIGNAL(valueChanged(double)), this, SIGNAL(EGUVelocityChanged(double)));
	connect(EGUBaseVelocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(EGUBaseVelocity_, SIGNAL(valueChanged(double)), this, SIGNAL(EGUBaseVelocityChanged(double)));
	connect(EGUAcceleration_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(EGUAcceleration_, SIGNAL(valueChanged(double)), this, SIGNAL(EGUAccelerationChanged(double)));
	connect(EGUCurrentVelocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(EGUCurrentVelocity_, SIGNAL(valueChanged(double)), this, SIGNAL(EGUCurrentVelocityChanged(double)));
	connect(EGUSetPosition_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(EGUSetPosition_, SIGNAL(valueChanged(double)), this, SIGNAL(EGUSetPositionChanged(double)));
	connect(EGUOffset_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(EGUOffset_, SIGNAL(valueChanged(double)), this, SIGNAL(EGUOffsetChanged(double)));

	connect(step_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(step_, SIGNAL(valueChanged(double)), this, SIGNAL(stepChanged(double)));
	connect(stepVelocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(stepVelocity_, SIGNAL(valueChanged(double)), this, SIGNAL(stepVelocityChanged(double)));
	connect(stepBaseVelocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(stepBaseVelocity_, SIGNAL(valueChanged(double)), this, SIGNAL(stepBaseVelocityChanged(double)));
	connect(stepAcceleration_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(stepAcceleration_, SIGNAL(valueChanged(double)), this, SIGNAL(stepAccelerationChanged(double)));
	connect(stepCurrentVelocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(stepCurrentVelocity_, SIGNAL(valueChanged(double)), this, SIGNAL(stepCurrentVelocityChanged(double)));

	connect(cwLimit_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(cwLimit_, SIGNAL(valueChanged(double)), this, SLOT(onLimitsChanged(double)));
	connect(ccwLimit_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(ccwLimit_, SIGNAL(valueChanged(double)), this, SLOT(onLimitsChanged(double)));

	connect(powerState_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(powerState_, SIGNAL(valueChanged(double)), this, SLOT(onPowerStateChanged(double)));
	connect(powerInverted_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(powerInverted_, SIGNAL(valueChanged(double)), this, SLOT(onPowerInvertedChanged(double)));

	connect(encoderCalibrationSlope_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderCalibrationSlope_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderCalibrationSlopeChanged(double)));
	connect(stepCalibrationSlope_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(stepCalibrationSlope_, SIGNAL(valueChanged(double)), this, SIGNAL(stepCalibrationSlopeChanged(double)));
	connect(encoderCalibrationOffset_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderCalibrationOffset_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderCalibrationOffsetChanged(double)));
	connect(encoderCalibrationAbsoluteOffset_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderCalibrationAbsoluteOffsetChanged(double)));
	connect(encoderCalibrationAbsoluteOffset_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)) );
	connect(stepCalibrationOffset_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(stepCalibrationOffset_, SIGNAL(valueChanged(double)), this, SIGNAL(stepCalibrationOffsetChanged(double)));

	connect(motorType_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(motorType_, SIGNAL(valueChanged(double)), this, SLOT(onMotorTypeChanged(double)));

	connect(limitActiveState_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(limitActiveState_, SIGNAL(valueChanged(double)), this, SLOT(onLimitActiveStateChanged(double)));
	connect(limitDisabled_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(limitDisabled_, SIGNAL(valueChanged(double)), this, SLOT(onLimitDisabledChanged(double)));

	connect(closedLoopEnabled_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(closedLoopEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onClosedLoopEnabledChanged(double)));
	connect(servoPIDEnabled_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(servoPIDEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onServoPIDEnabledChanged(double)));

	connect(encoderTarget_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderTarget_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderTargetChanged(double)));
	connect(encoderMovementType_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderMovementType_, SIGNAL(valueChanged(double)), this, SLOT(onEncoderMovementTypeChanged(double)));
	connect(preDeadBand_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(preDeadBand_, SIGNAL(valueChanged(double)), this, SIGNAL(preDeadBandChanged(double)));
	connect(postDeadBand_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(postDeadBand_, SIGNAL(valueChanged(double)), this, SIGNAL(postDeadBandChanged(double)));
	connect(maxRetries_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(maxRetries_, SIGNAL(valueChanged(double)), this, SIGNAL(maxRetriesChanged(double)));
	connect(encoderPercentApproach_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderPercentApproach_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderPercentApproachChanged(double)));
	connect(encoderStepSoftRatio_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderStepSoftRatio_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderStepSoftRatioChanged(double)));

	connect( stepMotorFeedback_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)) );
}

bool CLSMAXvMotor::isConnected() const{
	bool coreFunctions = true;
	bool encoderFunctions = true;
	coreFunctions = AMPVwStatusControl::isConnected()
			&& EGUVelocity_->isConnected()
			&& EGUBaseVelocity_->isConnected()
			&& EGUAcceleration_->isConnected()
			&& EGUCurrentVelocity_->isConnected()
			&& EGUSetPosition_->isConnected()
			&& EGUOffset_->isConnected()
			&& step_->isConnected()
			&& stepVelocity_->isConnected()
			&& stepBaseVelocity_->isConnected()
			&& stepAcceleration_->isConnected()
			&& stepCurrentVelocity_->isConnected()
			&& cwLimit_->isConnected()
			&& ccwLimit_->isConnected()
			&& powerState_->isConnected()
			&& powerInverted_->isConnected()
			&& stepCalibrationSlope_->isConnected()
			&& stepCalibrationOffset_->isConnected()
			&& motorType_->isConnected()
			&& limitActiveState_->isConnected()
			&& limitDisabled_->isConnected()
			&& stepMotorFeedback_->isConnected();
	if(hasEncoder_)
		encoderFunctions = encoderCalibrationSlope_->isConnected()
			&& encoderCalibrationOffset_->isConnected()
			&& encoderCalibrationAbsoluteOffset_->isConnected()
			&& closedLoopEnabled_->isConnected()
			&& servoPIDEnabled_->isConnected()
			&& encoderTarget_->isConnected()
			&& encoderMovementType_->isConnected()
			&& preDeadBand_->isConnected()
			&& postDeadBand_->isConnected()
			&& maxRetries_->isConnected()
			&& encoderPercentApproach_->isConnected()
			&& encoderStepSoftRatio_->isConnected();
	return coreFunctions && encoderFunctions;
}

QString CLSMAXvMotor::pvBaseName() const {
	return pvBaseName_;
}

QString CLSMAXvMotor::readPVName() const {
	return readPV_->pvName();
}

QString CLSMAXvMotor::writePVName() const {
	return writePV_->pvName();
}

QString CLSMAXvMotor::CWPVName() const {
	return cwLimit_->readPVName();
}

QString CLSMAXvMotor::CCWPVName() const {
	return ccwLimit_->readPVName();
}

QString CLSMAXvMotor::statusPVName() const {
	return statusPVControl_->readPVName();
}

double CLSMAXvMotor::EGUVelocity() const{
	if(isConnected())
		return EGUVelocity_->value();

	return 0.0;
}

double CLSMAXvMotor::EGUBaseVelocity() const{
	if(isConnected())
		return EGUBaseVelocity_->value();

	return 0.0;
}

double CLSMAXvMotor::EGUAcceleration() const{
	if(isConnected())
		return EGUAcceleration_->value();

	return 0.0;
}

double CLSMAXvMotor::EGUCurrentVelocity() const{
	if(isConnected())
		return EGUCurrentVelocity_->value();

	return 0.0;
}

double CLSMAXvMotor::EGUSetPosition() const{
	if(isConnected())
		return EGUSetPosition_->value();
	return 0.0;
}

double CLSMAXvMotor::EGUOffset() const{
	if(isConnected())
		return EGUOffset_->value();
	return 0.0;
}

double CLSMAXvMotor::step() const
{
	if (isConnected())
		return step_->value();

	return 0.5;
}

double CLSMAXvMotor::stepVelocity() const{
	if(isConnected())
		return stepVelocity_->value();

	return 0.5;
}

double CLSMAXvMotor::stepBaseVelocity() const{
	if(isConnected())
		return stepBaseVelocity_->value();

	return 0.5;
}

double CLSMAXvMotor::stepAcceleration() const{
	if(isConnected())
		return stepAcceleration_->value();

	return 0.5;
}

double CLSMAXvMotor::stepCurrentVelocity() const{
	if(isConnected())
		return stepCurrentVelocity_->value();

	return 0.5;
}

bool CLSMAXvMotor::usingKill() const{
	return usingKill_;
}

CLSMAXvMotor::Limit CLSMAXvMotor::atLimit() const{
	if(!isConnected())
		return CLSMAXvMotor::LimitError;
	// I would love to check withinTolerance(1) ... but I only really know 0 means not at limit
	if(cwLimit_->withinTolerance(0) && ccwLimit_->withinTolerance(0))
		return CLSMAXvMotor::LimitNone;
	else if(cwLimit_->withinTolerance(0))
		return CLSMAXvMotor::LimitCCW;
	else if(ccwLimit_->withinTolerance(0))
		return CLSMAXvMotor::LimitCW;
	else
		return CLSMAXvMotor::LimitError;
}

CLSMAXvMotor::PowerState CLSMAXvMotor::powerState() const{
	if(!isConnected())
		return CLSMAXvMotor::PowerError;
	int enumChoice = (int)powerState_->value();
	switch(enumChoice)
	{
	case 0:
		return CLSMAXvMotor::PowerOff;
		break;
	case 1:
		return CLSMAXvMotor::PowerOn;
		break;
	case 2:
		return CLSMAXvMotor::PowerAutoHardware;
		break;
	case 3:
		return CLSMAXvMotor::PowerAutoSoftware;
		break;
	default:
		return CLSMAXvMotor::PowerError;
	}
}

bool CLSMAXvMotor::isPowerInverted() const{
	if(!isConnected())
		return false;
	if(powerInverted_->withinTolerance(0))
		return false;
	return true;
}

double CLSMAXvMotor::encoderCalibrationSlope() const{
	if(isConnected())
		return encoderCalibrationSlope_->value();
	return 0.0;
}

double CLSMAXvMotor::stepCalibrationSlope() const{
	if(isConnected())
		return stepCalibrationSlope_->value();
	return 0.0;
}

double CLSMAXvMotor::encoderCalibrationOffset() const{
	if(isConnected())
		return encoderCalibrationOffset_->value();
	return 0.0;
}

double CLSMAXvMotor::encoderCalibrationAbsoluteOffset() const {
	if (isConnected())
		return encoderCalibrationAbsoluteOffset_->value();

	return 0.0;
}

double CLSMAXvMotor::stepCalibrationOffset() const{
	if(isConnected())
		return stepCalibrationOffset_->value();
	return 0.0;
}

CLSMAXvMotor::MotorType CLSMAXvMotor::motorType() const{
	if(!isConnected())
		return CLSMAXvMotor::MotorTypeError;
	int enumChoice = (int)motorType_->value();
	switch(enumChoice){
	case 0:
		return CLSMAXvMotor::MotorStepperOnly;
		break;
	case 1:
		return CLSMAXvMotor::MotorStepperAndEncoder;
		break;
	case 2:
		return CLSMAXvMotor::MotorServo;
		break;
	default:
		return CLSMAXvMotor::MotorTypeError;
		break;

	}
}

CLSMAXvMotor::LimitActiveState CLSMAXvMotor::limitActiveState() const{
	if(!isConnected())
		return CLSMAXvMotor::LimitActiveError;
	int enumChoice = (int)limitActiveState_->value();
	switch(enumChoice){
	case 0:
		return CLSMAXvMotor::LimitActiveLow;
		break;
	case 1:
		return CLSMAXvMotor::LimitActiveHigh;
		break;
	default:
		return CLSMAXvMotor::LimitActiveError;
		break;
	}
}

bool CLSMAXvMotor::limitDisabled() const{
	if(!isConnected())
		return false;
	if(limitDisabled_->withinTolerance(0))
		return false;
	else
		return true;
}

bool CLSMAXvMotor::closedLoopEnabled() const{
	if(!isConnected())
		return false;
	if(closedLoopEnabled_->withinTolerance(0))
		return false;
	else
		return true;
}

bool CLSMAXvMotor::servoPIDEnabled() const{
	if(!isConnected())
		return false;
	if(servoPIDEnabled_->withinTolerance(0))
		return false;
	else
		return true;
}

double CLSMAXvMotor::encoderTarget() const{
	if(isConnected())
		return encoderTarget_->value();
	return 0.5;
}

CLSMAXvMotor::EncoderMovementType CLSMAXvMotor::encoderMovementType() const{
	if(!isConnected())
		return CLSMAXvMotor::EncoderMovementTypeError;
	int enumChoice = (int)encoderMovementType_->value();
	switch(enumChoice){
	case 0:
		return CLSMAXvMotor::EncoderMovementOff;
		break;
	case 1:
		return CLSMAXvMotor::EncoderMovementIncreaseOnly;
		break;
	case 2:
		return CLSMAXvMotor::EncoderMovementDecreaseOnly;
		break;
	case 3:
		return CLSMAXvMotor::EncoderMovementIncrease;
		break;
	case 4:
		return CLSMAXvMotor::EncoderMovementDecrease;
		break;
	default:
		return CLSMAXvMotor::EncoderMovementTypeError;
		break;
	}
}

double CLSMAXvMotor::preDeadBand() const{
	if(isConnected())
		return preDeadBand_->value();
	return 0.5;
}

double CLSMAXvMotor::postDeadBand() const{
	if(isConnected())
		return postDeadBand_->value();
	return 0.5;
}

double CLSMAXvMotor::maxRetries() const{
	if(isConnected())
		return maxRetries_->value();
	return 0.5;
}

double CLSMAXvMotor::encoderPercentApproach() const{
	if(isConnected())
		return encoderPercentApproach_->value();
	return -1.0;
}

double CLSMAXvMotor::encoderStepSoftRatio() const{
	if(isConnected())
		return encoderStepSoftRatio_->value();
	return 0;
}

AMReadOnlyPVControl *CLSMAXvMotor::statusPVControl(){
	return statusPVControl_;
}

AMAction3 *CLSMAXvMotor::createMotorMoveAction(double position)
{
	return createEGUMoveAction(position);
}

AMAction3 *CLSMAXvMotor::createEGUMoveAction(double EGUPosition)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(this, EGUPosition);
}

AMAction3 *CLSMAXvMotor::createEGUVelocityAction(double EGUVelocity)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(EGUVelocity_, EGUVelocity);
}

AMAction3 *CLSMAXvMotor::createEGUBaseVelocityAction(double EGUBaseVelocity)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(EGUBaseVelocity_, EGUBaseVelocity);
}

AMAction3 *CLSMAXvMotor::createEGUAccelerationAction(double EGUAcceleration)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(EGUAcceleration_, EGUAcceleration);
}

AMAction3 *CLSMAXvMotor::createEGUSetPositionAction(double EGUSetPosition)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(EGUSetPosition_, EGUSetPosition);
}

AMAction3 *CLSMAXvMotor::createEGUOffsetAction(double EGUOffset)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(EGUOffset_, EGUOffset);
}

AMAction3 *CLSMAXvMotor::createStepMoveAction(double stepPosition)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(step_, stepPosition);
}

AMAction3 *CLSMAXvMotor::createStepVelocityAction(double stepVelocity)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(stepVelocity_, stepVelocity);
}

AMAction3 *CLSMAXvMotor::createStepBaseVelocityAction(double stepBaseVelocity)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(stepBaseVelocity_, stepBaseVelocity);
}

AMAction3 *CLSMAXvMotor::createStepAccelerationAction(double stepAcceleration)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(stepAcceleration_, stepAcceleration);
}

AMAction3 *CLSMAXvMotor::createStopAction()
{
	if(!isConnected())
		return 0;

	AMControlInfo info = toInfo();
	AMControlStopActionInfo *actionInfo = new AMControlStopActionInfo(info);
	AMAction3 *action = new AMControlStopAction(actionInfo, this);

	return action;
}

AMAction3 *CLSMAXvMotor::createEncoderCalibrationSlopeAction(double encoderCalibrationSlope)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderCalibrationSlope_, encoderCalibrationSlope);
}

AMAction3 *CLSMAXvMotor::createStepCalibrationSlopeAction(double stepCalibrationSlope)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(stepCalibrationSlope_, stepCalibrationSlope);
}

AMAction3 *CLSMAXvMotor::createEncoderCalibrationOffsetAction(double encoderCalibrationOffset)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderCalibrationOffset_, encoderCalibrationOffset);
}

AMAction3 *CLSMAXvMotor::createEncoderCalibrationAbsoluteOffsetAction(double newAbsoluteOffset)
{
	if (!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderCalibrationAbsoluteOffset_, newAbsoluteOffset);
}

AMAction3 *CLSMAXvMotor::createStepCalibrationOffsetAction(double stepCalibrationOffset)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(stepCalibrationOffset_, stepCalibrationOffset);
}


AMAction3 *CLSMAXvMotor::createMotorTypeAction(CLSMAXvMotor::MotorType motorType)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(motorType_, double(motorType));
}

AMAction3 *CLSMAXvMotor::createLimitActiveStateAction(CLSMAXvMotor::LimitActiveState limitActiveState)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(limitActiveState_, double(limitActiveState));
}

AMAction3 *CLSMAXvMotor::createLimitDisabledAction(bool limitDisabled)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(limitDisabled_, limitDisabled ? 1.0 : 0.0);
}

AMAction3 *CLSMAXvMotor::createClosedLoopEnabledAction(bool closedLoopEnabled)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(closedLoopEnabled_, closedLoopEnabled ? 1.0 : 0.0);
}

AMAction3 *CLSMAXvMotor::createServoPIDEnabledAction(bool servoPIDEnabled)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(servoPIDEnabled_, servoPIDEnabled ? 1.0 : 0.0);
}

AMAction3 *CLSMAXvMotor::createEncoderMoveAction(double encoderPosition)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderTarget_, encoderPosition);
}

AMAction3 *CLSMAXvMotor::createEncoderMovementTypeAction(CLSMAXvMotor::EncoderMovementType encoderMovementType)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderMovementType_, double(encoderMovementType));
}

AMAction3 *CLSMAXvMotor::createPreDeadBandAction(double preDeadBand)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(preDeadBand_, preDeadBand);
}

AMAction3 *CLSMAXvMotor::createPostDeadBandAction(double postDeadBand)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(postDeadBand_, postDeadBand);
}

AMAction3 *CLSMAXvMotor::createMaxRetriesAction(double maxRetries)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(maxRetries_, maxRetries);
}

AMAction3 *CLSMAXvMotor::createEncoderPercentApproachAction(double encoderPercentApproach)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderPercentApproach_, encoderPercentApproach);
}

AMAction3 *CLSMAXvMotor::createEncoderStepSoftRatioAction(double encoderStepSoftRatio)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderStepSoftRatio_, encoderStepSoftRatio);
}

AMAction3 *CLSMAXvMotor::createPowerAction(CLSMAXvMotor::PowerState newState)
{
	AMAction3 *action = 0;

	if (powerState_->isConnected()) {
		action = AMActionSupport::buildControlMoveAction(powerState_, newState);
	}

	return action;
}

AMAction3 *CLSMAXvMotor::createCCWLimitWaitAction(CLSMAXvMotor::Limit ccwLimitState)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlWaitAction(ccwLimit_, ccwLimitState);

}

AMAction3 *CLSMAXvMotor::createCWLimitWaitAction(CLSMAXvMotor::Limit cwLimitState)
{
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlWaitAction(cwLimit_, cwLimitState);

}

AMAction3 *CLSMAXvMotor::createCalibrationAction(double oldPosition, double newPosition)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *calibrationAction = new AMListAction3(new AMListActionInfo3("Motor calibration", "Motor calibration"), AMListAction3::Sequential);
		calibrationAction->addSubAction(AMActionSupport::buildControlMoveAction(this, oldPosition));
		calibrationAction->addSubAction(AMActionSupport::buildControlMoveAction(EGUSetPosition_, newPosition));

		result = calibrationAction;
	}

	return result;
}

QString CLSMAXvMotor::limitToString(CLSMAXvMotor::Limit limit) const
{
	switch (limit) {
	case CLSMAXvMotor::LimitNone:
		return "None";
	case CLSMAXvMotor::LimitCW:
		return "Clockwise";
	case CLSMAXvMotor::LimitCCW:
		return "Counter-clockwise";
	case CLSMAXvMotor::LimitError:
		return "Error";
	default:
		return "Unknown";
	}
}

AMControl::FailureExplanation CLSMAXvMotor::calibrate(double oldValue, double newValue)
{
	// Check that this motor is connected and able to be calibrated before proceeding.

	if (!isConnected()) {
		AMErrorMon::alert(this, CLSMAXVMOTOR_NOT_CONNECTED, QString("Failed to calibrate %1: motor is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!canCalibrate()) {
		AMErrorMon::alert(this, CLSMAXVMOTOR_CANNOT_CALIBRATE, QString("Failed to calibrate %1: motor cannot currently be calibrated.").arg(name()));
		return AMControl::OtherFailure;
	}

	// Proceed with creating calibration action.

	AMAction3 *action = createCalibrationAction(oldValue, newValue);

	// Check that a valid calibration action was generated.
	// If an invalid calibration action was generated, abort the calibration.

	if (!action) {
		AMErrorMon::alert(this, CLSMAXVMOTOR_INVALID_CALIBRATION_ACTION, QString("Did not calibrate %1: invalid calibration action generated.").arg(name()));
		return AMControl::LimitFailure;
	}

	// Proceed with initializing the calibration action.
	// Connect it's final-state signals to its deleteLater() slot to prevent memory leak.

	connect( action, SIGNAL(cancelled()), action, SLOT(deleteLater()) );
	connect( action, SIGNAL(failed()), action, SLOT(deleteLater()) );
	connect( action, SIGNAL(succeeded()), action, SLOT(deleteLater()) );

	// Run action.

	action->start();

	return AMControl::NoFailure;
}

void CLSMAXvMotor::setEGUVelocity(double velocity){
	if(isConnected())
		EGUVelocity_->move(velocity);
}

void CLSMAXvMotor::setEGUBaseVelocity(double baseVelocity){
	if(isConnected())
		EGUBaseVelocity_->move(baseVelocity);
}

void CLSMAXvMotor::setEGUAcceleration(double acceleration){
	if(isConnected())
		EGUAcceleration_->move(acceleration);
}

void CLSMAXvMotor::setEGUSetPosition(double EGUSetPosition){
	if(isConnected())
		EGUSetPosition_->move(EGUSetPosition);
}

void CLSMAXvMotor::setEGUOffset(double EGUOffset){
	if(isConnected())
		EGUOffset_->move(EGUOffset);
}

void CLSMAXvMotor::setStep(double step)
{
	if (isConnected())
		step_->move(step);
}

void CLSMAXvMotor::setStepVelocity(double velocity){
	if(isConnected())
		stepVelocity_->move(velocity);
}

void CLSMAXvMotor::setStepBaseVelocity(double baseVelocity){
	if(isConnected())
		stepBaseVelocity_->move(baseVelocity);
}

void CLSMAXvMotor::setStepAcceleration(double acceleration){
	if(isConnected())
		stepAcceleration_->move(acceleration);
}

void CLSMAXvMotor::setUsingKill(bool usingKill){
	usingKill_ = usingKill;
}

bool CLSMAXvMotor::stop(){
	if(!canStop())
		return false;

	if(usingKill_)
		killPV_->setValue(stopValue_);
	else
		stopPV_->setValue(stopValue_);

	stopInProgress_ = true;	// flag that a stop is "in progress" -- we've issued the stop command.
	moveInProgress_ = false;	// one of "our" moves is no longer in progress.
	return true;
}

void CLSMAXvMotor::setEncoderCalibrationSlope(double encoderCalibrationSlope){
	if(isConnected())
		encoderCalibrationSlope_->move(encoderCalibrationSlope);
}

void CLSMAXvMotor::setStepCalibrationSlope(double stepCalibrationSlope){
	if(isConnected())
		stepCalibrationSlope_->move(stepCalibrationSlope);
}

void CLSMAXvMotor::setEncoderCalibrationOffset(double encoderCalibrationOffset){
	if(isConnected())
		encoderCalibrationOffset_->move(encoderCalibrationOffset);
}

void CLSMAXvMotor::setEncoderCalibrationAbsoluteOffset(double encoderCalibrationAbsoluteOffset) {
	if (isConnected())
		encoderCalibrationAbsoluteOffset_->move(encoderCalibrationAbsoluteOffset);
}

void CLSMAXvMotor::setStepCalibrationOffset(double stepCalibrationOffset){
	if(isConnected())
		stepCalibrationOffset_->move(stepCalibrationOffset);
}

void CLSMAXvMotor::setMotorType(CLSMAXvMotor::MotorType motorType){
	if(isConnected())
		motorType_->move(motorType);
}

void CLSMAXvMotor::setLimitActiveState(CLSMAXvMotor::LimitActiveState limitActiveState){
	if(isConnected())
		limitActiveState_->move(limitActiveState);
}

void CLSMAXvMotor::setLimitDisabled(bool disabled){
	if(isConnected()){
		if(disabled)
			limitDisabled_->move(1);
		else
			limitDisabled_->move(0);
	}
}

void CLSMAXvMotor::setClosedLoopEnabled(bool closedLoopEnabled){
	if(isConnected()){
		if(closedLoopEnabled)
			closedLoopEnabled_->move(1);
		else
			closedLoopEnabled_->move(0);
	}
}

void CLSMAXvMotor::setServoPIDEnabled(bool servoPIDEnabled){
	if(isConnected()){
		if(servoPIDEnabled)
			servoPIDEnabled_->move(1);
		else
			servoPIDEnabled_->move(0);
	}
}

void CLSMAXvMotor::setEncoderTarget(double encoderTarget){
	if(isConnected())
		encoderTarget_->move(encoderTarget);
}

void CLSMAXvMotor::setEncoderMovementType(CLSMAXvMotor::EncoderMovementType encoderMovementType){
	if(isConnected())
		encoderMovementType_->move(encoderMovementType);
}

void CLSMAXvMotor::setPreDeadBand(double preDeadBand){
	if(isConnected())
		preDeadBand_->move(preDeadBand);
}

void CLSMAXvMotor::setPostDeadBand(double postDeadBand){
	if(isConnected())
		postDeadBand_->move(postDeadBand);
}

void CLSMAXvMotor::setMaxRetries(double maxRetries){
	if(isConnected())
		maxRetries_->move(maxRetries);
}

void CLSMAXvMotor::setEncoderPercentApproach(double encoderPercentApproach){
	if(isConnected())
		encoderPercentApproach_->move(encoderPercentApproach);
}

void CLSMAXvMotor::setEncoderStepSoftRatio(double encoderStepSoftRatio){
	if(isConnected())
		encoderStepSoftRatio_->move(encoderStepSoftRatio);
}

AMControl::FailureExplanation CLSMAXvMotor::move(double setpoint){
	CLSMAXvMotor::Limit limitCondition = atLimit();

	if(limitCondition == CLSMAXvMotor::LimitNone)
		return AMPVwStatusControl::move(setpoint);
	else if(limitCondition == CLSMAXvMotor::LimitError)
		return AMControl::LimitFailure;

	double currentPosition = value();

	bool positiveSlope = false;
	if(stepCalibrationSlope_->value() > 0)
		positiveSlope = true;

	bool positiveMovement = false;
	if(setpoint > currentPosition)
		positiveMovement = true;

	bool canMoveAwayFromLimit = false;
	if(limitCondition == CLSMAXvMotor::LimitCW && positiveMovement && !positiveSlope)
		canMoveAwayFromLimit = true;
	else if(limitCondition == CLSMAXvMotor::LimitCW && !positiveMovement && positiveSlope)
		canMoveAwayFromLimit = true;
	else if(limitCondition == CLSMAXvMotor::LimitCCW && !positiveMovement && !positiveSlope)
		canMoveAwayFromLimit = true;
	else if(limitCondition == CLSMAXvMotor::LimitCCW && positiveMovement && positiveSlope)
		canMoveAwayFromLimit = true;

	if(!canMoveAwayFromLimit)
		return AMControl::LimitFailure;

	settlingInProgress_ = false;
	stopInProgress_ = false;
	moveInProgress_ = false;
	// Flag that "our" move started:
	startInProgress_ = true;

	// This is our new target:
	setpoint_ = setpoint;

	// Update the move progress values.

	updateMoveStart();
	updateMoveValue();
	updateMoveEnd();

	// Normal move:
	// Issue the move command, check on attemptMoveWhenWithinTolerance
	if(!attemptMoveWhenWithinTolerance_ && inPosition())
		emit moveSucceeded();
	else{
		writePV_->setValue(setpoint_);
		// start the timer to check if our move failed to start:
		moveStartTimer_.start(int(moveStartTimeout_*1000.0));
	}

	return NoFailure;
}

void CLSMAXvMotor::onLimitsChanged(double value){
	Q_UNUSED(value)
	// I would love to check withinTolerance(1) ... but I only really know 0 means not at limit
	if(cwLimit_->withinTolerance(0) && ccwLimit_->withinTolerance(0))
		emit atLimitChanged(CLSMAXvMotor::LimitNone);
	else if(cwLimit_->withinTolerance(0))
		emit atLimitChanged(CLSMAXvMotor::LimitCCW);
	else if(ccwLimit_->withinTolerance(0))
		emit atLimitChanged(CLSMAXvMotor::LimitCW);
	else
		emit atLimitChanged(CLSMAXvMotor::LimitError);
}

void CLSMAXvMotor::onPowerStateChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice)
	{
	case 0:
		emit powerStatedChanged(CLSMAXvMotor::PowerOff);
		break;
	case 1:
		emit powerStatedChanged(CLSMAXvMotor::PowerOn);
		break;
	case 2:
		emit powerStatedChanged(CLSMAXvMotor::PowerAutoHardware);
		break;
	case 3:
		emit powerStatedChanged(CLSMAXvMotor::PowerAutoSoftware);
		break;
	default:
		emit powerStatedChanged(CLSMAXvMotor::PowerError);
	}
}

void CLSMAXvMotor::onPowerInvertedChanged(double value){
	Q_UNUSED(value)
	if(powerInverted_->withinTolerance(0))
		emit powerInvertedChanged(false);
	else
		emit powerInvertedChanged(true);
}

void CLSMAXvMotor::onMotorTypeChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit motorTypeChanged(CLSMAXvMotor::MotorStepperOnly);
		break;
	case 1:
		emit motorTypeChanged(CLSMAXvMotor::MotorStepperAndEncoder);
		break;
	case 2:
		emit motorTypeChanged(CLSMAXvMotor::MotorServo);
		break;
	default:
		emit motorTypeChanged(CLSMAXvMotor::MotorTypeError);
		break;
	}
}

void CLSMAXvMotor::onLimitActiveStateChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit limitActiveStateChanged(CLSMAXvMotor::LimitActiveLow);
		break;
	case 1:
		emit limitActiveStateChanged(CLSMAXvMotor::LimitActiveHigh);
		break;
	default:
		emit limitActiveStateChanged(CLSMAXvMotor::LimitActiveError);
		break;
	}
}

void CLSMAXvMotor::onLimitDisabledChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit limitDisabledChanged(false);
		break;
	case 1:
		emit limitDisabledChanged(true);
		break;
	}
}

void CLSMAXvMotor::onClosedLoopEnabledChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit closedLoopEnabledChanged(false);
		break;
	case 1:
		emit closedLoopEnabledChanged(true);
		break;
	}
}

void CLSMAXvMotor::onServoPIDEnabledChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit servoPIDEnabledChanged(false);
		break;
	case 1:
		emit servoPIDEnabledChanged(true);
		break;
	}
}

void CLSMAXvMotor::onEncoderMovementTypeChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementOff);
		break;
	case 1:
		emit encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementIncreaseOnly);
		break;
	case 2:
		emit encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementDecreaseOnly);
		break;
	case 3:
		emit encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementIncrease);
		break;
	case 4:
		emit encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementDecrease);
		break;
	default:
		emit encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementTypeError);
		break;
	}

}
 CLSMAXvControlStatusChecker::~CLSMAXvControlStatusChecker(){}
