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


#include "AMAction.h"

// Used for qWarning() messages that may be helpful to people trying to debug AMAction implementations.
#include <QDebug>

#include "util/AMErrorMonitor.h"
#include <QStringBuilder>

// Constructor: create an action to run the specified AMActionInfo.
AMAction::AMAction(AMActionInfo* info, QObject *parent)
	: QObject(parent)
{
	info_ = info;
	progress_ = QPair<double,double>(-1.0, -1.0);
	state_ = previousState_ = Constructed;
	statusText_ = "Not yet started";
	secondsSpentPaused_ = secondsSpentWaitingForPrereqs_ = 0;
	parentAction_ = 0;

	connect(info_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(expectedDurationChanged(double)));

	prereqBehaviour_ = WaitPrereqBehaviour;
	failureResponseInActionRunner_ = MoveOnResponse;
	failureResponseAsSubAction_ = MoveOnResponse;
}

// Copy constructor. Takes care of making copies of the info and prerequisites.
AMAction::AMAction(const AMAction& other)
	: QObject()
{
	progress_ = QPair<double,double>(-1.0, -1.0);
	state_ = previousState_ = Constructed;
	statusText_ = "Not yet started";
	secondsSpentPaused_ = secondsSpentWaitingForPrereqs_ = 0;
	parentAction_ = 0;

	prereqBehaviour_ = other.prereqBehaviour_;
	failureResponseInActionRunner_ = other.failureResponseInActionRunner_;
	failureResponseAsSubAction_ = other.failureResponseAsSubAction_;

	info_ = other.info()->createCopy();
	connect(info_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(expectedDurationChanged(double)));
	foreach(AMActionPrereq* prereq, other.prereqs_)
		prereqs_ << prereq->createCopy();
}


// Destructor: deletes the info and prerequisites
AMAction::~AMAction() {
	// qDebug() << "Deleting action:" << info_->shortDescription();

	delete info_;
	while(!prereqs_.isEmpty())
		delete prereqs_.takeLast();
}



bool AMAction::start()
{
	if(state() != Constructed)
		return false;

	startDateTime_ = QDateTime::currentDateTime();

	// Do we have prerequisites that aren't satisfied?
	if(prereqs_.count() && !prereqsSatisfied()) {

		PrereqBehaviour p = prereqBehaviour();
		if(p == PromptUserPrereqBehaviour)
			p = promptUserForPrereqBehaviour();

		switch(p) {
		case WaitPrereqBehaviour:
			setState(WaitingForPrereqs);
			setProgress(0,0);
			// connect prereq changed signals to our slot, so we can detect when they are satisfied
			foreach(AMActionPrereq* prereq, prereqs_)
				connect(prereq, SIGNAL(satisfiedChanged(bool)), this, SLOT(internalOnPrereqChanged()));
			// the prereqs might have been satisfied in the time we were waiting for the user response. Check again...
			internalOnPrereqChanged();
			break;
		case CancelActionPrereqBehaviour:
			endDateTime_ = QDateTime::currentDateTime();
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, "The action '" % info()->shortDescription() % "' was cancelled because it had prerequisites that weren't ready."));
			setState(Cancelled);	// we don't need to go through Cancelling, because we haven't started yet.
			break;
		case FailActionPrereqBehaviour:
		default:
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, "The action '" % info()->shortDescription() % "' failed because it had prerequisites that weren't ready."));
			setState(Failed);
			break;
		}
	}

	// No prereqs... start the action directly.
	else {
		setState(Starting);
		startImplementation();
	}
	return true;
}

bool AMAction::cancel()
{
	switch(state()) {

	case Cancelling:
	case Cancelled:
	case Succeeded:
	case Failed:
		qWarning() << "AMAction: Warning: You cannot cancel this action because it is already in a final state.";
		return false;	// can't cancel from these states. Already cancelling, or in a final state.

	case Constructed:
		endDateTime_ = QDateTime::currentDateTime();
		setState(Cancelled);	// can cancel directly; haven't started yet
		return true;

	case WaitingForPrereqs:
		foreach(AMActionPrereq* prereq, prereqs_)
			disconnect(prereq, SIGNAL(satisfiedChanged(bool)), this, SLOT(internalOnPrereqChanged()));
		endDateTime_ = QDateTime::currentDateTime();
		setState(Cancelled);
		return true;

	case Starting:
	case Running:
	case Pausing:
	case Paused:
	case Resuming:
		setState(Cancelling);
		cancelImplementation();
		return true;
	default:
		return false;
	}
}

bool AMAction::pause()
{
	if(!canPause()) {
		qWarning() << "AMAction: Warning: Tried to pause an action that specifies that it cannot pause (at the moment).";
		return false;
	}

	if(state() == WaitingForPrereqs) {
		lastPausedAt_ = QDateTime::currentDateTime();
		setProgress(0,0	);
		setState(Paused);	// that's all we need... As long as the state is set to Paused, we won't advance if the prereqs become satisfied until we've been resumed.
		return true;
	}
	if(state() == Running) {
		lastPausedAt_ = QDateTime::currentDateTime();
		setProgress(0,0);
		setState(Pausing);
		pauseImplementation();
		return true;
	}

	qWarning() << "AMAction: Warning: You can not pause this action, because it is not in a pausable state.";
	return false;	// any other state: cannot pause from there.
}

bool AMAction::resume()
{
	if(state() == Paused) {
		secondsSpentPaused_ += double(lastPausedAt_.msecsTo(QDateTime::currentDateTime()))/1000.0;

		if(previousState() == WaitingForPrereqs) {
			setState(WaitingForPrereqs);
			internalOnPrereqChanged();	// review all prereqs to see if we should carry on.  They might have changed while paused.
			return true;
		}
		else {
			setState(Resuming);
			resumeImplementation();
			return true;
		}
	}

	qWarning() << "AMAction: Warning: You can not resume this action because it is not paused.";
	return false;	// cannot resume from any other states except Pause.
}

bool AMAction::addPrereq(AMActionPrereq *newPrereq)
{
	if(state() != Constructed) {
		qWarning() << "AMAction: Warning: You can not set action prerequisites after the action is running.";
		return false;
	}

	prereqs_ << newPrereq;
	return true;
}

bool AMAction::setPrereqBehaviour(AMAction::PrereqBehaviour prereqBehaviour)
{
	if(state() != Constructed) {
		qWarning() << "AMAction: Warning: You can not set action prerequisites after the action is running.";
		return false;
	}

	prereqBehaviour_ = prereqBehaviour;
	return true;
}

void AMAction::notifyStarted()
{
	if(state() != Starting) {
		qWarning() << "AMAction: Warning: An implementation told us it has started, but we did not tell it to start.";
		qWarning() << "    Action name:" << info()->name();
		qWarning() << "    Current state:" << stateDescription(state());
		return;
	}

	setState(Running);
}

void AMAction::notifySucceeded()
{
	switch(state()) {

	case Starting:
	case Running:
	case Pausing:
	case Paused:
	case Resuming:
	case Cancelling:
		endDateTime_ = QDateTime::currentDateTime();
		setState(Succeeded);
		break;

	default:
		qWarning() << "AMAction: Warning: An implementation told us it had succeeded before it could possibly be running.";
		qWarning() << "    Action name:" << info()->name();
		qWarning() << "    Current state:" << stateDescription(state());
	}
}

void AMAction::notifyFailed()
{
	switch(state()) {

	case Starting:
	case Running:
	case Pausing:
	case Paused:
	case Resuming:
	case Cancelling:
		endDateTime_ = QDateTime::currentDateTime();
		setState(Failed);
		break;

	default:
		qWarning() << "AMAction: Warning: An implementation told us it had failed before it could possibly be running.";
	}
}

void AMAction::notifyPaused()
{
	if(state() != Pausing) {
		qWarning() << "AMAction: Warning: An action notified us it had paused, when it should not be pausing.";
		return;
	}
	setState(Paused);
}

void AMAction::notifyResumed()
{
	if(state() != Resuming) {
		qWarning() << "AMAction: Warning: An action notified us it had resumed, when it should not be resuming.";
		return;
	}
	setState(Running);
}

void AMAction::notifyCancelled()
{
	// only called by implementation when finished cancelling.
	if(state() != Cancelling) {
		qWarning() << "AMAction: Warning: An action notified us it was cancelled before we could possibly cancel it.";
		return;
	}
	endDateTime_ = QDateTime::currentDateTime();
	setState(Cancelled);
}

void AMAction::internalOnPrereqChanged()
{
	if(state() != WaitingForPrereqs)
		return;	// only check this and possibly move on if we're in the correct state. If we're paused while the prereqs are connected, we won't enter the remainder of this function.

	if(prereqsSatisfied()) {
		// we're done here; disconnect prereqs and move on to the Starting state
		foreach(AMActionPrereq* prereq, prereqs_) {
			disconnect(prereq, SIGNAL(satisfiedChanged(bool)), this, SLOT(internalOnPrereqChanged()));
		}
		secondsSpentWaitingForPrereqs_ = elapsedTime() - secondsSpentPaused_;
		setState(Starting);	// move on to the next state
		startImplementation();
	}
}

bool AMAction::prereqsSatisfied() const
{
	bool allSatisfied = true;
	foreach(AMActionPrereq* prereq, prereqs_)
		allSatisfied &= prereq->satisfied();
	return allSatisfied;
}

#include <QMessageBox>
#include <QStringBuilder>
#include <QPushButton>

AMAction::PrereqBehaviour AMAction::promptUserForPrereqBehaviour()
{
	QMessageBox box;
	box.setWindowTitle("Prerequisites not ready");
	box.setText("There are some prerequisites for running this action that aren't ready yet. Would you like to wait for them to be ready, or cancel the action?");

	QStringList explanations;
	foreach(AMActionPrereq* prereq, prereqs_)
		if(!prereq->satisfied())
			explanations << prereq->explanation();

	box.setInformativeText("Action: " % info()->shortDescription() % "\n\n" % explanations.join("\n"));
	QPushButton* waitButton = new QPushButton("Wait for Prerequsites");
	box.addButton(waitButton, QMessageBox::AcceptRole);
	QPushButton* cancelButton = new QPushButton("Cancel Action");
	box.addButton(cancelButton, QMessageBox::RejectRole);

	box.exec();
	if(box.clickedButton() == waitButton)
		return WaitPrereqBehaviour;
	else
		return CancelActionPrereqBehaviour;
}

QString AMAction::stateDescription(AMAction::State state)
{
	switch(state) {
	case Constructed:
		return "Not yet started";
	case WaitingForPrereqs:
		return "Wating for Prerequisites";
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
	default:
		return "Invalid State";
	}
}

void AMAction::setState(AMAction::State newState) {
	previousState_ = state_;
	state_ = newState;
	// qDebug() << "AMAction:" << info()->name() << ": Entering state:" << stateDescription(state_);
	setStatusText(stateDescription(state_));
	emit stateChanged(state_, previousState_);
	// convenience signals for final states:
	if(state_ == Succeeded)
		emit succeeded();
	else if(state_ == Failed)
		emit failed();
	else if(state_ == Cancelled)
		emit cancelled();
}

double AMAction::pausedTime() const
{
	if(!startDateTime_.isValid())
		return -1.0;

	// if we're still in the paused state, secondsSpentPaused_ hasn't been updated yet. Need to add the length of the current pause break;
	if(state() == Paused)
		return secondsSpentPaused_ + double(lastPausedAt_.msecsTo(QDateTime::currentDateTime()))/1000.0;
	else
		return secondsSpentPaused_;
}

double AMAction::waitingForPrereqsTime() const
{
	if(!startDateTime_.isValid())
		return -1.0;

	// Are we before secondsSpentWaitingForPrereqs_ has been set?
	if(state() == WaitingForPrereqs ||
			(state() == Paused && previousState() == WaitingForPrereqs))
		return elapsedTime() - pausedTime();	// Our whole elapsed time so far is WaitingForPrereqs time. Except for any paused time, which needs to be counted separately.

	// otherwise, this has been set already. Just use it.
	return secondsSpentWaitingForPrereqs_;
}
