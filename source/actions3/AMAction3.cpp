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


#include "AMAction3.h"

#include <QStringBuilder>

#include "util/AMErrorMonitor.h"

// Constructor: create an action to run the specified AMActionInfo.
AMAction3::AMAction3(AMActionInfo3* info, QObject *parent)
	: QObject(parent)
{
	info_ = info;
	progress_ = QPair<double,double>(-1.0, -1.0);
	state_ = Constructed;
	statusText_ = "Not yet started";
	secondsSpentPaused_ = 0;
	parentAction_ = 0;

	connect(info_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(expectedDurationChanged(double)));

	failureResponseInActionRunner_ = MoveOnResponse;
	failureResponseAsSubAction_ = MoveOnResponse;

	generateScanActionMessages_ = false;
	isScheduledForDeletion_ = false;
	isLoggingFinished_ = true;
}

// Copy constructor. Takes care of making copies of the info and prerequisites.
AMAction3::AMAction3(const AMAction3& other, QObject *parent )
	: QObject(parent)
{
	progress_ = QPair<double,double>(-1.0, -1.0);
	state_ = Constructed;
	statusText_ = "Not yet started";
	secondsSpentPaused_ = 0;
	parentAction_ = 0;
	failureMessage_ = other.failureMessage();

	failureResponseInActionRunner_ = other.failureResponseInActionRunner_;
	failureResponseAsSubAction_ = other.failureResponseAsSubAction_;

	isScheduledForDeletion_ = false;
	isLoggingFinished_ = true;

	info_ = other.info()->createCopy();
	generateScanActionMessages_ = other.generateScanActionMessages();
	connect(info_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(expectedDurationChanged(double)));
}


// Destructor: deletes the info and prerequisites
AMAction3::~AMAction3() {
	if(info_){
		info_->deleteLater();
		info_ = 0;
	}
}

bool AMAction3::isScheduledForDeletion() const{
	return isScheduledForDeletion_;
}

bool AMAction3::isLoggingFinished() const{
	return isLoggingFinished_;
}

double AMAction3::elapsedTime() const
{
	if(!startDateTime_.isValid())
		return 0;

	return double(startDateTime_.msecsTo(QDateTime::currentDateTime()))/1000.0;
}

double AMAction3::pausedTime() const
{
	if(!startDateTime_.isValid())
		return 0;

	// if we're still in the paused state, secondsSpentPaused_ hasn't been updated yet. Need to add the length of the current pause break
	if(isPaused())
		return secondsSpentPaused_ + double(lastPausedAt_.msecsTo(QDateTime::currentDateTime()))/1000.0;
	else
		return secondsSpentPaused_;
}

double AMAction3::runningTime() const
{
	if(!startDateTime_.isValid())
		return 0;

	return elapsedTime() - pausedTime();
}

QString AMAction3::stateDescription(AMAction3::State state)
{
	switch(state) {
	case Constructed:
		return "Not yet started";
	case Starting:
		return "Starting";
	case Running:
		return "Running";
	case Pausing:
		return "Pausing";
	case Paused:
		return "Paused";
	case Resuming:
		return "Resuming";
	case Cancelling:
		return "Cancelling";
	case Cancelled:
		return "Cancelled";
	case Succeeded:
		return "Succeeded";
	case Failed:
		return "Failed";
	case Skipping:
		return "Skipping";
	default:
		return "Invalid State";
	}
}

bool AMAction3::start()
{
	if (canChangeState(Starting)){

		startDateTime_ = QDateTime::currentDateTime();

		setState(Starting);
		startImplementation();

		return true;
	}

	return false;
}

bool AMAction3::cancel()
{
	if (canChangeState(Cancelling)){

		setState(Cancelling);
		cancelImplementation();
		return true;
	}

	AMErrorMon::debug(this, AMACTION3_CANNOT_CANCEL_NOT_IN_FINAL_STATE, QString("You cannot cancel this action because it is already in a final state (%1).").arg(state_));
	return false;
}

bool AMAction3::pause()
{
	if(!canPause()) {
		AMErrorMon::debug(this, AMACTION3_CANNOT_PAUSE_ACCORDING_TO_ACTION, "Tried to pause an action that specifies that it cannot pause (at the moment).");
		return false;
	}

	if(canChangeState(Pausing)) {

		lastPausedAt_ = QDateTime::currentDateTime();
//		setProgress(0,0);
		setState(Pausing);
		pauseImplementation();
		return true;
	}

	AMErrorMon::debug(this, AMACTION3_CANNOT_PAUSE_NOT_IN_PAUSABLE_STATE, "You can not pause this action, because it is not in a pausable state.");
	return false;	// any other state: cannot pause from there.
}

bool AMAction3::resume()
{
	if(canChangeState(Resuming)) {

		secondsSpentPaused_ += double(lastPausedAt_.msecsTo(QDateTime::currentDateTime()))/1000.0;
		setState(Resuming);
		resumeImplementation();
		return true;
	}

	AMErrorMon::debug(this, AMACTION3_CANNOT_RESUME_NOT_CURRENTLY_PAUSED, "You can not resume this action because it is not paused.");
	return false;	// cannot resume from any other states except Pause.
}

bool AMAction3::skip(const QString &command)
{
	if (!canSkip()){

		AMErrorMon::debug(this, AMACTION3_CANNOT_SKIP_NOT_POSSIBLE, "Tried to skip an action that doesn't support skipping.");
		return false;
	}

	if (canChangeState(Skipping)){

		setState(Skipping);
		skipImplementation(command);
		return true;
	}

	AMErrorMon::debug(this, AMACTION3_CANNOT_SKIP_NOT_CURRENTLY_RUNNING, "Could not skip this action because it was not running to begin with.");
	return false;
}

void AMAction3::scheduleForDeletion(){
	if(!isScheduledForDeletion_){
		isScheduledForDeletion_ = true;
		scheduleForDeletionImplementation();
	}
}

void AMAction3::setIsLoggingFinished(bool isLoggingFinished){
	if(isLoggingFinished_ != isLoggingFinished){
		isLoggingFinished_ = isLoggingFinished;
		emit isLoggingFinishedChanged(isLoggingFinished_);
		if(isLoggingFinished_)
			emit loggingIsFinished();
	}
}

/// ================ protected =====================
void AMAction3::setProgress(double numerator, double denominator)
{
	progress_ = QPair<double,double>(numerator,denominator);
	emit progressChanged(numerator, denominator);
}

void AMAction3::setExpectedDuration(double expectedTotalTimeInSeconds)
{
	info_->setExpectedDuration(expectedTotalTimeInSeconds);
}

void AMAction3::setStatusText(const QString& statusText)
{
	emit statusTextChanged(statusText_ = statusText);
}

void AMAction3::scheduleForDeletionImplementation(){
	if(isLoggingFinished())
		deleteLater();
	else
		connect(this, SIGNAL(loggingIsFinished()), this, SLOT(deleteLater()));
}

void AMAction3::setStarted()
{
	if (canChangeState(Running)){

		setProgress(0, info_->expectedDuration());
		setState(Running);
		emit started();
	}

	else
		AMErrorMon::debug(this, AMACTION3_NOTIFIED_STARTED_BUT_NOT_TOLD_TO_START, QString("An implementation told us it has started, but we did not tell it to start. Action name: %1. Current state: %2.").arg(info()->name()).arg(stateDescription(state())) );
}

void AMAction3::setSucceeded()
{
	if (canChangeState(Succeeded)){

		endDateTime_ = QDateTime::currentDateTime();
		setState(Succeeded);
		emit succeeded();
	}

	else
		AMErrorMon::debug(this, AMACTION3_NOTIFIED_SUCCEEDED_BUT_NOT_YET_POSSIBLE, QString("An implementation told us it had succeeded before it could possibly be running. Action name: %1. Current state: %2.").arg(info()->name()).arg(stateDescription(state())) );
}

void AMAction3::setFailed(const QString &message)
{
	if (canChangeState(Failed)){

		endDateTime_ = QDateTime::currentDateTime();
		failureMessage_ = message;
		setState(Failed);
		emit failed();
	}

	else
		AMErrorMon::debug(this, AMACTION3_NOTIFIED_FAILED_BUT_NOT_YET_POSSIBLE, "An implementation told us it had failed before it could possibly be running.");
}

void AMAction3::setPaused()
{
	if (canChangeState(Paused)){

		setState(Paused);
		emit paused();
	}

	else
		AMErrorMon::debug(this, AMACTION3_NOTIFIED_PAUSED_BUT_NOT_CURRENTLY_POSSIBLE, "An action notified us it had paused, when it should not be pausing.");
}

void AMAction3::setResumed()
{
	if (canChangeState(Running)){

		setState(Running);
		emit resumed();
	}

	else
		AMErrorMon::debug(this, AMACTION3_NOTIFIED_RESUMED_BUT_NOT_CURRENTLY_POSSIBLE, "An action notified us it had resumed, when it should not be resuming.");
}

void AMAction3::setCancelled()
{
	if (canChangeState(Cancelled)){

		endDateTime_ = QDateTime::currentDateTime();
		setState(Cancelled);
		emit cancelled();
	}

	else
		AMErrorMon::debug(this, AMACTION3_NOTIFIED_CANCELLED_BUT_NOT_YET_POSSIBLE, "An action notified us it was cancelled before we could possibly cancel it.");
}

void AMAction3::setSkipped()
{
	setSucceeded();
}

bool AMAction3::canChangeState(State newState) const
{
	bool canTransition = false;

	switch(newState){

	case Constructed:
		break;

	case Starting:
		if (state_ == Constructed)
			canTransition = true;
		break;

	case Running:
		if (state_ == Starting || state_ == Resuming)
			canTransition = true;
		break;

	case Pausing:
		if (canPause() && state_ == Running)
			canTransition = true;
		break;

	case Paused:
		if (state_ == Pausing)
			canTransition = true;
		break;

	case Resuming:
		if (state_ == Paused)
			canTransition = true;
		break;

	case Cancelling:
		if (!inFinalState())
			canTransition = true;
		break;

	case Cancelled:
		if (state_ == Cancelling)
			canTransition = true;
		break;

	case Succeeded:
		if (state_ == Running || state_ == Starting || state_ == Skipping)
			canTransition = true;
		break;

	case Failed:
		if (state_ == Starting || state_ == Running || state_ == Pausing
				|| state_ == Paused || state_ == Resuming || state_ == Cancelling || state_ == Skipping)
			canTransition = true;
		break;

	case Skipping:
		if (canSkip() && state_ == Running)
			canTransition = true;
		break;
	}

	return canTransition;
}

void AMAction3::setState(AMAction3::State newState)
{
	if (!canChangeState(newState))
		return;

	AMAction3::State fromState = state_;

	state_ = newState;

	setStatusText(stateDescription(state_));

	emit stateChanged(fromState, state_);
}
