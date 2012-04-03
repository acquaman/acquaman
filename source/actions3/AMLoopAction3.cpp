#include "AMLoopAction3.h"
#include "actions3/AMActionLog3.h"

#include <QDebug>

AMLoopAction3::AMLoopAction3(AMLoopActionInfo3 *info, QObject *parent) : AMNestedAction3(info, parent) {
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	logSubActionsSeparately_ = true;
	currentSubAction_ = 0;
}

AMLoopAction3::AMLoopAction3(int iterations, QObject *parent) : AMNestedAction3(new AMLoopActionInfo3(iterations), parent) {
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	logSubActionsSeparately_ = true;
	currentSubAction_ = 0;
}

AMLoopAction3::AMLoopAction3(const AMLoopAction3 &other) : AMNestedAction3(other) {
	logSubActionsSeparately_ = other.logSubActionsSeparately_;
	// These are runtime, not configure, properties. This is supposed to be a new action:
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	currentSubAction_ = 0;

    foreach(AMAction3* action, other.subActions_)
		subActions_ << action->createCopy();
    foreach(AMAction3* action, subActions_)
        action->setParentAction(this);
}

AMLoopAction3::~AMLoopAction3() {
	qDeleteAll(subActions_);
}


void AMLoopAction3::insertSubActionImplementation(AMAction3 *action, int index)
{
	subActions_.insert(index, action);
}

AMAction3* AMLoopAction3::takeSubActionImplementation(int index)
{
	return subActions_.takeAt(index);
}

void AMLoopAction3::startImplementation()
{
	// done already with nothing to do.
	if(subActionCount() == 0 || loopCount() == 0) {
        setStarted();
        setSucceeded();
		return;
	}

    setStarted();
	setStatusText(QString("Loop %1 of %2").arg(currentIteration_+1).arg(loopCount()));
	internalDoNextAction();
}

void AMLoopAction3::cancelImplementation()
{
	// haven't started running anything... Easy to cancel.
	if(currentSubActionIndex_ < 0) {
        setCancelled();
		return;
	}

	// tell the current action to cancel
	currentSubAction_->cancel();
	// when we receive the cancelled signal, we'll notifyCancelled.
}

void AMLoopAction3::internalOnCurrentActionStateChanged(int newState, int oldState)
{
    if(newState == AMAction3::Paused) {
        if(state() == AMAction3::Pausing) {
            setPaused();
		}
		else {
			qWarning() << "AMLoopAction: Warning: One of our sub-actions was paused, but not by us.";
		}
	}
    else if(newState == AMAction3::Running && oldState == AMAction3::Resuming) {
        if(state() == AMAction3::Resuming) {
            setResumed();
		}
		else {
			qWarning() << "AMLoopAction: Warning: One of our sub-actions was resumed, but not by us.";
		}
	}
    else if(newState == AMAction3::Cancelled) {
        if(state() == AMAction3::Cancelling) {
			disconnect(currentSubAction_, 0, this, 0);
			if(internalShouldLogSubAction(currentSubAction_))
                AMActionLog3::logCompletedAction(currentSubAction_);
			currentSubAction_->deleteLater();
            setCancelled();
		}
		else {
			qWarning() << "AMLoopAction: Warning: One of our sub-actions was cancelled, but not by us. This will cause this action to fail.";
			disconnect(currentSubAction_, 0, this, 0);
			if(internalShouldLogSubAction(currentSubAction_))
                AMActionLog3::logCompletedAction(currentSubAction_);
			currentSubAction_->deleteLater();
            setFailed();
		}
	}
    else if(newState == AMAction3::Failed) {
		disconnect(currentSubAction_, 0, this, 0);
		if(internalShouldLogSubAction(currentSubAction_))
            AMActionLog3::logCompletedAction(currentSubAction_);
		currentSubAction_->deleteLater();
        setFailed();
	}
    else if(newState == AMAction3::Succeeded) {
		internalDoNextAction();
	}
}

void AMLoopAction3::internalDoNextAction()
{
	// did an action just finish completing?
	if(currentSubActionIndex_ >= 0) {
		disconnect(currentSubAction_, 0, this, 0);
		if(internalShouldLogSubAction(currentSubAction_))
            AMActionLog3::logCompletedAction(currentSubAction_);
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
            setSucceeded();
		}
	}
}

void AMLoopAction3::internalOnCurrentActionProgressChanged(double numerator, double denominator)
{
	bool allActionsHaveExpectedDuration = true;
    foreach(AMAction3* action, subActions_) {
		if(action->info()->expectedDuration() < 0) {
			allActionsHaveExpectedDuration = false;
			break;
		}
	}


	if(allActionsHaveExpectedDuration) {
		double totalNumerator = 0, totalDenominator = 0;
		for(int i=0, cc=subActionCount(); i<cc; i++) {
            AMAction3* action = subActionAt(i);

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

void AMLoopAction3::internalOnCurrentActionStatusTextChanged(const QString &statusText)
{
	setStatusText(QString("Loop %1 of %2 (%3: %4)").arg(currentIteration_+1).arg(loopCount()).arg(currentSubAction_->info()->shortDescription()).arg(statusText));
}

bool AMLoopAction3::internalShouldLogSubAction(AMAction3 *action)
{
    AMNestedAction3* nestedAction = qobject_cast<AMNestedAction3*>(action);
	return shouldLogSubActionsSeparately() && !(nestedAction && nestedAction->shouldLogSubActionsSeparately());
}
