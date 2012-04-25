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


#include "AMListAction.h"

#include <QStringBuilder>

// Used for qWarning() messages that may be useful to developers trying to debug action implementatons
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
	currentSubActionIndex_ = -1; // prior to running an subactions

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
	if(!(state() == Constructed || state() == Starting)) {
		qWarning() << "AMListAction: Cannot add sub-actions once the action is already running.";
		return false;
	}

	if(index<0 || index > subActions_.count())
		index = subActions_.count();

	emit subActionAboutToBeAdded(index);
	subActions_.insert(index, action);
	emit subActionAdded(index);

	return true;
}

AMAction * AMListAction::takeSubActionAt(int index)
{

	if(!(state() == Constructed || state() == Starting)) {
		qWarning() << "AMListAction: Cannot remove sub-actions once the action is already running.";
		return 0;
	}

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
			return ((subActionAt(0)->state() == Running
					 || subActionAt(0)->state() == WaitingForPrereqs)
					&& subActionAt(0)->canPause());
		// More than one action. Are we on the last action? Then whether we can pause depends on whether that last action can pause
		if(currentSubActionIndex() == subActionCount()-1)
			return ((currentSubAction()->state() == Running
					 || currentSubAction()->state() == WaitingForPrereqs)
					&& currentSubAction()->canPause());
		// If we've made it here, we have more than one action and we're not on the last action. Therefore, at least we can pause between actions even if they can't pause themselves.
		return true;
	}

	// in parallel mode, we can pause if all still-running sub-actions can.  If ALL the actions are in a final state, then we won't be Running, so the base class will never let someone call pause().
	else {
		bool canDoPause = true;
		foreach(AMAction* action, subActions_)
			canDoPause &= (action->inFinalState()
						   || ((action->state() == Running || action->state() == WaitingForPrereqs)
							   && action->canPause()));

		return canDoPause;
	}
}

void AMListAction::startImplementation()
{
	// if this was called by the base class, we know that we are in the Starting state.

	// no actions? That's easy...
	if(subActionCount() == 0) {
		notifyStarted();
		notifySucceeded();	// done and done.
		return;
	}

	if(subActionMode() == SequentialMode) {
		emit currentSubActionChanged(currentSubActionIndex_ = 0);
		notifyStarted();
		internalConnectAction(currentSubAction());
		currentSubAction()->start();
	}
	// parallel mode
	else {
		notifyStarted();
		foreach(AMAction* action, subActions_) {
			internalConnectAction(action);
		}
		foreach(AMAction* action, subActions_) {
			action->start();
		}
	}
}
void AMListAction::pauseImplementation()
{
	// if this was called by the base class, we know that we are in the Pausing state, and were in the Running state. We also know that canPause() is true.

	// sequential mode:
	////////////////////////
	if(subActionMode() == SequentialMode) {
		if(currentSubAction()) {
			// Can this action pause?
			if(currentSubAction()->canPause())
				currentSubAction()->pause();
			// If it can't, we'll pause by stopping at the next action. When the current action transitions to Succeeded, we simply won't start the next one and will notifyPaused(). In this case, we could stay at Pausing for quite some time until the current action finishes.
		}
		else {
			qWarning() << "AMListAction: Warning: pauseImplementation() was called at an unexpected time. No action is running.";
		}
		// when the action changes to the Paused state, we'll pick that up in internalOnSubActionStateChanged() and notifyPaused().
	}

	// parallel mode:
	/////////////////////
	else {
		// because we can trust that canPause() was true for all, let's just tell all of our actions to pause.
		foreach(AMAction* action, subActions_) {
			action->pause();	// this will fail for completed actions, but that's OK.
		}
		// when ALL of the still-running actions change to the Paused state, we'll pick that up in internalOnSubActionStateChanged() and notifyPaused().
	}
}

void AMListAction::resumeImplementation()
{
	// If this is called by the base class, we know that we're now in Resuming and used to be in Pause.
	if(subActionMode() == SequentialMode) {
		// The currentSubAction() will either be Paused (if it supported pausing when we were paused), or Constructed (if the last action didn't support pausing and completed; now we're at the beginning of the next one).
		if(currentSubAction()) {
			if(currentSubAction()->state() == Paused)
				currentSubAction()->resume();
			else if(currentSubAction()->state() == Constructed) {
				internalConnectAction(currentSubAction());
				currentSubAction()->start();
			}
			else
				qWarning() << "AMListAction: Warning: Asked to resume, but the current action is not paused.";
		}
		else {
			qWarning() << "AMListAction: Warning: Asked to resume unexpectedly: there is no current action to resume.";
		}
	}
	// Parallel mode:
	////////////////////////
	else {
		// we know that we were in Paused (aka: all actions finished pausing), and that canPause() was true, so we can tell them all to resume.
		foreach(AMAction* action, subActions_) {
			if(!action->inFinalState())
				action->resume();
		}
	}
}

void AMListAction::cancelImplementation()
{
	// sequential mode
	if(subActionMode() == SequentialMode) {
		if(currentSubAction()) {
			if(currentSubAction()->state() == Constructed)	// this sub-action not connected or run yet. Don't need to cancel it. (This could happen if we are paused between actions and currentSubAction() hasn't been started yet.)
				notifyCancelled();
			else
				currentSubAction()->cancel(); // action is already connected and running. Need to cancel it. We'll pick it up in internalOnSubActionStateChanged
		}
		else {
			notifyCancelled();
		}
	}
	// Parallel mode:
	else {
		// this won't be called unless startImplementation() has already been given. In that case, all actions have been connected and started. Try to cancel all the ones that aren't finished already.
		foreach(AMAction* action, subActions_)
			if(!action->inFinalState())
				action->cancel();
	}
}


void AMListAction::internalOnSubActionStateChanged(int newState, int oldState)
{
	Q_UNUSED(oldState)

	// sequential mode: could only come from the current action
	if(subActionMode() == SequentialMode) {
		switch(newState) {
		case WaitingForPrereqs:
			// If we were paused between actions and resuming, the next action is now running...
			if(state() == Resuming)
				notifyResumed();
			return;
		case Starting:
			// If we were paused between actions and resuming, the next action is now running...
			if(state() == Resuming)
				notifyResumed();
			return;
		case Running:
			// If we had a current action paused:
			if(state() == Resuming)
				notifyResumed();
			return;
		case Pausing:
			return;
		case Paused:
			// the current action paused, so now we're paused. This will only happen if the current action supports pause and transitioned to it.
			if(state() == Pausing) {
				notifyPaused();
			}
			else {
				qWarning() << "AMListAction: Warning: A sub-action was paused without cancelling its parent list action. This should not happen.";
			}
			return;
		case Resuming:
			return;
		case Cancelling:
			return;
		case Cancelled:
			if(state() == Cancelling) {
				internalDisconnectAction(currentSubAction());
				notifyCancelled();
			}
			else {
				qWarning() << "AMListAction: Warning: A sub-action was cancelled without cancelling its parent list action. This should not happen.";
			}
			return;
		case Succeeded:
			// in sequential mode, always move on to the next action here.
			internalDisconnectAction(currentSubAction());
			emit currentSubActionChanged(++currentSubActionIndex_);
			// Are we done the last action? If we are, we're done no matter what [even if supposed to be pausing]
			if(currentSubActionIndex_ == subActionCount()) {
				notifySucceeded();
				return;
			}
			// if we were waiting to pause and had to wait for a (non-pausable) action to finish, we're there now.
			if(state() == Pausing) {
				notifyPaused();
				return;
			}
			// if we were running and an action has completed, time to start the next action. currentSubAction() has already been advanced for us.
			if(state() == Running) {
				internalConnectAction(currentSubAction());
				currentSubAction()->start();
				return;
			}
			qWarning() << "AMListAction: Warning: A sub-action succeeded when we weren't expecting it to be running.";
			return;
		case Failed:
			internalDisconnectAction(currentSubAction());
			notifyFailed();
			return;
		}
	}

	// parallel mode:
	/////////////////////////
	else {
		switch(newState) {
		case WaitingForPrereqs:
			return;
		case Starting:
			return;
		case Running:
			if(state() == Resuming) {
				if(internalAllActionsRunningOrFinal())
					notifyResumed();
			}
			return;
		case Pausing:
			return;
		case Paused:
			if(state() == Pausing) {
				// one of them paused. Are all the actions paused now?
				if(internalAllActionsPausedOrFinal())
					notifyPaused();
			}
			else
				qWarning() << "AMListAction: Warning: A sub-action was paused without pausing its parent list action. This should not happen.";
			return;
		case Resuming:
			return;
		case Cancelling:
			return;
		case Cancelled:
			if(state() != Cancelling)
				qWarning() << "AMListAction: Warning: A sub-action was cancelled with cancelling its parent list action. This should not happen.";
			// Continue on to common handling with Succeeded and Failed:
		case Succeeded:
		case Failed:
			// only do something here if all actions are done.
			if(internalAllActionsInFinalState()) {
				// disconnect all actions
				foreach(AMAction* action, subActions_)
					internalDisconnectAction(action);
				// Any failures?
				if(internalAnyActionsFailed())
					notifyFailed();
				// Any cancelled?
				else if(internalAnyActionsCancelled())
					notifyCancelled();
				else	 // well, I guess they're all good then. We're done!
					notifySucceeded();
			}
			return;
		}
	}
}

void AMListAction::internalOnSubActionProgressChanged(double numerator, double denominator)
{
	// sequential mode:
	if(subActionMode() == SequentialMode) {
		// If all still-running actions specify an expected duration, then we use this to specify the relative size of each action's "work chunk". The denominator of our progress will be in total (expected) seconds for all actions to complete.
		if(internalAllActionsHaveExpectedDuration()) {
			double totalNumerator = 0, totalDenominator = 0;
			for(int i=0, cc=subActionCount(); i<cc; i++) {
				AMAction* action = subActionAt(i);

				// completed action? Add total run time to both denominator and numerator.
				if(action->inFinalState()) {
					double runTimeSeconds = action->startDateTime().secsTo(action->endDateTime());
					totalDenominator += runTimeSeconds;
					totalNumerator += runTimeSeconds;
				}
				// incomplete action.
				else {
					double expectedDuration = action->info()->expectedDuration();
					totalDenominator += expectedDuration;
					if(i == currentSubActionIndex())	// if it's the current action, we have some partial progress.
						totalNumerator += numerator/denominator * expectedDuration;
				}
			}
			setProgress(totalNumerator, totalDenominator);
		}
		// Otherwise, assume every subaction makes up an equal fraction of the total amount of work. Our denominator will be the number of sub-actions, and our numerator will be the sub-action we are on (plus its fraction done). For ex: with 8 sub-actions: At the beginning, this will be 0/8.  Half-way through the first action it will be (0+0.5)/8.  At the very end it will be (7+1)/8.
		else {
			setProgress(currentSubActionIndex() + numerator/denominator, subActionCount());
		}
	}
	// parallel mode:
	else {
		// if they all have an expected duration, try to return total progress in secondsCompleted of secondsTotal.
		if(internalAllActionsHaveExpectedDuration()) {
			double totalNumerator = 0, totalDenominator = 0;
			for(int i=0, cc=subActionCount(); i<cc; i++) {
				AMAction* action = subActionAt(i);

				// completed action? Add total run time to both denominator and numerator.
				if(action->inFinalState()) {
					double runTimeSeconds = action->startDateTime().secsTo(action->endDateTime());
					totalDenominator += runTimeSeconds;
					totalNumerator += runTimeSeconds;
				}
				// incomplete action.
				else {
					double expectedDuration = action->info()->expectedDuration();
					totalDenominator += expectedDuration;
					totalNumerator += action->startDateTime().secsTo(action->endDateTime());
				}
			}
			setProgress(totalNumerator/subActionCount(), totalDenominator/subActionCount());
		}
		else {
			double totalNumerator = 0, totalDenominator = 0;
			foreach(AMAction* action, subActions_) {
				QPair<double, double> p = action->progress();
				// Will return (0,0) if unknown, or (-1,-1) if not yet started.
				if(p.second == 0) {
					setProgress(0,0);
					return;
				}
				if(p.first < 0 && p.second < 0) { // hasnt started yet
					totalDenominator += 1;
				}
				else {
					totalNumerator += p.first;
					totalDenominator += p.second;
				}
			}
			setProgress(totalNumerator, totalDenominator);
		}
	}
}

void AMListAction::internalOnSubActionStatusTextChanged(const QString &statusText)
{
	// sequential mode:
	if(subActionMode() == SequentialMode) {
		setStatusText("Step " % QString::number(currentSubActionIndex()+1) % " (" % currentSubAction()->info()->shortDescription() % "): " % statusText);
	}
	// parallel mode:
	else {
		QStringList allStatuses;
		for(int i=0, cc=subActionCount(); i<cc; i++)
			allStatuses << "Step " % QString::number(i+1) % " (" % subActionAt(i)->info()->shortDescription() % "): " % subActionAt(i)->statusText();

		setStatusText(allStatuses.join("\n"));
	}
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

bool AMListAction::internalAllActionsRunningOrFinal() const
{
	bool rv = true;
	foreach(AMAction* action, subActions_)
		rv &= (action->state() == Running || action->inFinalState());
	return rv;
}

bool AMListAction::internalAllActionsPausedOrFinal() const
{
	bool rv = true;
	foreach(AMAction* action, subActions_)
		rv &= (action->state() == Paused || action->inFinalState());
	return rv;
}

bool AMListAction::internalAllActionsInFinalState() const
{
	bool rv = true;
	foreach(AMAction* action, subActions_)
		rv &= action->inFinalState();
	return rv;
}

bool AMListAction::internalAnyActionsFailed() const
{
	foreach(AMAction* action, subActions_)
		if(action->state() == Failed)
			return true;
	return false;
}

bool AMListAction::internalAnyActionsCancelled() const
{
	foreach(AMAction* action, subActions_)
		if(action->state() == Cancelled)
			return true;
	return false;
}

bool AMListAction::internalAllActionsHaveExpectedDuration() const
{
	foreach(AMAction* action, subActions_)
		if(action->info()->expectedDuration() < 0)
			return false;
	return true;
}
