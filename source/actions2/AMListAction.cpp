#include "AMListAction.h"

#include <QDebug>

AMListAction::AMListAction(AMActionInfo* info, SubActionMode subActionMode, QObject *parent) :
	AMAction(info, parent)
{
	currentSubActionIndex_ = -1;	// prior to running any subactions
	subActionMode_ = subActionMode;
}

// Copy constructor. Takes care of making copies of the sub-actions
AMListAction::AMListAction(const AMListAction& other) : AMAction(other) {
	subActionMode_ = other.subActionMode_;
	currentSubActionIndex_ = -1;

	foreach(AMAction* action, other.subActions_)
		subActions_ << action->createCopy();
}
/// Destructor: deletes the sub-actions
AMListAction::~AMListAction() {
	while(!subActions_.isEmpty())
		delete subActions_.takeLast();
}


AMAction * AMListAction::subActionAt(int index)
{
	if(index < 0 || index >= subActions_.count())
		return 0;
	return subActions_.at(index);
}

const AMAction * AMListAction::subActionAt(int index) const
{
	if(index < 0 || index >= subActions_.count())
		return 0;
	return subActions_.at(index);
}

bool AMListAction::insertSubAction(AMAction *action, int index)
{
	if(state() != Constructed)
		return false;	// cannot change this once running.

	if(index<0)
		index = 0;
	if(index > subActions_.count())
		index = subActions_.count();

	emit subActionAboutToBeAdded(index);
	subActions_.insert(index, action);
	emit subActionAdded(index);

	return true;
}

AMAction * AMListAction::takeSubActionAt(int index)
{
	if(state() != Constructed)
		return 0; // cannot change this once running
	if(index <0 || index >= subActions_.count())
		return 0;

	emit subActionAboutToBeRemoved(index);
	AMAction* action = subActions_.takeAt(index);
	emit subActionRemoved(index);

	return action;
}

bool AMListAction::setSubActionMode(AMListAction::SubActionMode parallelOrSequential)
{
	if(state() != Constructed)
		return false;

	subActionMode_ = parallelOrSequential;
	return true;
}

bool AMListAction::canPause() const
{
	if(subActionMode() == SequentialMode) {
		// if we have no actions, then cannot pause; we'll complete instantly.
		if(subActionCount() == 0)
			return false;
		// if we just have one sub-action and it cannot pause, then we can't pause.
		if(subActionCount() == 1)
			return subActionAt(0)->canPause();
		// More than one action. Are we on the last action? Then whether we can pause depends on whether that last action can pause
		if(currentSubActionIndex() == subActionCount()-1)
			return currentSubAction()->canPause();
		// If we've made it here, we have more than one action and we're not on the last action. Therefore, we can pause between actions even if they can't pause themselves.
		return true;
	}

	// in parallel mode, we can pause if all sub-actions can.
	else {
		bool canDoPause = true;
		foreach(AMAction* action, subActions_)
			canDoPause &= action->canPause();

		return canDoPause;
	}
}

void AMListAction::startImplementation()
{
	if(subActionCount() == 0) {
		notifyStarted();
		notifySucceeded();	// that was quick and easy...
		return;
	}

	if(subActionMode() == SequentialMode) {
		currentSubActionIndex_ = 0;
		internalConnectAction(currentSubAction());
		notifyStarted();
		currentSubAction()->start();
	}
	// parallel mode
	else {
		foreach(AMAction* action, subActions_) {
			internalConnectAction(action);
		}
		notifyStarted();
		foreach(AMAction* action, subActions_) {
			action->start();
		}
	}
}
void AMListAction::pauseImplementation()
{
}

void AMListAction::resumeImplementation()
{
}

void AMListAction::cancelImplementation()
{
}


void AMListAction::internalOnSubActionStateChanged(int newState, int oldState)
{
	// sequential mode: could only come from the current action
	if(subActionMode() == SequentialMode) {
		switch(newState) {
		case WaitingForPrereqs:
			return;
		case Starting:
			return;
		case Running:
			if(state() == Resuming && oldState == Resuming)
				notifyResumed();
			return;
		case Pausing:
			return;
		case Paused:
			// the current action paused, so now we're paused. This will only happen if the current action supports pause and transitioned to it.
			if(state() == Pausing && oldState == Pausing)
				notifyPaused();
			return;
		case Resuming:
			return;
		case Cancelling:
			return;
		case Cancelled:
			internalDisconnectAction(currentSubAction());
			notifyCancelled();
			return;
		case Succeeded:
			internalDisconnectAction(currentSubAction());
			// Are we on the last action? If we are, we're done no matter what [even if supposed to be pausing]
			if(++currentSubActionIndex_ == subActionCount()-1) {
				notifySucceeded();
				return;
			}
			// if we were waiting to pause and had to wait for a (non-pausable) action to finish, we're there now.
			if(state() == Pausing) {
				notifyPaused();
				return;
			}
			// if we were running and an action has completed, time to move on to next
			if(state() == Running) {
				internalConnectAction(currentSubAction());
				currentSubAction()->start();
				return;
			}
		case Failed:
			internalDisconnectAction(currentSubAction());
			notifyFailed();
			return;
		}
		qWarning() << "AMListAction: Unexpected sub-action state transition. Sub-action state:" << newState << "Sub-action previous state:" << oldState << "Our state:" << (int)state() << "Our previous state:" << (int)previousState();
	}

	// parallel mode:
	else {

	}
}

void AMListAction::internalOnSubActionProgressChanged(double numerator, double denominator)
{
}

void AMListAction::internalOnSubActionStatusTextChanged(const QString &statusText)
{
}



void AMListAction::internalConnectAction(AMAction *action)
{
	connect(action, SIGNAL(stateChanged(int,int)), this, SLOT(internalOnSubActionStateChanged(int,int)));
	connect(action, SIGNAL(progressChanged(double,double)), this, SLOT(internalOnSubActionProgressChanged(double,double)));
	connect(action, SIGNAL(statusTextChanged(QString)), this, SLOT(internalOnSubActionStatusTextChanged(QString)));
}

void AMListAction::internalDisconnectAction(AMAction *action)
{
	disconnect(action, SIGNAL(stateChanged(int,int)), this, SLOT(internalOnSubActionStateChanged(int,int)));
	disconnect(action, SIGNAL(progressChanged(double,double)), this, SLOT(internalOnSubActionProgressChanged(double,double)));
	disconnect(action, SIGNAL(statusTextChanged(QString)), this, SLOT(internalOnSubActionStatusTextChanged(QString)));
}
