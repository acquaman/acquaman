#include "BioXASBraggMaxVMotorControl.h"

BioXASBraggMaxVMotorControl::BioXASBraggMaxVMotorControl(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds, QObject *parent, QString pvUnitFieldName) :
	CLSMAXvMotor(name, baseName, description, hasEncoder, tolerance, moveStartTimeoutSeconds, parent, pvUnitFieldName)
{
	moveValueUpdateReceived_ = false;
}

BioXASBraggMaxVMotorControl::~BioXASBraggMaxVMotorControl()
{

}

AMControl::ErrorExplanation BioXASBraggMaxVMotorControl::move(double setpoint)
{
	moveValueUpdateReceived_ = false;

	return CLSMAXvMotor::move(setpoint);
}

void BioXASBraggMaxVMotorControl::onNewFeedbackValue(double value)
{
	if (moveInProgress_) {
		moveValueUpdateReceived_ = true;
	}

	CLSMAXvMotor::onNewFeedbackValue(value);
}

void BioXASBraggMaxVMotorControl::onMovingChanged(int isMovingValue)
{
	bool nowMoving = (*statusChecker_)(isMovingValue);	// according to the hardware.  For checking moveSucceeded/moveStarted/moveFailed, use the value delivered in the signal argument, instead of re-checking the PV, in case we respond late and the hardware has already changed again.

	// In case the hardware is being silly and sending multiple MOVE ACTIVE, MOVE ACTIVE, MOVE ACTIVE states in a row, or MOVE DONE, MOVE DONE, MOVE DONE states in a row: only act on changes. [Edge detection]
	if(nowMoving == hardwareWasMoving_)
		return;

	hardwareWasMoving_ = nowMoving;

	// moveStarted, moveFailed, moveSucceeded, or transition to settling:
	///////////////////////////////////////////////////////////////////////

	// if we requested one of our moves, and moving just started:
	if(startInProgress_ && nowMoving) {
		moveInProgress_ = true;
		startInProgress_ = false;
		// This is great... the device started moving within the timeout:

		// disable the moveStartTimer, we don't need it anymore
		moveStartTimer_.stop();

		emit moveStarted();
	}

	// If one of our moves was running, and we stopped moving:
	if(moveInProgress_ && !nowMoving && moveValueUpdateReceived_) {

		// Mode 1: No settling:
		if( settlingTime_ == 0.0) {
			// That's the end of our move
			moveInProgress_ = false;

			// Check if we succeeded...
			if(inPosition()) {
				emit moveSucceeded();

			} else {
				emit moveFailed(AMControl::ToleranceFailure);

			}
		}
		// Mode 2: allow settling
		else {
			if(!settlingInProgress_) {
				settlingInProgress_ = true;
				settlingTimer_.start(int(settlingTime_*1000)); // QTimer uses millisecond time intervals.
			}
		}
	}

	// "sucessfully" stopped due to a stop() command.
	if(stopInProgress_ && !nowMoving) {
		stopInProgress_ = false;
		// but the move itself has failed, due to a stop() intervention.
		emit moveFailed(AMControl::WasStoppedFailure);
	}



	// Emitting movingChanged().
	/////////////////////////////////////

	// For external purposes, isMoving() depends on whether the hardware says we're moving, or we're in the settling phase.
	nowMoving = isMoving();

	if(nowMoving != wasMoving_)
		emit movingChanged(wasMoving_ = nowMoving);
}
