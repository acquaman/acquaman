#include "AMAction3.h"

// Used for qWarning() messages that may be helpful to people trying to debug AMAction implementations.
#include <QDebug>

#include "util/AMErrorMonitor.h"
#include <QStringBuilder>

// Constructor: create an action to run the specified AMActionInfo.
AMAction3::AMAction3(AMActionInfo3* info, QObject *parent)
	: QObject(parent)
{
	info_ = info;
	progress_ = QPair<double,double>(-1.0, -1.0);
	state_ = previousState_ = Constructed;
	statusText_ = "Not yet started";
    secondsSpentPaused_ = 0;
	parentAction_ = 0;

	connect(info_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(expectedDurationChanged(double)));

	failureResponseInActionRunner_ = MoveOnResponse;
	failureResponseAsSubAction_ = MoveOnResponse;
}

// Copy constructor. Takes care of making copies of the info and prerequisites.
AMAction3::AMAction3(const AMAction3& other)
	: QObject()
{
	progress_ = QPair<double,double>(-1.0, -1.0);
	state_ = previousState_ = Constructed;
	statusText_ = "Not yet started";
    secondsSpentPaused_ = 0;
	parentAction_ = 0;

	failureResponseInActionRunner_ = other.failureResponseInActionRunner_;
	failureResponseAsSubAction_ = other.failureResponseAsSubAction_;

	info_ = other.info()->createCopy();
	connect(info_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(expectedDurationChanged(double)));
}


// Destructor: deletes the info and prerequisites
AMAction3::~AMAction3() {
	// qDebug() << "Deleting action:" << info_->shortDescription();

	delete info_;
}



bool AMAction3::start()
{
	if(state() != Constructed)
		return false;

	startDateTime_ = QDateTime::currentDateTime();
    setState(Starting);
    startImplementation();

	return true;
}

bool AMAction3::cancel()
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

bool AMAction3::pause()
{
	if(!canPause()) {
		qWarning() << "AMAction: Warning: Tried to pause an action that specifies that it cannot pause (at the moment).";
		return false;
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

bool AMAction3::resume()
{
	if(state() == Paused) {
		secondsSpentPaused_ += double(lastPausedAt_.msecsTo(QDateTime::currentDateTime()))/1000.0;
        setState(Resuming);
        resumeImplementation();
        return true;
	}

	qWarning() << "AMAction: Warning: You can not resume this action because it is not paused.";
	return false;	// cannot resume from any other states except Pause.
}

void AMAction3::notifyStarted()
{
	if(state() != Starting) {
		qWarning() << "AMAction: Warning: An implementation told us it has started, but we did not tell it to start.";
		qWarning() << "    Action name:" << info()->name();
		qWarning() << "    Current state:" << stateDescription(state());
		return;
	}

	setState(Running);
}

void AMAction3::notifySucceeded()
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

void AMAction3::notifyFailed()
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

void AMAction3::notifyPaused()
{
	if(state() != Pausing) {
		qWarning() << "AMAction: Warning: An action notified us it had paused, when it should not be pausing.";
		return;
	}
	setState(Paused);
}

void AMAction3::notifyResumed()
{
	if(state() != Resuming) {
		qWarning() << "AMAction: Warning: An action notified us it had resumed, when it should not be resuming.";
		return;
	}
	setState(Running);
}

void AMAction3::notifyCancelled()
{
	// only called by implementation when finished cancelling.
	if(state() != Cancelling) {
		qWarning() << "AMAction: Warning: An action notified us it was cancelled before we could possibly cancel it.";
		return;
	}
	endDateTime_ = QDateTime::currentDateTime();
	setState(Cancelled);
}

#include <QMessageBox>
#include <QStringBuilder>
#include <QPushButton>

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
	default:
		return "Invalid State";
	}
}

void AMAction3::setState(AMAction3::State newState) {

    if (!canChangeState(newState))
        return;

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
        if (state_ == Running)
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
        canTransition = true;
        break;

    case Cancelled:
        if (state_ == Cancelling)
            canTransition = true;
        break;

    case Succeeded:
        if (state_ == Running || state_ == Starting)
            canTransition = true;
        break;

    case Failed:
        if (state_ == Starting || state_ == Running || state_ == Pausing
                || state_ == Paused || state_ == Resuming || state_ == Cancelling)
            canTransition = true;
        break;
    }

    return canTransition;
}

double AMAction3::pausedTime() const
{
	if(!startDateTime_.isValid())
		return -1.0;

	// if we're still in the paused state, secondsSpentPaused_ hasn't been updated yet. Need to add the length of the current pause break;
	if(state() == Paused)
		return secondsSpentPaused_ + double(lastPausedAt_.msecsTo(QDateTime::currentDateTime()))/1000.0;
	else
		return secondsSpentPaused_;
}
