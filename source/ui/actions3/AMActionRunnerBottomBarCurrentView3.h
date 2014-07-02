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

#include <QWidget>

#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QProgressBar>

#include "ui/actions3/AMCancelActionPrompt.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMLoopAction3.h"

/// This class is meant to be identical to the AMActionRunnerCurrentView widget in the workflow, but fit nicely on the bottom bar.
class AMActionRunnerBottomBarCurrentView3 : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  For now, include AMActionRunner::s() for \c actionRunner.
 	virtual ~AMActionRunnerBottomBarCurrentView3();
	AMActionRunnerBottomBarCurrentView3(AMActionRunner3 *actionRunner, QWidget *parent = 0);

	/// Sets whether the cancel prompt should be shown.
	void setCancelPromptVisibility(bool showPrompt) { showCancelPrompt_ = showPrompt; }
	/// Returns whether the cancel prompt will be shown.
	bool showCancelPrompt() const { return showCancelPrompt_; }

protected slots:
	/// When the current action in the AMActionRunner changes.
	void onCurrentActionChanged(AMAction3 *action);

	/// When the pause button is clicked for the current action.
	void onPauseButtonClicked();
	/// When the skip button is pressed, it handles telling the action to skip.
	void onSkipButtonClicked();
	/// Handles when the cancel button is clicked.  Prompts the user if this was actually what they wanted to do.
	void onCancelButtonClicked();

	// Signals from the current action, forwarded through the Action Runner.
	/// When the action's status text changes.
	void onStatusTextChanged();
	/// When the actions expected duration changes.
	void onExpectedDurationChanged(double totalSeconds);
	/// When the action's progress changes.
	void onProgressChanged(double numerator, double denominator);
	/// When the action's state changes.  We use this to change teh pause button to "Resume" if the action is paused.
	void onStateChanged(int state, int previousState);

	/// Called on a one-second timer to update the time elapsed, time remaining.
	void onTimeUpdateTimer();

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
	/// Helper function to format an amount of seconds into an hour-minute-seconds string.
	QString formatSeconds(double seconds);

	/// The pause button.
	QToolButton *pauseButton_;
	/// The cancel button.
	QToolButton *cancelButton_;
	/// The skip button.
	QToolButton *skipButton_;
	/// The button to increase the number of iterations of a master loop action.
	QToolButton *increaseIterations_;
	/// The button to decrease the number of iterations of a master loop action.
	QToolButton *decreaseIterations_;
	/// The progress bar.
	QProgressBar *progressBar_;
	/// The time elapsed label.
	QLabel *timeElapsedLabel_;
	/// The time remaining label.
	QLabel *timeRemainingLabel_;
	/// The description label.
	QLabel *descriptionLabel_;

	/// This string holds a small message that states what is currently running.  This is used when someone might be interested on what is "hanging up" the current action, or if they are just curious where they are.
	QString whatIsRunning_;

	/// The action runner model.
	AMActionRunner3 *actionRunner_;
	/// Pointer to the root loop action (if it exists).
	AMLoopAction3 *rootLoopAction_;

	/// The flag for determinining whether the cancel prompt should be displayed.
	bool showCancelPrompt_;
};

#endif // AMACTIONRUNNERBOTOMBARCURRENTVIEW3_H
