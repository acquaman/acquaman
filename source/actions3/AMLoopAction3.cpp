#include "AMLoopAction3.h"
#include "actions3/AMActionLog3.h"

#include <QDebug>

AMLoopAction3::AMLoopAction3(AMLoopActionInfo3 *info, QObject *parent) : AMListAction3(info, AMListAction3::Sequential, parent) {
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	logSubActionsSeparately_ = true;
	currentSubAction_ = 0;
}

AMLoopAction3::AMLoopAction3(int iterations, QObject *parent) : AMListAction3(new AMLoopActionInfo3(iterations), AMListAction3::Sequential, parent) {
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	logSubActionsSeparately_ = true;
	currentSubAction_ = 0;
}

AMLoopAction3::AMLoopAction3(const AMLoopAction3 &other) : AMListAction3(other) {
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
}

bool AMLoopAction3::duplicateSubActions(const QList<int> &indexesToCopy)
{
	if(state() != Constructed)
		return false;

	if(indexesToCopy.isEmpty())
		return true;	// done

	// sort the list, so we know the highest index in it.
	QList<int> sIndexesToCopy = indexesToCopy;
	qSort(sIndexesToCopy);

	// any indexes out of range?
	if(sIndexesToCopy.first() < 0)
		return false;
	if(sIndexesToCopy.last() >= subActionCount())
		return false;

	// insert at the position after last existing subAction to copy
	int insertionIndex = sIndexesToCopy.last() + 1;

	// insert copies of all, using regular insertSubAction().
	foreach(int i, sIndexesToCopy)
		insertSubAction(subActionAt(i)->createCopy(), insertionIndex++);

	return true;
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

void AMLoopAction3::internalCleanupAction(AMAction3 *action)
{
	AMAction3 *cleanupAction = action ? action : currentSubAction_;

	internalDisconnectAction(cleanupAction);
	if(internalShouldLogSubAction(cleanupAction)){
		int parentLogId = logActionId();
		AMActionLog3::logCompletedAction(cleanupAction, parentLogId);
	}
	// delete it later (since we might still be executing inside the action's functions).
	cleanupAction->deleteLater();

	if (!action)
		currentSubAction_ = 0;
}

void AMLoopAction3::internalDoNextAction()
{
	// did an action just finish completing?
	if(currentSubActionIndex_ >= 0) {

		internalDisconnectAction(currentSubAction_);
		if(internalShouldLogSubAction(currentSubAction_)){
			int parentLogId = logActionId();
			AMActionLog3::logCompletedAction(currentSubAction_, parentLogId);
		}
		// delete it later (since we might still be executing inside the action's functions).
		currentSubAction_->deleteLater();
	}

	// do we have a next action in this loop? [This will also trigger the start of the very first action]
	if(currentSubActionIndex_ < subActionCount()-1) {
		emit currentSubActionChanged(++currentSubActionIndex_);

		currentSubAction_ = subActions_.at(currentSubActionIndex_)->createCopy();
		internalConnectAction(currentSubAction_);
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
			internalConnectAction(currentSubAction_);
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
	if(internalAllActionsHaveExpectedDuration()) {
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
