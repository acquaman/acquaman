#include "BioXASCLSMAXvMotor.h"

BioXASCLSMAXvMotor::BioXASCLSMAXvMotor(BioXASBeamlineDef::BioXASMotorType motorType, const QString& name, const QString& baseName, const QString& description, QString pvUnitFieldName, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds, QObject *parent)
	: CLSMAXvMotor(name, baseName, description, hasEncoder, tolerance, moveStartTimeoutSeconds, parent, pvUnitFieldName)
{
	type_ = motorType;
	pvBaseName_ = baseName;
	pvUnitFieldName_ = pvUnitFieldName;

	statusPVControl_ = new AMReadOnlyPVControl("Motor moving status", statusPVName(), this);
}

BioXASBeamlineDef::BioXASMotorType BioXASCLSMAXvMotor::type(){
	return type_;
}

QString BioXASCLSMAXvMotor::valuePVName(){
	return pvBaseName_ + pvUnitFieldName_;
}

QString BioXASCLSMAXvMotor::feedbackPVName(){
	return pvBaseName_ + pvUnitFieldName_ + ":fbk";
}

QString BioXASCLSMAXvMotor::statusPVName(){
	return pvBaseName_ + ":status";
}

QString BioXASCLSMAXvMotor::CWPVName(){
	return pvBaseName_ + ":cw";
}

QString BioXASCLSMAXvMotor::CCWPVName(){
	return pvBaseName_ + ":ccw";
}

AMReadOnlyPVControl *BioXASCLSMAXvMotor::statusPVControl(){
	return statusPVControl_;
}

AMControl::FailureExplanation BioXASCLSMAXvMotor::move(double setpoint){
	CLSMAXvMotor::Limit limitCondition = atLimit();

	if(limitCondition == CLSMAXvMotor::LimitNone)
		return CLSMAXvMotor::move(setpoint);
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
