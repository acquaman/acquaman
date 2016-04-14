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


#ifndef AMACTIONRUNNERBOTTOMBARCURRENTVIEW3_H
#define AMACTIONRUNNERBOTTOMBARCURRENTVIEW3_H

#include "ui/actions3/AMActionRunnerCurrentViewBase.h"

/// This class is meant to be identical to the AMActionRunnerCurrentView widget in the workflow, but fit nicely on the bottom bar.
class AMActionRunnerBottomBarCurrentView3 : public AMActionRunnerCurrentViewBase
{
	Q_OBJECT

public:
	/// Constructor.  For now, include AMActionRunner::s() for \c actionRunner.
	AMActionRunnerBottomBarCurrentView3(AMActionRunner3 *actionRunner, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMActionRunnerBottomBarCurrentView3();

protected slots:
	/// When the current action in the AMActionRunner changes
	virtual void onCurrentActionChanged(AMAction3 *action);
	/// When the action's state changes. We use this to change the pause button to "Resume" if the action is paused.
	virtual void onStateChanged(int state, int previousState);
	/// When the action's status text changes
	virtual void onStatusTextChanged(const QString& newStatus);
	/// Handles updating whatever description labels child views require.
	virtual void updateDescriptionViews(AMAction3 *action);

	/// Slot that handles when the root loop action current sub action changes.  The new index of the sub action is provided.
	void onRootLoopSubActionChanged(int index);
	/// Slot that handles updating the description label when the loop count is updated.
	void onRootLoopActionLoopCountChanged();
	/// Slot that handles updating the view when the iteration of the root loop action updates.
	void onLoopIterationUpdate(int iteration);
	/// Slot that handles when the increase iterations button is clicked.
	void onIncreaseLoopIterationsClicked();
	/// Slot that handles when the decrease iterations button is clicked.
	void onDecreaseLoopIterationsClicked();

protected:
	/// The button to increase the number of iterations of a master loop action.
	QToolButton *increaseIterations_;
	/// The button to decrease the number of iterations of a master loop action.
	QToolButton *decreaseIterations_;
	/// The description label.
	QLabel *descriptionLabel_;

	/// This string holds a small message that states what is currently running.  This is used when someone might be interested on what is "hanging up" the current action, or if they are just curious where they are.
	QString whatIsRunning_;

	/// Pointer to the root loop action (if it exists).
	AMLoopAction3 *rootLoopAction_;
};

#endif // AMACTIONRUNNERBOTOMBARCURRENTVIEW3_H
