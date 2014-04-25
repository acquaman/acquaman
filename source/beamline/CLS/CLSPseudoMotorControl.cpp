#include "CLSPseudoMotorControl.h"

CLSPseudoMotorControl::CLSPseudoMotorControl(const QString &name, const QString &readPVname, const QString &writePVname, const QString &movingPVname, const QString &stopPVname, QObject *parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker *statusChecker, int stopValue, const QString &description)
	: AMPVwStatusControl(name, readPVname, writePVname, movingPVname, stopPVname, parent, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description)
{
}

AMControl::FailureExplanation CLSPseudoMotorControl::move(double setpoint)
{
	if(isMoving()) {

		delayMove_ = true;
		delayMoveSetpoint_ = setpoint;
	}

	else {

		if (delayMove_)
			delayMove_ = false;

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
	}

	return NoFailure;
}

bool CLSPseudoMotorControl::stop()
{
	if (delayMove_)
		delayMove_ = false;

	return AMPVwStatusControl::stop();
}

void CLSPseudoMotorControl::onMovingChanged(int isMovingValue)
{
	AMPVwStatusControl::onMovingChanged(isMovingValue);

	if (delayMove_)
		move(delayMoveSetpoint_);
}
