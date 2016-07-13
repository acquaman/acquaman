#include "BioXASMAXvMotorMoveToLimitAction.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

BioXASMAXvMotorMoveToLimitAction::BioXASMAXvMotorMoveToLimitAction(BioXASMAXvMotorMoveToLimitActionInfo *info, BioXASMAXvMotor *motor, QObject *parent) :
	AMAction3(info, parent)
{
	control_ = motor;
	initialValue_ = 0;

	progressTimer_.setSingleShot(false);
	progressTimer_.setInterval(BIOXASMAXVMOTORMOVETOLIMITACTION_PROGRESS_INTERVAL);

	moveFinishedTimer_.setSingleShot(true);
	moveFinishedTimer_.setInterval(BIOXASMAXVMOTORMOVETOLIMITACTION_MOVE_FINISHED_TIMEOUT);
}

BioXASMAXvMotorMoveToLimitAction::BioXASMAXvMotorMoveToLimitAction(const BioXASMAXvMotorMoveToLimitAction &original) :
	AMAction3(original)
{
	control_ = original.control();
	initialValue_ = 0;

	progressTimer_.setSingleShot(false);
	progressTimer_.setInterval(BIOXASMAXVMOTORMOVETOLIMITACTION_PROGRESS_INTERVAL);

	moveFinishedTimer_.setSingleShot(true);
	moveFinishedTimer_.setInterval(BIOXASMAXVMOTORMOVETOLIMITACTION_MOVE_FINISHED_TIMEOUT);
}

BioXASMAXvMotorMoveToLimitAction::~BioXASMAXvMotorMoveToLimitAction()
{

}

void BioXASMAXvMotorMoveToLimitAction::onMotorStarted()
{
	// Set the action as started.

	setStarted();

	// Start progress timer updates.

	connect( &progressTimer_, SIGNAL(timeout()), this, SLOT(onProgressTimerTimeout()) );
	progressTimer_.start();
}

void BioXASMAXvMotorMoveToLimitAction::onMotorFinished()
{
	// If the motor move is finished and the limit has been reached,
	// the move is finished! Else, start the move finished timer.

	if (moveFinished()) {

		onMoveFinished();

	} else if (motorFinished() && !moveFinishedTimer_.isActive()) {

		connect( &moveFinishedTimer_, SIGNAL(timeout()), this, SLOT(onMoveFinished()) );
		moveFinishedTimer_.start();
	}
}

void BioXASMAXvMotorMoveToLimitAction::onMotorLimitStatusChanged()
{
	// If the motor move is finished and the limit has been reached,
	// the move is finished! Else, start the move finished timer.

	if (moveFinished()) {

		   onMoveFinished();

	} else if (limitReached() && !moveFinishedTimer_.isActive()) {

		connect( &moveFinishedTimer_, SIGNAL(timeout()), this, SLOT(onMoveFinished()) );
		moveFinishedTimer_.start();
	}
}

void BioXASMAXvMotorMoveToLimitAction::onMoveFinished()
{
	// Disconnect from control.

	disconnect( control_, 0, this, 0 );

	// End move finished timer updates.

	moveFinishedTimer_.stop();
	disconnect( &moveFinishedTimer_, 0, this, 0 );

	// End progress timer updates.

	progressTimer_.stop();
	disconnect( &progressTimer_, 0, this, 0 );

	// Update whether the action succeeded or failed as appropriate.

	if (moveFinished()) {

		setProgress(100, 100);
		setSucceeded();

	} else {

		setFailed(QString("Failed to move motor '%1' to '%2' limit."));
	}
}

void BioXASMAXvMotorMoveToLimitAction::onProgressTimerTimeout()
{
	if (control_) {

		// Identify the action's current progress.

		double destination = control_->setpoint();
		double fractionComplete = (control_->value() - initialValue_) / (destination - initialValue_);
		double runningSeconds = runningTime();
		double expectedTotalSeconds = runningSeconds / fractionComplete;

		// If the action is in the middle of executing, update the progress and expected duration.
		// Else, ensure reported progress is 0.

		if (fractionComplete != 0) {

			setExpectedDuration(expectedTotalSeconds);
			setProgress(runningSeconds, expectedTotalSeconds);

		} else {

			setProgress(0, 100);
		}
	}
}

void BioXASMAXvMotorMoveToLimitAction::startImplementation()
{
	// Must have a control.

	if(!control_) {
		AMErrorMon::alert(this, BIOXASMAXVMOTORMOVETOLIMITACTION_INVALID_CONTROL, QString("Failed to move control '%1' to '%2' limit. The control was not found.").arg(moveToLimitInfo()->controlInfo().name()).arg(moveToLimitInfo()->limitSetpointToString(moveToLimitInfo()->limitSetpoint())));
		setFailed();
	}

	// Identify initial value for progress updates. Must be set early, before any motor move stuff.

	initialValue_ = control_->value();

	// Make connections.

	connect( control_, SIGNAL(moveStarted()), this, SLOT(onMotorStarted()) );
	connect( control_, SIGNAL(moveFailed(int)), this, SLOT(onMotorFinished()) );
	connect( control_, SIGNAL(moveSucceeded()), this, SLOT(onMotorFinished()) );
	connect( control_, SIGNAL(atLimitChanged(CLSMAXvMotor::Limit)), this, SLOT(onMotorLimitStatusChanged()) );

	// Identify whether the control is at the desired limit already.
	// If so, artifically "start" and end the move to get the action through the appropriate states.
	// If not, initate move to the new limit.

	if (limitReached()) {

		onMotorStarted();
		onMotorFinished();

	} else {

		AMControl::FailureExplanation failureExplanation = control_->moveToLimit(moveToLimitInfo()->limitSetpoint());

		if (failureExplanation != AMControl::NoFailure)
			onMoveFinished();
	}
}

