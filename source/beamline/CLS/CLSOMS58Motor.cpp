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


#include "CLSOMS58Motor.h"

#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/AMBeamlineControlStopAction.h"
/*
 CLSOMS58Motor::~CLSOMS58Motor(){}
CLSOMS58Motor::CLSOMS58Motor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, bool hasEGU, double tolerance, double moveStartTimeoutSeconds, QObject *parent) :
	AMPVwStatusControl(name,
				//hasEncoder ? baseName+":mm:fbk" : baseName+":mm:sp",
				hasEncoder ? hasEGU ? baseName+":mm:fbk" : baseName+":step" : hasEGU ? baseName+":mm:sp" : baseName+":step:sp",
				baseName+":mm", baseName+":status",
				baseName+":stop",
				parent,
				tolerance,
				moveStartTimeoutSeconds,
				new AMControlStatusCheckerCLSMAXv(),
				1,
				description)
{
	hasEncoder_ = hasEncoder;
	hasEGU_ = hasEGU;

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

	encoderCalibrationSlope_ = new AMPVControl(name+"EncoderCalibrationSlope", baseName+":enc:slope", baseName+":enc:slope", QString(), this, 0.00001);
	stepCalibrationSlope_ = new AMPVControl(name+"StepCalibrationSlope", baseName+":step:slope", baseName+":step:slope", QString(), this, 0.00001);
	encoderCalibrationOffset_ = new AMPVControl(name+"EncoderCalibrationOffset", baseName+":enc:offset", baseName+":enc:offset", QString(), this, 0.001);
	stepCalibrationOffset_ = new AMPVControl(name+"StepCalibrationOffset", baseName+":step:offset", baseName+":step:offset", QString(), this, 0.001);

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

	connect(encoderCalibrationSlope_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderCalibrationSlope_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderCalibrationSlopeChanged(double)));
	connect(stepCalibrationSlope_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(stepCalibrationSlope_, SIGNAL(valueChanged(double)), this, SIGNAL(stepCalibrationSlopeChanged(double)));
	connect(encoderCalibrationOffset_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderCalibrationOffset_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderCalibrationOffsetChanged(double)));
	connect(stepCalibrationOffset_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(stepCalibrationOffset_, SIGNAL(valueChanged(double)), this, SIGNAL(stepCalibrationOffsetChanged(double)));

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

bool CLSOMS58Motor::isConnected() const{
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
			&& stepCalibrationSlope_->isConnected()
			&& stepCalibrationOffset_->isConnected();
	if(hasEncoder_)
		encoderFunctions = encoderCalibrationSlope_->isConnected()
			&& encoderCalibrationOffset_->isConnected()
			&& encoderCalibrationOffset_->isConnected()
			&& encoderTarget_->isConnected()
			&& encoderMovementType_->isConnected()
			&& preDeadBand_->isConnected()
			&& postDeadBand_->isConnected()
			&& maxRetries_->isConnected()
			&& encoderPercentApproach_->isConnected()
			&& encoderStepSoftRatio_->isConnected();
	return coreFunctions && encoderFunctions;
}

double CLSOMS58Motor::EGUVelocity() const{
	if(isConnected())
		return EGUVelocity_->value();

	return 0.0;
}

double CLSOMS58Motor::EGUBaseVelocity() const{
	if(isConnected())
		return EGUBaseVelocity_->value();

	return 0.0;
}

double CLSOMS58Motor::EGUAcceleration() const{
	if(isConnected())
		return EGUAcceleration_->value();

	return 0.0;
}

double CLSOMS58Motor::EGUCurrentVelocity() const{
	if(isConnected())
		return EGUCurrentVelocity_->value();

	return 0.0;
}

double CLSOMS58Motor::EGUOffset() const{
	if(isConnected())
		return EGUOffset_->value();
	return 0.0;
}

double CLSOMS58Motor::step() const
{
	if (isConnected())
		return step_->value();

	return 0.5;
}

double CLSOMS58Motor::stepVelocity() const{
	if(isConnected())
		return stepVelocity_->value();

	return 0.5;
}

double CLSOMS58Motor::stepBaseVelocity() const{
	if(isConnected())
		return stepBaseVelocity_->value();

	return 0.5;
}

double CLSOMS58Motor::stepAcceleration() const{
	if(isConnected())
		return stepAcceleration_->value();

	return 0.5;
}

double CLSOMS58Motor::stepCurrentVelocity() const{
	if(isConnected())
		return stepCurrentVelocity_->value();

	return 0.5;
}

bool CLSOMS58Motor::usingKill() const{
	return usingKill_;
}

CLSOMS58Motor::Limit CLSOMS58Motor::atLimit() const{
	if(!isConnected())
		return CLSOMS58Motor::LimitError;
	// I would love to check withinTolerance(1) ... but I only really know 0 means not at limit
	if(cwLimit_->withinTolerance(0) && ccwLimit_->withinTolerance(0))
		return CLSOMS58Motor::LimitNone;
	else if(cwLimit_->withinTolerance(0))
		return CLSOMS58Motor::LimitCCW;
	else if(ccwLimit_->withinTolerance(0))
		return CLSOMS58Motor::LimitCW;
	else
		return CLSOMS58Motor::LimitError;
}

CLSOMS58Motor::PowerState CLSOMS58Motor::powerState() const{
	if(!isConnected())
		return CLSOMS58Motor::PowerError;
	int enumChoice = (int)powerState_->value();
	switch(enumChoice)
	{
	case 0:
		return CLSOMS58Motor::PowerOff;
		break;
	case 1:
		return CLSOMS58Motor::PowerOn;
		break;
	case 2:
		return CLSOMS58Motor::PowerAutoHardware;
		break;
	case 3:
		return CLSOMS58Motor::PowerAutoSoftware;
		break;
	default:
		return CLSOMS58Motor::PowerError;
	}
}

bool CLSOMS58Motor::isPowerInverted() const{
	if(!isConnected())
		return false;
	if(powerInverted_->withinTolerance(0))
		return false;
	return true;
}

double CLSOMS58Motor::encoderCalibrationSlope() const{
	if(isConnected())
		return encoderCalibrationSlope_->value();
	return 0.0;
}

double CLSOMS58Motor::stepCalibrationSlope() const{
	if(isConnected())
		return stepCalibrationSlope_->value();
	return 0.0;
}

double CLSOMS58Motor::encoderCalibrationOffset() const{
	if(isConnected())
		return encoderCalibrationOffset_->value();
	return 0.0;
}

double CLSOMS58Motor::stepCalibrationOffset() const{
	if(isConnected())
		return stepCalibrationOffset_->value();
	return 0.0;
}

double CLSOMS58Motor::encoderCalibrationAbsoluteOffset() const{
	if(isConnected())
		return encoderCalibrationAbsoluteOffset_->value();
	return 0.0;
}

CLSOMS58Motor::MotorType CLSOMS58Motor::motorType() const{
	if(!isConnected())
		return CLSOMS58Motor::MotorTypeError;
	int enumChoice = (int)motorType_->value();
	switch(enumChoice){
	case 0:
		return CLSOMS58Motor::MotorStepperOnly;
		break;
	case 1:
		return CLSOMS58Motor::MotorStepperAndEncoder;
		break;
	case 2:
		return CLSOMS58Motor::MotorServo;
		break;
	default:
		return CLSOMS58Motor::MotorTypeError;
		break;

	}
}

CLSOMS58Motor::EncoderType CLSOMS58Motor::encoderType() const{
	if(!isConnected())
		return CLSOMS58Motor::EncoderTypeError;
	if(!hasEncoder_)
		return CLSOMS58Motor::EncoderNone;
	int enumChoice = (int)encoderType_->value();
	switch(enumChoice){
	case 0:
		return CLSOMS58Motor::EncoderIncrememtal;
		break;
	case 1:
		return CLSOMS58Motor::EncoderAbsolute;
		break;
	case 2:
		return CLSOMS58Motor::EncoderPotentiometer;
		break;
	default:
		return CLSOMS58Motor::EncoderTypeError;
		break;
	}
}

CLSOMS58Motor::EncoderEncoding CLSOMS58Motor::encoderEncoding() const{
	if(!isConnected())
		return CLSOMS58Motor::EncodingError;
	if(!hasEncoder_)
		return CLSOMS58Motor::EncodingNone;
	int enumChoice = (int)encoderEncoding_->value();
	switch(enumChoice){
	case 0:
		return CLSOMS58Motor::EncodingBinary;
		break;
	case 1:
		return CLSOMS58Motor::EncodingGreyCode;
		break;
	default:
		return CLSOMS58Motor::EncodingError;
		break;
	}
}

CLSOMS58Motor::LimitActiveState CLSOMS58Motor::limitActiveState() const{
	if(!isConnected())
		return CLSOMS58Motor::LimitActiveError;
	int enumChoice = (int)limitActiveState_->value();
	switch(enumChoice){
	case 0:
		return CLSOMS58Motor::LimitActiveLow;
		break;
	case 1:
		return CLSOMS58Motor::LimitActiveHigh;
		break;
	default:
		return CLSOMS58Motor::LimitActiveError;
		break;
	}
}

bool CLSOMS58Motor::limitDisabled() const{
	if(!isConnected())
		return false;
	if(limitDisabled_->withinTolerance(0))
		return false;
	else
		return true;
}

bool CLSOMS58Motor::closedLoopEnabled() const{
	if(!isConnected())
		return false;
	if(closedLoopEnabled_->withinTolerance(0))
		return false;
	else
		return true;
}

bool CLSOMS58Motor::servoPIDEnabled() const{
	if(!isConnected())
		return false;
	if(servoPIDEnabled_->withinTolerance(0))
		return false;
	else
		return true;
}

double CLSOMS58Motor::encoderTarget() const{
	if(isConnected())
		return encoderTarget_->value();
	return 0.5;
}

CLSOMS58Motor::EncoderMovementType CLSOMS58Motor::encoderMovementType() const{
	if(!isConnected())
		return CLSOMS58Motor::EncoderMovementTypeError;
	int enumChoice = (int)encoderMovementType_->value();
	switch(enumChoice){
	case 0:
		return CLSOMS58Motor::EncoderMovementOff;
		break;
	case 1:
		return CLSOMS58Motor::EncoderMovementIncreaseOnly;
		break;
	case 2:
		return CLSOMS58Motor::EncoderMovementDecreaseOnly;
		break;
	case 3:
		return CLSOMS58Motor::EncoderMovementIncrease;
		break;
	case 4:
		return CLSOMS58Motor::EncoderMovementDecrease;
		break;
	default:
		return CLSOMS58Motor::EncoderMovementTypeError;
		break;
	}
}

double CLSOMS58Motor::preDeadBand() const{
	if(isConnected())
		return preDeadBand_->value();
	return 0.5;
}

double CLSOMS58Motor::postDeadBand() const{
	if(isConnected())
		return postDeadBand_->value();
	return 0.5;
}

double CLSOMS58Motor::maxRetries() const{
	if(isConnected())
		return maxRetries_->value();
	return 0.5;
}

double CLSOMS58Motor::actualRetries() const{
	if(isConnected())
		return actualRetries_->value();
	return 0.5;
}

double CLSOMS58Motor::encoderPercentApproach() const{
	if(isConnected())
		return encoderPercentApproach_->value();
	return -1.0;
}

double CLSOMS58Motor::encoderStepSoftRatio() const{
	if(isConnected())
		return encoderStepSoftRatio_->value();
	return 0;
}

AMBeamlineActionItem* CLSOMS58Motor::createMotorMoveAction(double position){
	return createEGUMoveAction(position);
}

AMBeamlineActionItem* CLSOMS58Motor::createEGUMoveAction(double EGUPosition){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(this);
	action->setSetpoint(EGUPosition);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEGUVelocityAction(double EGUVelocity){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(EGUVelocity_);
	action->setSetpoint(EGUVelocity);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEGUBaseVelocityAction(double EGUBaseVelocity){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(EGUBaseVelocity_);
	action->setSetpoint(EGUBaseVelocity);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEGUAccelerationAction(double EGUAcceleration){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(EGUAcceleration_);
	action->setSetpoint(EGUAcceleration);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEGUOffsetAction(double EGUOffset){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(EGUOffset_);
	action->setSetpoint(EGUOffset);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createStepMoveAction(double stepPosition){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(step_);
	action->setSetpoint(stepPosition);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createStepVelocityAction(double stepVelocity){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(stepVelocity_);
	action->setSetpoint(stepVelocity);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createStepBaseVelocityAction(double stepBaseVelocity){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(stepBaseVelocity_);
	action->setSetpoint(stepBaseVelocity);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createStepAccelerationAction(double stepAcceleration){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(stepAcceleration_);
	action->setSetpoint(stepAcceleration);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createStopAction(){
	if(!isConnected())
		return 0;

	AMBeamlineControlStopAction *action = new AMBeamlineControlStopAction(this);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderCalibrationSlopeAction(double encoderCalibrationSlope){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderCalibrationSlope_);
	action->setSetpoint(encoderCalibrationSlope);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createStepCalibrationSlopeAction(double stepCalibrationSlope){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(stepCalibrationSlope_);
	action->setSetpoint(stepCalibrationSlope);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderCalibrationOffsetAction(double encoderCalibrationOffset){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderCalibrationOffset_);
	action->setSetpoint(encoderCalibrationOffset);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createStepCalibrationOffsetAction(double stepCalibrationOffset){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(stepCalibrationOffset_);
	action->setSetpoint(stepCalibrationOffset);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderCalibrationAbsoluteOffsetAction(double encoderCalibrationAbsoluteOffset){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderCalibrationAbsoluteOffset_);
	action->setSetpoint(encoderCalibrationAbsoluteOffset);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createMotorTypeAction(CLSOMS58Motor::MotorType motorType){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(motorType_);
	action->setSetpoint(motorType);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderTypeAction(CLSOMS58Motor::EncoderType encoderType){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderType_);
	action->setSetpoint(encoderType);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderEncodingAction(CLSOMS58Motor::EncoderEncoding encoderEncoding){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderEncoding_);
	action->setSetpoint(encoderEncoding);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createLimitActiveStateAction(CLSOMS58Motor::LimitActiveState limitActiveState){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(limitActiveState_);
	action->setSetpoint(limitActiveState);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createLimitDisabledAction(bool limitDisabled){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(limitDisabled_);
	if(limitDisabled)
		action->setSetpoint(1);
	else
		action->setSetpoint(0);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createClosedLoopEnabledAction(bool closedLoopEnabled){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(closedLoopEnabled_);
	if(closedLoopEnabled)
		action->setSetpoint(1);
	else
		action->setSetpoint(0);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createServoPIDEnabledAction(bool servoPIDEnabled){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(servoPIDEnabled_);
	if(servoPIDEnabled)
		action->setSetpoint(1);
	else
		action->setSetpoint(0);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderMoveAction(double encoderPosition){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderTarget_);
	action->setSetpoint(encoderPosition);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderMovementTypeAction(CLSOMS58Motor::EncoderMovementType encoderMovementType){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderMovementType_);
	action->setSetpoint(encoderMovementType);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createPreDeadBandAction(double preDeadBand){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(preDeadBand_);
	action->setSetpoint(preDeadBand);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createPostDeadBandAction(double postDeadBand){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(postDeadBand_);
	action->setSetpoint(postDeadBand);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createMaxRetriesAction(double maxRetries){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(maxRetries_);
	action->setSetpoint(maxRetries);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderPercentApproachAction(double encoderPercentApproach){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderPercentApproach_);
	action->setSetpoint(encoderPercentApproach);
	return action;
}

AMBeamlineActionItem* CLSOMS58Motor::createEncoderStepSoftRatioAction(double encoderStepSoftRatio){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderStepSoftRatio_);
	action->setSetpoint(encoderStepSoftRatio);
	return action;
}

void CLSOMS58Motor::setEGUVelocity(double velocity){
	if(isConnected())
		EGUVelocity_->move(velocity);
}

void CLSOMS58Motor::setEGUBaseVelocity(double baseVelocity){
	if(isConnected())
		EGUBaseVelocity_->move(baseVelocity);
}

void CLSOMS58Motor::setEGUAcceleration(double acceleration){
	if(isConnected())
		EGUAcceleration_->move(acceleration);
}

void CLSOMS58Motor::setEGUOffset(double EGUOffset){
	if(isConnected())
		EGUOffset_->move(EGUOffset);
}

void CLSOMS58Motor::setStep(double step)
{
	if (isConnected())
		step_->move(step);
}

void CLSOMS58Motor::setStepVelocity(double velocity){
	if(isConnected())
		stepVelocity_->move(velocity);
}

void CLSOMS58Motor::setStepBaseVelocity(double baseVelocity){
	if(isConnected())
		stepBaseVelocity_->move(baseVelocity);
}

void CLSOMS58Motor::setStepAcceleration(double acceleration){
	if(isConnected())
		stepAcceleration_->move(acceleration);
}

void CLSOMS58Motor::setUsingKill(bool usingKill){
	usingKill_ = usingKill;
}

bool CLSOMS58Motor::stop(){
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

void CLSOMS58Motor::setEncoderCalibrationSlope(double encoderCalibrationSlope){
	if(isConnected())
		encoderCalibrationSlope_->move(encoderCalibrationSlope);
}

void CLSOMS58Motor::setStepCalibrationSlope(double stepCalibrationSlope){
	if(isConnected())
		stepCalibrationSlope_->move(stepCalibrationSlope);
}

void CLSOMS58Motor::setEncoderCalibrationOffset(double encoderCalibrationOffset){
	if(isConnected())
		encoderCalibrationOffset_->move(encoderCalibrationOffset);
}

void CLSOMS58Motor::setStepCalibrationOffset(double stepCalibrationOffset){
	if(isConnected())
		stepCalibrationOffset_->move(stepCalibrationOffset);
}

void CLSOMS58Motor::setEncoderCalibrationAbsoluteOffset(double encoderCalibrationAbsoluteOffset){
	if(isConnected())
		encoderCalibrationAbsoluteOffset_->move(encoderCalibrationAbsoluteOffset);
}

void CLSOMS58Motor::setMotorType(CLSOMS58Motor::MotorType motorType){
	if(isConnected())
		motorType_->move(motorType);
}

void CLSOMS58Motor::setEncoderType(CLSOMS58Motor::EncoderType encoderType){
	if(isConnected())
		encoderType_->move(encoderType);
}

void CLSOMS58Motor::setEncoderEncoding(CLSOMS58Motor::EncoderEncoding encoderEncoding){
	if(isConnected())
		encoderEncoding_->move(encoderEncoding);
}

void CLSOMS58Motor::setLimitActiveState(CLSOMS58Motor::LimitActiveState limitActiveState){
	if(isConnected())
		limitActiveState_->move(limitActiveState);
}

void CLSOMS58Motor::setLimitDisabled(bool disabled){
	if(isConnected()){
		if(disabled)
			limitDisabled_->move(1);
		else
			limitDisabled_->move(0);
	}
}

void CLSOMS58Motor::setClosedLoopEnabled(bool closedLoopEnabled){
	if(isConnected()){
		if(closedLoopEnabled)
			closedLoopEnabled_->move(1);
		else
			closedLoopEnabled_->move(0);
	}
}

void CLSOMS58Motor::setServoPIDEnabled(bool servoPIDEnabled){
	if(isConnected()){
		if(servoPIDEnabled)
			servoPIDEnabled_->move(1);
		else
			servoPIDEnabled_->move(0);
	}
}

void CLSOMS58Motor::setEncoderTarget(double encoderTarget){
	if(isConnected())
		encoderTarget_->move(encoderTarget);
}

void CLSOMS58Motor::setEncoderMovementType(CLSOMS58Motor::EncoderMovementType encoderMovementType){
	if(isConnected())
		encoderMovementType_->move(encoderMovementType);
}

void CLSOMS58Motor::setPreDeadBand(double preDeadBand){
	if(isConnected())
		preDeadBand_->move(preDeadBand);
}

void CLSOMS58Motor::setPostDeadBand(double postDeadBand){
	if(isConnected())
		postDeadBand_->move(postDeadBand);
}

void CLSOMS58Motor::setMaxRetries(double maxRetries){
	if(isConnected())
		maxRetries_->move(maxRetries);
}

void CLSOMS58Motor::setEncoderPercentApproach(double encoderPercentApproach){
	if(isConnected())
		encoderPercentApproach_->move(encoderPercentApproach);
}

void CLSOMS58Motor::setEncoderStepSoftRatio(double encoderStepSoftRatio){
	if(isConnected())
		encoderStepSoftRatio_->move(encoderStepSoftRatio);
}

void CLSOMS58Motor::onLimitsChanged(double value){
	Q_UNUSED(value)
	// I would love to check withinTolerance(1) ... but I only really know 0 means not at limit
	if(cwLimit_->withinTolerance(0) && ccwLimit_->withinTolerance(0))
		emit atLimitChanged(CLSOMS58Motor::LimitNone);
	else if(cwLimit_->withinTolerance(0))
		emit atLimitChanged(CLSOMS58Motor::LimitCCW);
	else if(ccwLimit_->withinTolerance(0))
		emit atLimitChanged(CLSOMS58Motor::LimitCW);
	else
		emit atLimitChanged(CLSOMS58Motor::LimitError);
}

void CLSOMS58Motor::onPowerStateChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice)
	{
	case 0:
		emit powerStatedChanged(CLSOMS58Motor::PowerOff);
		break;
	case 1:
		emit powerStatedChanged(CLSOMS58Motor::PowerOn);
		break;
	case 2:
		emit powerStatedChanged(CLSOMS58Motor::PowerAutoHardware);
		break;
	case 3:
		emit powerStatedChanged(CLSOMS58Motor::PowerAutoSoftware);
		break;
	default:
		emit powerStatedChanged(CLSOMS58Motor::PowerError);
	}
}

void CLSOMS58Motor::onPowerInvertedChanged(double value){
	Q_UNUSED(value)
	if(powerInverted_->withinTolerance(0))
		emit powerInvertedChanged(false);
	else
		emit powerInvertedChanged(true);
}

void CLSOMS58Motor::onMotorTypeChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit motorTypeChanged(CLSOMS58Motor::MotorStepperOnly);
		break;
	case 1:
		emit motorTypeChanged(CLSOMS58Motor::MotorStepperAndEncoder);
		break;
	case 2:
		emit motorTypeChanged(CLSOMS58Motor::MotorServo);
		break;
	default:
		emit motorTypeChanged(CLSOMS58Motor::MotorTypeError);
		break;
	}
}

void CLSOMS58Motor::onEncoderTypeChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit encoderTypeChanged(CLSOMS58Motor::EncoderIncrememtal);
		break;
	case 1:
		emit encoderTypeChanged(CLSOMS58Motor::EncoderAbsolute);
		break;
	case 2:
		emit encoderTypeChanged(CLSOMS58Motor::EncoderPotentiometer);
		break;
	default:
		emit encoderTypeChanged(CLSOMS58Motor::EncoderTypeError);
		break;
	}
}

void CLSOMS58Motor::onEncoderEncodingChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit encoderEncodingChanged(CLSOMS58Motor::EncodingBinary);
		break;
	case 1:
		emit encoderEncodingChanged(CLSOMS58Motor::EncodingGreyCode);
		break;
	default:
		emit encoderEncodingChanged(CLSOMS58Motor::EncodingError);
		break;
	}
}

void CLSOMS58Motor::onLimitActiveStateChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit limitActiveStateChanged(CLSOMS58Motor::LimitActiveLow);
		break;
	case 1:
		emit limitActiveStateChanged(CLSOMS58Motor::LimitActiveHigh);
		break;
	default:
		emit limitActiveStateChanged(CLSOMS58Motor::LimitActiveError);
		break;
	}
}

void CLSOMS58Motor::onLimitDisabledChanged(double value){
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

void CLSOMS58Motor::onClosedLoopEnabledChanged(double value){
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

void CLSOMS58Motor::onServoPIDEnabledChanged(double value){
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

void CLSOMS58Motor::onEncoderMovementTypeChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit encoderMovementTypeChanged(CLSOMS58Motor::EncoderMovementOff);
		break;
	case 1:
		emit encoderMovementTypeChanged(CLSOMS58Motor::EncoderMovementIncreaseOnly);
		break;
	case 2:
		emit encoderMovementTypeChanged(CLSOMS58Motor::EncoderMovementDecreaseOnly);
		break;
	case 3:
		emit encoderMovementTypeChanged(CLSOMS58Motor::EncoderMovementIncrease);
		break;
	case 4:
		emit encoderMovementTypeChanged(CLSOMS58Motor::EncoderMovementDecrease);
		break;
	default:
		emit encoderMovementTypeChanged(CLSOMS58Motor::EncoderMovementTypeError);
		break;
	}

}

*/
 AMControlStatusCheckerCLSOMS58::~AMControlStatusCheckerCLSOMS58(){}
