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


#include "AMControlMoveAction3.h"

#include "beamline/AMControl.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/AMAgnosticDataAPI.h"

AMControlMoveAction3::AMControlMoveAction3(AMControlMoveActionInfo3 *info, AMControl *control, QObject *parent)
	: AMAction3(info, parent)
{
	if (control)
		control_ = control;
	else
		control_ = AMBeamline::bl()->exposedControlByInfo(*(info->controlInfo()));
}

AMControlMoveAction3::AMControlMoveAction3(const AMControlMoveAction3 &other)
	: AMAction3(other)
{
	const AMControlMoveActionInfo3 *info = qobject_cast<const AMControlMoveActionInfo3*>(other.info());

	if (info)
		control_ = AMBeamline::bl()->exposedControlByInfo(*(info->controlInfo()));
	else
		control_ = 0;
}

void AMControlMoveAction3::startImplementation()
{
	const AMControlInfo& setpoint = *(controlMoveInfo()->controlInfo());

	// If you still don't have a control, check the exposed controls one last time.
	if (!control_)
		control_ = AMBeamline::bl()->exposedControlByInfo(*(controlMoveInfo()->controlInfo()));

	// Must have a control, and it must be able to move.
	if(!control_) {
		AMErrorMon::alert(this,
						  -13001,
						  QString("There was an error moving the control '%1' into position, because the control was not found. Please report this problem to the Acquaman developers.").arg(setpoint.name()));
		setFailed();
		return;
	}
	// check we can move...
	if(!control_->canMove()) {
		AMErrorMon::alert(this,
						  -13002,
						  QString("There was an error moving the control '%1' into position, because the control was not connected and ready to move. Please report this problem to the beamline staff.")
						  .arg(control_->name()));
		setFailed();
		return;
	}
	// check that the destination is in range...
	if(control_->valueOutOfRange(controlMoveInfo()->isRelativeMove() ? control_->value()+setpoint.value() : setpoint.value())) {
		AMErrorMon::alert(this,
						  -13003,
						  QString("There was an error moving the control '%1' into position, because the destination %2 %3 was outside its range. Please report this problem to the beamline staff.")
						  .arg(control_->name())
						  .arg(setpoint.value())
						  .arg(setpoint.units()));
		setFailed();
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
	int failureExplanation;
	if(controlMoveInfo()->isRelativeMove() && controlMoveInfo()->isRelativeFromSetpoint())
		failureExplanation = control_->moveRelative(setpoint.value(), AMControl::RelativeMoveFromSetpoint);
	else if(controlMoveInfo()->isRelativeMove())
		failureExplanation = control_->moveRelative(setpoint.value(), AMControl::RelativeMoveFromValue);
	else
		failureExplanation = control_->move(setpoint.value());

	if(failureExplanation != AMControl::NoFailure)
		onMoveFailed(failureExplanation);
}

void AMControlMoveAction3::onMoveStarted()
{
	disconnect(control_, SIGNAL(moveStarted()), this, SLOT(onMoveStarted()));
	disconnect(control_, SIGNAL(moveReTargetted()), this, SLOT(onMoveStarted()));
	// From now on, if we get a moveReTargetted() signal, this would be bad: someone is re-directing our move to a different setpoint, so this now counts as a failure.
	connect(control_, SIGNAL(moveReTargetted()), this, SLOT(onMoveReTargetted()));

	setStarted();
	// start the progressTick timer
	connect(&progressTick_, SIGNAL(timeout()), this, SLOT(onProgressTick()));
	progressTick_.start(250);
}

void AMControlMoveAction3::onMoveReTargetted()
{
	// someone is re-directing our move to a different setpoint, so this now counts as a failure.
	onMoveFailed(AMControl::RedirectedFailure);
}

void AMControlMoveAction3::onMoveFailed(int reason)
{
	disconnect(control_, 0, this, 0);
	progressTick_.stop();
	disconnect(&progressTick_, 0, this, 0);

	// error message with reason
	AMErrorMon::alert(this,
					  13000+reason,
					  QString("There was an error moving the control '%1' into position. Target: %2 %3. Actual position: %4 %5. Reason: %6. Please report this problem to the beamline staff.")
					  .arg(control_->name())
					  .arg(control_->setpoint())
					  .arg(control_->units())
					  .arg(control_->value())
					  .arg(control_->units())
					  .arg(AMControl::failureExplanation(reason)));
	setFailed();
}

void AMControlMoveAction3::onMoveSucceeded()
{
	disconnect(control_, 0, this, 0);
	progressTick_.stop();
	disconnect(&progressTick_, 0, this, 0);

	setProgress(100,100);

	if(generateScanActionMessages_){
		AMAgnosticDataAPIControlMovedMessage controlMovedMessage(control_->name(), "INVALIDMOVEMENTTYPE", controlMoveInfo()->controlInfo()->value());
		if(controlMoveInfo()->isRelativeMove())
			controlMovedMessage.setControlMovementType("Relative");
		else
			controlMovedMessage.setControlMovementType("Absolute");
		AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions")->postMessage(controlMovedMessage);
	}

	setSucceeded();
}

void AMControlMoveAction3::onProgressTick()
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

void AMControlMoveAction3::cancelImplementation()
{
	// either don't have a control, or called prior to startImplementation(). Nothing to do except report cancelled.
	if(!control_) {

		setCancelled();
		return;
	}

	/// \todo Question: what about controls that cannot stop? Do we need to wait for them to finish their full move before notifying that we've cancelled?   [And in that case... What about mis-behaving controls that never report being done? Should they be able to block the whole action system by never cancelling?  Do we need another action instruction for "abort()", which moves on from actions that have ignored their instruction to cancel()?]
	if(!control_->stop()) {
		AMErrorMon::alert(this, -13005, QString("Could not stop the control '%1' because it doesn't support stopping in mid-motion. This action has been cancelled, but be careful because the machine could still be moving.").arg(control_->name()));
	}

	// for now, cancel the action as long as we've sent the 'stop' request. This is an open question for debate.
	setCancelled();
}



