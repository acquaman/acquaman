/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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
	// check we can move...
	if(!control_->canMove()) {
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 -2,
										 QString("There was an error moving the control '%1' into position, because the control was not connected and ready to move. Please report this problem to the beamline staff.")
										 .arg(control_->name())));
		notifyFailed();
		return;
	}
	// check that the destination is in range...
	if(control_->valueOutOfRange(setpoint.value())) {
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 -3,
										 QString("There was an error moving the control '%1' into position, because the destination %2 %3 was outside its range. Please report this problem to the beamline staff.")
										 .arg(control_->name())
										 .arg(setpoint.value())
										 .arg(setpoint.units())));
		notifyFailed();
		return;
	}

	// connect to its moveSucceeded and moveFailed signals
	connect(control_, SIGNAL(moveStarted()), this, SLOT(onMoveStarted()));
	connect(control_, SIGNAL(moveReTargetted()), this, SLOT(onMoveStarted()));	// For controls that support allowsMovesWhileMoving(), they might already be moving when we request our move(). A moveReTargetted() signal from them also counts as a moveStarted() for us.
	connect(control_, SIGNAL(moveFailed(int)), this, SLOT(onMoveFailed(int)));
	connect(control_, SIGNAL(moveSucceeded()), this, SLOT(onMoveSucceeded()));

	// remember the start position:
	startPosition_ = control_->toInfo();

	// start the move:
	if(controlMoveInfo()->isRelativeMove()) {
		if(!control_->moveRelative(setpoint.value()))
			onMoveFailed(AMControl::OtherFailure);
	}
	else {
		if(!control_->move(setpoint.value()))
			onMoveFailed(AMControl::OtherFailure);
	}
}

void AMControlMoveAction::onMoveStarted()
{
	disconnect(control_, SIGNAL(moveStarted()), this, SLOT(onMoveStarted()));
	disconnect(control_, SIGNAL(moveReTargetted()), this, SLOT(onMoveStarted()));
	// From now on, if we get a moveReTargetted() signal, this would be bad: someone is re-directing our move to a different setpoint, so this now counts as a failure.
	connect(control_, SIGNAL(moveReTargetted()), this, SLOT(onMoveReTargetted()));

	notifyStarted();
	// start the progressTick timer
	connect(&progressTick_, SIGNAL(timeout()), this, SLOT(onProgressTick()));
	progressTick_.start(250);
}

void AMControlMoveAction::onMoveReTargetted()
{
	// someone is re-directing our move to a different setpoint, so this now counts as a failure.
	onMoveFailed(6);	// AMControl::FailureExplanation goes up to 5; Using one higher to indicate that our desired move was re-directed somewhere else.
}

void AMControlMoveAction::onMoveFailed(int reason)
{
	disconnect(control_, 0, this, 0);
	progressTick_.stop();
	disconnect(&progressTick_, 0, this, 0);

	QString failureExplanation;
	switch(reason) {
	case 1: failureExplanation = "The control was not connected."; break;
	case 2: failureExplanation = "The required tolerance was not met."; break;
	case 3: failureExplanation = "The move timed out without starting or reaching its destination."; break;
	case 4: failureExplanation = "The move was manually interrupted or stopped."; break;
	case 6: failureExplanation = "The move was externally re-directed to another destination."; break;
	case 5:
	default:
		failureExplanation = "An undocumented failure happened."; break;
	}

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
									 .arg(failureExplanation)));
	notifyFailed();
}

void AMControlMoveAction::onMoveSucceeded()
{
	disconnect(control_, 0, this, 0);
	progressTick_.stop();
	disconnect(&progressTick_, 0, this, 0);

	setProgress(100,100);
	notifySucceeded();
}

void AMControlMoveAction::onProgressTick()
{
	double destination = control_->setpoint();
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

