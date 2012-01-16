#include "AMControlMoveAction.h"

#include "beamline/AMControl.h"
#include "util/AMErrorMonitor.h"

AMControlMoveAction::AMControlMoveAction(AMControlMoveActionInfo *info, QObject *parent)
	: AMAction(info, parent) {
	control_ = 0;
}

void AMControlMoveAction::startImplementation() {
	const AMControlInfo& setpoint = *(controlMoveInfo()->controlInfo());

	// lookup the control we need to use to run this
	control_ = getControlForAction(setpoint);

	// Must have a control, and it must be able to move.
	if(!control_) {
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 -1,
										 QString("There was an error moving the control '%1' into position, because the control was not found. Please report this problem to the Acquaman developers.").arg(setpoint.name())));
		notifyFailed();
		return;
	}
	if(!control_->canMove()) {
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 -2,
										 QString("There was an error moving the control '%1' into position, because the control was not connected and ready to move. Please report this problem to the beamline staff.")
										 .arg(control_->name())));
		notifyFailed();
		return;
	}

	// connect to its moveSucceeded and moveFailed signals
	connect(control_, SIGNAL(moveStarted()), this, SLOT(onMoveStarted()));
	connect(control_, SIGNAL(moveFailed(int)), this, SLOT(onMoveFailed(int)));
	connect(control_, SIGNAL(moveSucceeded()), this, SLOT(onMoveSucceeded()));

	// remember the start position:
	startPosition_ = control_->toInfo();

	// start the move
	control_->move(setpoint.value());
}

void AMControlMoveAction::onMoveStarted()
{
	notifyStarted();
	// start the progressTick timer
	connect(&progressTick_, SIGNAL(timeout()), this, SLOT(onProgressTick()));
	progressTick_.start(250);
}

void AMControlMoveAction::onMoveFailed(int reason)
{
	progressTick_.stop();
	disconnect(&progressTick_, 0, this, 0);

	// error message with reason
	AMErrorMon::report(AMErrorReport(this,
									 AMErrorReport::Alert,
									 reason,
									 QString("There was an error moving the control '%1' into position. Target: %2 %3. Actual position: %4 %5. Reason: %6. Please report this problem to the beamline staff.")
									 .arg(control_->name())
									 .arg(control_->setpoint())
									 .arg(control_->units())
									 .arg(control_->value())
									 .arg(control_->units())
									 .arg(reason)));
	notifyFailed();
}

void AMControlMoveAction::onMoveSucceeded()
{
	progressTick_.stop();
	disconnect(&progressTick_, 0, this, 0);

	setProgress(100,100);
	notifySucceeded();
}

void AMControlMoveAction::onProgressTick()
{
	double destination = controlMoveInfo()->controlInfo()->value();
	double fractionComplete = (control_->value() - startPosition_.value()) / (destination - startPosition_.value());
	double runningSeconds = runningTime();
	double expectedTotalSeconds = runningSeconds / fractionComplete;

	if(fractionComplete != 0) {
		// set expected duration based on time taken so far and distance travelled to destination.
		setExpectedDuration(expectedTotalSeconds);
		setProgress(runningSeconds, expectedTotalSeconds);	// exactly equal to fractionComplete. We observe the convention that when possible, progress numerator and denominator should be in seconds.
	}
	else {
		setProgress(0, 100);
		// in this case, no idea on the expected duration (unless we knew the speed of the control)
	}
}

void AMControlMoveAction::cancelImplementation()
{
	// either don't have a control, or called prior to startImplementation(). Nothing to do except report cancelled.
	if(!control_) {
		notifyCancelled();
		return;
	}


	/// \todo Question: what about controls that cannot stop? Do we need to wait for them to finish their full move before notifying that we've cancelled?   [And in that case... What about mis-behaving controls that never report being done? Should they be able to block the whole action system by never cancelling?  Do we need another action instruction for "abort()", which moves on from actions that have ignored their instruction to cancel()?]
	if(!control_->stop()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -5, QString("Could not stop the control '%1' because it doesn't support stopping in mid-motion. This action has been cancelled, but be careful because the machine could still be moving.").arg(control_->name())));
	}

	// for now, cancel the action as long as we've sent the 'stop' request. This is an open question for debate.
	notifyCancelled();
}

