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

#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMControlStopAction.h"

 CLSMAXvMotor::~CLSMAXvMotor(){}
CLSMAXvMotor::CLSMAXvMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds, QString pvUnitFieldName, QObject *parent) :
	AMPVwStatusControl(name, hasEncoder ? baseName+pvUnitFieldName+":fbk" : baseName+pvUnitFieldName+":sp", baseName+pvUnitFieldName, baseName+":status", baseName+":stop", parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerCLSMAXv(), 1, description)
		//AMPVwStatusControl(name, baseName+":mm:fbk", baseName+":mm", baseName+":status", baseName+":stop", parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerStopped(0), 1, description)
{
	hasEncoder_ = hasEncoder;
	usingKill_ = false;
	killPV_ = new AMProcessVariable(baseName+":kill", true, this);

	EGUVelocity_ = new AMPVControl(name+"EGUVelocity", baseName+":vel:mmps:sp", baseName+":velo:mmps", QString(), this, 0.05);
	EGUBaseVelocity_ = new AMPVControl(name+"EGUBaseVelocity", baseName+":vBase:mmps:sp", baseName+":vBase:mmps", QString(), this, 0.05);
	EGUAcceleration_ = new AMPVControl(name+"EGUAcceleration", baseName+":acc:mmpss:sp", baseName+":accel:mmpss", QString(), this, 2);
	EGUCurrentVelocity_ = new AMReadOnlyPVControl(name+"EGUCurrentVelocity", baseName+":vel:mmps:fbk", this);
	EGUOffset_ = new AMPVControl(name+"EGUOffset", baseName+":mm:offset", baseName+":mm:offset", QString(), this, 0.005);

	step_ = new AMPVControl(name+"Step", baseName+":step:sp", baseName+":step", QString(), this, 20);
	stepVelocity_ = new AMPVControl(name+"StepVelocity", baseName+":velo:sp", baseName+":velo", QString(), this, 2);
	stepBaseVelocity_ = new AMPVControl(name+"StepBaseVelocity", baseName+":vBase:sp", baseName+":veloBase", QString(), this, 2);
	stepAcceleration_ = new AMPVControl(name+"StepAcceleration", baseName+":accel:sp", baseName+":accel", QString(), this, 2);
	stepCurrentVelocity_ = new AMReadOnlyPVControl(name+"StepCurrentVelocity", baseName+":velo:fbk", this);

	cwLimit_ = new AMReadOnlyPVControl(name+"CWLimit", baseName+":cw", this);
	cwLimit_->setTolerance(0.1);
	ccwLimit_ = new AMReadOnlyPVControl(name+"CCWLimit", baseName+":ccw", this);
	ccwLimit_->setTolerance(0.1);

	powerState_ = new AMPVControl(name+"PowerState", baseName+":power", baseName+":power", QString(), this, 0.1);
	powerInverted_ = new AMPVControl(name+"PowerInverted", baseName+":invertPower", baseName+":invertPower", QString(), this, 0.1);

	encoderCalibrationSlope_ = new AMPVControl(name+"EncoderCalibrationSlope", baseName+":enc:slope", baseName+":enc:slope", QString(), this, 0.00001);
	stepCalibrationSlope_ = new AMPVControl(name+"StepCalibrationSlope", baseName+":step:slope", baseName+":step:slope", QString(), this, 0.00001);
	encoderCalibrationOffset_ = new AMPVControl(name+"EncoderCalibrationOffset", baseName+":enc:offset", baseName+":enc:offset", QString(), this, 0.001);
	stepCalibrationOffset_ = new AMPVControl(name+"StepCalibrationOffset", baseName+":step:offset", baseName+":step:offset", QString(), this, 0.001);

	motorType_ = new AMPVControl(name+"MotorType", baseName+":motorType:sp", baseName+":motorType", QString(), this, 0.1);

	limitActiveState_ = new AMPVControl(name+"LimitActiveState", baseName+":limitHigh", baseName+":limitHigh", QString(), this, 0.1);
	limitDisabled_ = new AMPVControl(name+"LimitDisabled", baseName+":disableLimits", baseName+":disableLimits", QString(), this, 0.1);

	closedLoopEnabled_ = new AMPVControl(name+"ClosedLoopEnabled", baseName+":closedLoop", baseName+":closedLoop", QString(), this, 0.1);
	servoPIDEnabled_ = new AMPVControl(name+"ServoPIDEnabled", baseName+":hold:sp", baseName+":hold", QString(), this, 0.1);

	encoderTarget_ = new AMPVwStatusControl(name+"EncoderTarget", baseName+":enc:fbk", baseName+":encTarget", baseName+":status", QString(), this, 10, 2.0, new AMControlStatusCheckerCLSMAXv(), 1);
	encoderMovementType_ = new AMPVControl(name+"EncoderMovementType", baseName+":encMoveType", baseName+":selEncMvType", QString(), this, 0.1);
	preDeadBand_ = new AMPVControl(name+"PreDeadBand", baseName+":preDBand", baseName+":preDBand", QString(), this, 1);
	postDeadBand_ = new AMPVControl(name+"PostDeadBand", baseName+":postDBand", baseName+":postDBand", QString(), this, 1);
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
}

bool CLSMAXvMotor::isConnected() const{
	bool coreFunctions = true;
	bool encoderFunctions = true;
	coreFunctions = AMPVwStatusControl::isConnected()
			&& EGUVelocity_->isConnected()
			&& EGUBaseVelocity_->isConnected()
			&& EGUAcceleration_->isConnected()
			&& EGUCurrentVelocity_->isConnected()
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
			&& limitDisabled_->isConnected();
	if(hasEncoder_)
		encoderFunctions = encoderCalibrationSlope_->isConnected()
			&& encoderCalibrationOffset_->isConnected()
			&& encoderCalibrationOffset_->isConnected()
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

AMAction3 *CLSMAXvMotor::createMotorMoveAction(double position)
{
	return createEGUMoveAction(position);
}

AMAction3 *CLSMAXvMotor::createEGUMoveAction(double EGUPosition)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = toInfo();
	setpoint.setValue(EGUPosition);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, this);

	return action;
}

AMAction3 *CLSMAXvMotor::createEGUVelocityAction(double EGUVelocity)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = EGUVelocity_->toInfo();
	setpoint.setValue(EGUVelocity);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, EGUVelocity_);

	return action;
}

AMAction3 *CLSMAXvMotor::createEGUBaseVelocityAction(double EGUBaseVelocity)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = EGUBaseVelocity_->toInfo();
	setpoint.setValue(EGUBaseVelocity);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, EGUBaseVelocity_);

	return action;
}

AMAction3 *CLSMAXvMotor::createEGUAccelerationAction(double EGUAcceleration)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = EGUAcceleration_->toInfo();
	setpoint.setValue(EGUAcceleration);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, EGUAcceleration_);

	return action;
}

AMAction3 *CLSMAXvMotor::createEGUOffsetAction(double EGUOffset)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = EGUOffset_->toInfo();
	setpoint.setValue(EGUOffset);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, EGUOffset_);

	return action;
}

AMAction3 *CLSMAXvMotor::createStepMoveAction(double stepPosition)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = step_->toInfo();
	setpoint.setValue(stepPosition);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, step_);

	return action;
}

AMAction3 *CLSMAXvMotor::createStepVelocityAction(double stepVelocity)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = stepVelocity_->toInfo();
	setpoint.setValue(stepVelocity);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, stepVelocity_);

	return action;
}

AMAction3 *CLSMAXvMotor::createStepBaseVelocityAction(double stepBaseVelocity)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = stepBaseVelocity_->toInfo();
	setpoint.setValue(stepBaseVelocity);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, stepBaseVelocity_);

	return action;
}

AMAction3 *CLSMAXvMotor::createStepAccelerationAction(double stepAcceleration)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = stepAcceleration_->toInfo();
	setpoint.setValue(stepAcceleration);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, stepAcceleration_);

	return action;
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

	AMControlInfo setpoint = encoderCalibrationSlope_->toInfo();
	setpoint.setValue(encoderCalibrationSlope);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, encoderCalibrationSlope_);

	return action;
}

AMAction3 *CLSMAXvMotor::createStepCalibrationSlopeAction(double stepCalibrationSlope)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = stepCalibrationSlope_->toInfo();
	setpoint.setValue(stepCalibrationSlope);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, stepCalibrationSlope_);

	return action;
}

AMAction3 *CLSMAXvMotor::createEncoderCalibrationOffsetAction(double encoderCalibrationOffset)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = encoderCalibrationOffset_->toInfo();
	setpoint.setValue(encoderCalibrationOffset);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, encoderCalibrationOffset_);

	return action;
}

AMAction3 *CLSMAXvMotor::createStepCalibrationOffsetAction(double stepCalibrationOffset)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = stepCalibrationOffset_->toInfo();
	setpoint.setValue(stepCalibrationOffset);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, stepCalibrationOffset_);

	return action;
}


AMAction3 *CLSMAXvMotor::createMotorTypeAction(CLSMAXvMotor::MotorType motorType)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = motorType_->toInfo();
	setpoint.setValue(double(motorType));
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, motorType_);

	return action;
}

AMAction3 *CLSMAXvMotor::createLimitActiveStateAction(CLSMAXvMotor::LimitActiveState limitActiveState)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = limitActiveState_->toInfo();
	setpoint.setValue(double(limitActiveState));
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, limitActiveState_);

	return action;
}

AMAction3 *CLSMAXvMotor::createLimitDisabledAction(bool limitDisabled)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = limitDisabled_->toInfo();
	setpoint.setValue(limitDisabled ? 1.0 : 0.0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, limitDisabled_);

	return action;
}

AMAction3 *CLSMAXvMotor::createClosedLoopEnabledAction(bool closedLoopEnabled)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = closedLoopEnabled_->toInfo();
	setpoint.setValue(closedLoopEnabled ? 1.0 : 0.0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, closedLoopEnabled_);

	return action;
}

AMAction3 *CLSMAXvMotor::createServoPIDEnabledAction(bool servoPIDEnabled)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = servoPIDEnabled_->toInfo();
	setpoint.setValue(servoPIDEnabled ? 1.0 : 0.0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, servoPIDEnabled_);

	return action;
}

AMAction3 *CLSMAXvMotor::createEncoderMoveAction(double encoderPosition)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = encoderTarget_->toInfo();
	setpoint.setValue(encoderPosition);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, encoderTarget_);

	return action;
}

AMAction3 *CLSMAXvMotor::createEncoderMovementTypeAction(CLSMAXvMotor::EncoderMovementType encoderMovementType)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = encoderMovementType_->toInfo();
	setpoint.setValue(double(encoderMovementType));
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, encoderMovementType_);

	return action;
}

AMAction3 *CLSMAXvMotor::createPreDeadBandAction(double preDeadBand)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = preDeadBand_->toInfo();
	setpoint.setValue(preDeadBand);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, preDeadBand_);

	return action;
}

AMAction3 *CLSMAXvMotor::createPostDeadBandAction(double postDeadBand)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = postDeadBand_->toInfo();
	setpoint.setValue(postDeadBand);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, postDeadBand_);

	return action;
}

AMAction3 *CLSMAXvMotor::createMaxRetriesAction(double maxRetries)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = maxRetries_->toInfo();
	setpoint.setValue(maxRetries);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, maxRetries_);

	return action;
}

AMAction3 *CLSMAXvMotor::createEncoderPercentApproachAction(double encoderPercentApproach)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = encoderPercentApproach_->toInfo();
	setpoint.setValue(encoderPercentApproach);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, encoderPercentApproach_);

	return action;
}

AMAction3 *CLSMAXvMotor::createEncoderStepSoftRatioAction(double encoderStepSoftRatio)
{
	if(!isConnected())
		return 0;

	AMControlInfo setpoint = encoderStepSoftRatio_->toInfo();
	setpoint.setValue(encoderStepSoftRatio);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, encoderStepSoftRatio_);

	return action;
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
 AMControlStatusCheckerCLSMAXv::~AMControlStatusCheckerCLSMAXv(){}
