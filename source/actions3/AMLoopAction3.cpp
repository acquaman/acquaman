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


#include "AMLoopAction3.h"
#include "actions3/AMActionLog3.h"
#include "acquaman/AMAgnosticDataAPI.h"

#include <QStringBuilder>

AMLoopAction3::AMLoopAction3(AMLoopActionInfo3 *info, QObject *parent)
	: AMListAction3(info, AMListAction3::Sequential, parent)
{
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	logSubActionsSeparately_ = true;
	currentSubAction_ = 0;
	skipAfterCurrentIteration_ = false;
	skipOptions_.append("After current iteration");
}

AMLoopAction3::AMLoopAction3(int iterations, QObject *parent)
	: AMListAction3(new AMLoopActionInfo3(iterations), AMListAction3::Sequential, parent)
{
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	logSubActionsSeparately_ = true;
	currentSubAction_ = 0;
	skipAfterCurrentIteration_ = false;
	skipOptions_.append("After current iteration");
}

AMLoopAction3::AMLoopAction3(const AMLoopAction3 &other)
	: AMListAction3(other)
{
	logSubActionsSeparately_ = other.logSubActionsSeparately_;
	// These are runtime, not configure, properties. This is supposed to be a new action:
	currentIteration_ = 0;
	currentSubActionIndex_ = -1;
	currentSubAction_ = 0;
	skipAfterCurrentIteration_ = false;
	skipOptions_.append("After current iteration");

	// AMListAction already handles copying the actions, so we just need to make sure the parents are set properly.
	foreach(AMAction3* action, subActions_)
		action->setParentAction(this);
}

AMLoopAction3::~AMLoopAction3() {
}

bool AMLoopAction3::canPause() const{
	if(currentIteration_ < loopCount()-1)
		return true;
	return AMListAction3::canPause();
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

void AMLoopAction3::skipImplementation(const QString &command)
{
	if (command == "After current action"){

		skipAfterCurrentAction_ = true;
		setStatusText("Skipping - After current action");
	}

	else if (command == "After current iteration"){

		skipAfterCurrentIteration_ = true;
		setStatusText("Skipping - After current iteration");
	}
}

void AMLoopAction3::internalCleanupAction(AMAction3 *action)
{
	AMAction3 *cleanupAction = action ? action : currentSubAction_;

	internalDisconnectAction(cleanupAction);
	// delete it later (since we might still be executing inside the action's functions).
	cleanupAction->scheduleForDeletion();

	if (!action)
		currentSubAction_ = 0;
}

void AMLoopAction3::internalDoNextAction()
{
	// did an action just finish completing?
	if(currentSubActionIndex_ >= 0) {

		internalDisconnectAction(currentSubAction_);
		// delete it later (since we might still be executing inside the action's functions).
		currentSubAction_->scheduleForDeletion();
	}

	// Check if we are stopping now.
	if (skipAfterCurrentAction_)
		setSkipped();

	// do we have a next action in this loop? [This will also trigger the start of the very first action]
	else if(currentSubActionIndex_ < subActionCount()-1) {
		emit currentSubActionChanged(++currentSubActionIndex_);

		currentSubAction_ = subActions_.at(currentSubActionIndex_)->createCopy();
		internalConnectAction(currentSubAction_);

		if(state() == AMAction3::Pausing)
			setPaused();
		else if(state() == AMAction3::Running)
			currentSubAction_->start();
	}
	else {
		// done this loop.
		emit currentIterationChanged(++currentIteration_);

		// Are we stopping now that we are at the end of this iteration?
		if (skipAfterCurrentIteration_)
			setSkipped();

		// Is there a next one?
		else if(currentIteration_ < loopCount()) {
			setStatusText(QString("Loop %1 of %2.").arg(currentIteration_+1).arg(loopCount()));
			emit currentSubActionChanged(currentSubActionIndex_ = 0);

			currentSubAction_ = subActions_.at(currentSubActionIndex_)->createCopy();
			internalConnectAction(currentSubAction_);

			if(state() == AMAction3::Pausing)
				setPaused();
			else if(state() == AMAction3::Running)
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

void AMLoopAction3::internalOnSubActionStatusTextChanged(const QString &statusText)
{
	setStatusText("Step " % QString::number(currentSubActionIndex()+1) % " of loop " % QString::number(currentIteration()+1) % "\n(" % currentSubAction()->info()->shortDescription() % "): " % statusText);
}
