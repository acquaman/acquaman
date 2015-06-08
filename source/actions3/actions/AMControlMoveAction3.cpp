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
		control_ = AMBeamline::bl()->exposedControlByInfo(info->controlInfo());

	retries_ = 1;
	attempts_ = 0;
	destination_ = 0;
}

AMControlMoveAction3::AMControlMoveAction3(const AMControlMoveAction3 &other)
	: AMAction3(other)
{
	const AMControlMoveActionInfo3 *info = qobject_cast<const AMControlMoveActionInfo3*>(other.info());

	if (info)
		control_ = AMBeamline::bl()->exposedControlByInfo(info->controlInfo());
	else
		control_ = 0;

	retries_ = other.retries();
	attempts_ = 0;
	destination_ = 0;
}

AMControlMoveAction3::~AMControlMoveAction3(){}

double AMControlMoveAction3::destination() const
{
	return destination_;
}

int AMControlMoveAction3::retries() const
{
	return retries_;
}

int AMControlMoveAction3::attempts() const
{
	return attempts_;
}

void AMControlMoveAction3::setRetries(int retries)
{
	retries_ = retries;
}

void AMControlMoveAction3::startImplementation()
{
	const AMControlInfo& setpoint = controlMoveInfo()->controlInfo();

	// If you still don't have a control, check the exposed controls one last time.
	if (!control_)
		control_ = AMBeamline::bl()->exposedControlByInfo(controlMoveInfo()->controlInfo());

	// Must have a control, and it must be able to move.
	if(!control_) {
		QString fundamentalFailureMessage = QString("There was an error moving the control '%1' into position, because the control was not found.").arg(setpoint.name());
		AMErrorMon::alert(this, -13001, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	// check we can move...
	if(!control_->canMove()) {
		QString fundamentalFailureMessage = QString("There was an error moving the control '%1' into position, because the control reported it cannot move.").arg(control_->name());
		AMErrorMon::alert(this, -13002, QString("%1. Please report this problem to the beamline staff.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	// check that the destination is in range...
	if(control_->valueOutOfRange(controlMoveInfo()->isRelativeMove() ? control_->value()+setpoint.value() : setpoint.value())) {
		QString fundamentalFailureMessage = QString("There was an error moving the control '%1' into position, because the destination %2 %3 was outside its range of %4 to %5.").arg(control_->name()).arg(setpoint.value()).arg(setpoint.units()).arg(control_->minimumValue()).arg(control_->maximumValue());
		AMErrorMon::alert(this, -13003, QString("%1. Please report this problem to the beamline staff.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	// connect to its moveSucceeded and moveFailed signals
	connect(control_, SIGNAL(moveStarted()), this, SLOT(onMoveStarted()));
	connect(control_, SIGNAL(moveReTargetted()), this, SLOT(onMoveStarted()));	// For controls that support allowsMovesWhileMoving(), they might already be moving when we request our move(). A moveReTargetted() signal from them also counts as a moveStarted() for us.
	connect(control_, SIGNAL(moveFailed(int)), this, SLOT(onMoveFailed(int)));
	connect(control_, SIGNAL(moveSucceeded()), this, SLOT(onMoveSucceeded()));

	// remember the start position:
	startPosition_ = control_->toInfo();

	attempts_ = 1;

	// determine the absolute destination
	if(controlMoveInfo()->isRelativeMove() && controlMoveInfo()->isRelativeFromSetpoint())
		destination_ = control_->setpoint() + setpoint.value();
	else if(controlMoveInfo()->isRelativeMove())
		destination_ = control_->value() + setpoint.value();
	else
		destination_ = setpoint.value();

	// start the move:
	int failureExplanation = control_->move(destination_);
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
	bool retryAvailable = false;

	int definedFailureReason;
	switch(reason){
	case AMControl::NotConnectedFailure:
		definedFailureReason = AMCONTROLMOVEACTION3_NOT_CONNECTED_FAILURE;
		break;
	case AMControl::ToleranceFailure:
		definedFailureReason = AMCONTROLMOVEACTION3_TOLERANCE_FAILURE;
		retryAvailable = true;
		break;
	case AMControl::TimeoutFailure:
		definedFailureReason = AMCONTROLMOVEACTION3_TIMEOUT_FAILURE;
		retryAvailable = true;
		break;
	case AMControl::WasStoppedFailure:
		definedFailureReason = AMCONTROLMOVEACTION3_WAS_STOPPED_FAILURE;
		break;
	case AMControl::AlreadyMovingFailure:
		definedFailureReason = AMCONTROLMOVEACTION3_ALREADY_MOVING_FAILURE;
		retryAvailable = true;
		break;
	case AMControl::RedirectedFailure:
		definedFailureReason = AMCONTROLMOVEACTION3_REDIRECTED_FAILURE;
		retryAvailable = true;
		break;
	case AMControl::OtherFailure:
		definedFailureReason = AMCONTROLMOVEACTION3_OTHER_FAILURE;
		break;
	default:
		definedFailureReason = AMCONTROLMOVEACTION3_OTHER_FAILURE;
		break;
	}

	if(retryAvailable && attempts_ > retries_)
		retryAvailable = false;

	QString retryAvailableString = "No";
	if(retryAvailable)
		retryAvailableString = "Yes";
	QString fundamentalFailureMessage = QString("There was an error moving the control '%1' into position (Retry %2, attempt %3 of %4). Target: %5 %6. Actual position: %7 %8. Tolerance %9. Reason: %10.").arg(control_->name()).arg(retryAvailableString).arg(attempts_).arg(retries_+1).arg(control_->setpoint()).arg(control_->units()).arg(control_->value()).arg(control_->units()).arg(control_->tolerance()).arg(AMControl::failureExplanation(reason));
	if (reason == AMControl::WasStoppedFailure) {

		AMErrorMon::alert(this, definedFailureReason, fundamentalFailureMessage);
	} else {

		// error message with reason
		AMErrorMon::alert(this, definedFailureReason, QString("%1. Please report this problem to the beamline staff.").arg(fundamentalFailureMessage));
	}

	if(retryAvailable){
		attempts_++;

		// retry move
		int failureExplanation = control_->move(destination_);
		if(failureExplanation != AMControl::NoFailure)
			onMoveFailed(failureExplanation);
	}
	else{
		disconnect(control_, 0, this, 0);
		progressTick_.stop();
		disconnect(&progressTick_, 0, this, 0);

		setFailed(fundamentalFailureMessage);
	}
}

void AMControlMoveAction3::onMoveSucceeded()
{
	disconnect(control_, 0, this, 0);
	progressTick_.stop();
	disconnect(&progressTick_, 0, this, 0);

	setProgress(100,100);

	if(generateScanActionMessages_){

		AMAgnosticDataAPIControlMovedMessage controlMovedMessage(control_->name(), "INVALIDMOVEMENTTYPE", controlMoveInfo()->controlInfo().value(), control_->value());
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



