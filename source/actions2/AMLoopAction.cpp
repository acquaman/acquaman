#include "AMLoopAction.h"
#include "actions2/AMActionLog.h"

#include <QDebug>

AMLoopAction::AMLoopAction(AMLoopActionInfo *info, QObject *parent) : AMNestedAction(info, parent) {
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	logSubActionsSeparately_ = true;
	currentSubAction_ = 0;
}


AMLoopAction::AMLoopAction(const AMLoopAction &other) : AMNestedAction(other) {
	logSubActionsSeparately_ = other.logSubActionsSeparately_;
	// These are runtime, not configure, properties. This is supposed to be a new action:
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	currentSubAction_ = 0;

	foreach(AMAction* action, other.subActions_)
		subActions_ << action->createCopy();
	foreach(AMAction* action, subActions_)
		action->internalSetParentAction(this);
}

AMLoopAction::~AMLoopAction() {
	qDeleteAll(subActions_);
}


void AMLoopAction::insertSubActionImplementation(AMAction *action, int index)
{
	subActions_.insert(index, action);
}

AMAction* AMLoopAction::takeSubActionImplementation(int index)
{
	return subActions_.takeAt(index);
}

void AMLoopAction::startImplementation()
{
	// done already with nothing to do.
	if(subActionCount() == 0) {
		notifyStarted();
		notifySucceeded();
		return;
	}

	notifyStarted();
	setStatusText(QString("Loop %1 of %2").arg(currentIteration_+1).arg(loopCount()));
	internalDoNextAction();
}

void AMLoopAction::cancelImplementation()
{
	// haven't started running anything... Easy to cancel.
	if(currentSubActionIndex_ < 0) {
		notifyCancelled();
		return;
	}

	// tell the current action to cancel
	currentSubAction_->cancel();
	// when we receive the cancelled signal, we'll notifyCancelled.
}

void AMLoopAction::internalOnCurrentActionStateChanged(int newState, int oldState)
{
	if(newState == AMAction::Paused) {
		if(state() == AMAction::Pausing) {
			notifyPaused();
		}
		else {
			qWarning() << "AMLoopAction: Warning: One of our sub-actions was paused, but not by us.";
		}
	}
	else if(newState == AMAction::Running && oldState == AMAction::Resuming) {
		if(state() == AMAction::Resuming) {
			notifyResumed();
		}
		else {
			qWarning() << "AMLoopAction: Warning: One of our sub-actions was resumed, but not by us.";
		}
	}
	else if(newState == AMAction::Cancelled) {
		if(state() == AMAction::Cancelling) {
			disconnect(currentSubAction_, 0, this, 0);
			if(internalShouldLogSubAction(currentSubAction_))
				AMActionLog::logCompletedAction(currentSubAction_);
			currentSubAction_->deleteLater();
			notifyCancelled();
		}
		else {
			qWarning() << "AMLoopAction: Warning: One of our sub-actions was cancelled, but not by us. This will cause this action to fail.";
			disconnect(currentSubAction_, 0, this, 0);
			if(internalShouldLogSubAction(currentSubAction_))
				AMActionLog::logCompletedAction(currentSubAction_);
			currentSubAction_->deleteLater();
			notifyFailed();
		}
	}
	else if(newState == AMAction::Failed) {
		disconnect(currentSubAction_, 0, this, 0);
		if(internalShouldLogSubAction(currentSubAction_))
			AMActionLog::logCompletedAction(currentSubAction_);
		currentSubAction_->deleteLater();
		notifyFailed();
	}
	else if(newState == AMAction::Succeeded) {
		internalDoNextAction();
	}
}

void AMLoopAction::internalDoNextAction()
{
	// did an action just finish completing?
	if(currentSubActionIndex_ >= 0) {
		disconnect(currentSubAction_, 0, this, 0);
		if(internalShouldLogSubAction(currentSubAction_))
			AMActionLog::logCompletedAction(currentSubAction_);
		// delete it later (since we might still be executing inside the action's functions).
		currentSubAction_->deleteLater();
	}

	// do we have a next action in this loop? [This will also trigger the start of the very first action]
	if(currentSubActionIndex_ < subActionCount()-1) {
		emit currentSubActionChanged(++currentSubActionIndex_);

		currentSubAction_ = subActions_.at(currentSubActionIndex_)->createCopy();
		connect(currentSubAction_, SIGNAL(stateChanged(int,int)), this, SLOT(internalOnCurrentActionStateChanged(int,int)));
		connect(currentSubAction_, SIGNAL(progressChanged(double,double)), this, SLOT(internalOnCurrentActionProgressChanged(double,double)));
		connect(currentSubAction_, SIGNAL(statusTextChanged(QString)), this, SLOT(internalOnCurrentActionStatusTextChanged(QString)));
		currentSubAction_->start();
	}
	else {
		// done this loop.
		emit currentIterationChanged(++currentIteration_);
		// Is there a next one?
		if(currentIteration_ < loopCount()) {
			setStatusText(QString("Loop %1 of %2.").arg(currentIteration_+1).arg(loopCount()));
			emit currentSubActionChanged(currentSubActionIndex_ = 0);

			currentSubAction_ = subActions_.at(currentSubActionIndex_)->createCopy();
			connect(currentSubAction_, SIGNAL(stateChanged(int,int)), this, SLOT(internalOnCurrentActionStateChanged(int,int)));
			connect(currentSubAction_, SIGNAL(progressChanged(double,double)), this, SLOT(internalOnCurrentActionProgressChanged(double,double)));
			connect(currentSubAction_, SIGNAL(statusTextChanged(QString)), this, SLOT(internalOnCurrentActionStatusTextChanged(QString)));
			currentSubAction_->start();
		}
		// Nope, that's the end.
		else {
			notifySucceeded();
		}
	}
}

void AMLoopAction::internalOnCurrentActionProgressChanged(double numerator, double denominator)
{
	bool allActionsHaveExpectedDuration = true;
	foreach(AMAction* action, subActions_) {
		if(action->info()->expectedDuration() < 0) {
			allActionsHaveExpectedDuration = false;
			break;
		}
	}


	if(allActionsHaveExpectedDuration) {
		double totalNumerator = 0, totalDenominator = 0;
		for(int i=0, cc=subActionCount(); i<cc; i++) {
			AMAction* action = subActionAt(i);

			double expectedSecs = action->info()->expectedDuration();
			totalDenominator += expectedSecs*loopCount();
			int timesFinished = currentIteration_;
			if(i < currentSubActionIndex_)
				timesFinished++;
			totalNumerator += expectedSecs*timesFinished;

			if(i == currentSubActionIndex())	// if it's the current action, we have some partial progress.
				totalNumerator += numerator/denominator * expectedSecs;
		}
		setProgress(totalNumerator, totalDenominator);
		setExpectedDuration(totalDenominator);
	}
	// Otherwise, assume every subaction makes up an equal unit ('1') of the total amount of work. Our denominator will be the number of sub-actions * the number of loops, and our numerator will be the sub-actions we've completed (plus the current sub-action's fraction done).
	else {
		double totalDenominator = subActionCount()*loopCount();
		double totalNumerator = subActionCount()*currentIteration_;	// add the completed full loops
		totalNumerator += currentSubActionIndex_;	// add the actions done in this loop
		totalNumerator += numerator/denominator;	// add the fraction done for the current action.
		setProgress(totalNumerator, totalDenominator);
		setExpectedDuration(runningTime()*totalDenominator/totalNumerator);
	}
}

void AMLoopAction::internalOnCurrentActionStatusTextChanged(const QString &statusText)
{
	setStatusText(QString("Loop %1 of %2 (%3: %4)").arg(currentIteration_+1).arg(loopCount()).arg(currentSubAction_->info()->shortDescription()).arg(statusText));
}

bool AMLoopAction::internalShouldLogSubAction(AMAction *action)
{
	AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(action);
	return shouldLogSubActionsSeparately() && !(nestedAction && nestedAction->shouldLogSubActionsSeparately());
}
