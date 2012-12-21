#ifndef AMACTIONRUNNERBOTTOMBARCURRENTVIEW3_H
#define AMACTIONRUNNERBOTTOMBARCURRENTVIEW3_H

#include <QWidget>

#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QProgressBar>

#include "actions3/AMActionRunner3.h"
#include "actions3/AMLoopAction3.h"

/// This class is meant to be identical to the AMActionRunnerCurrentView widget in the workflow, but fit nicely on the bottom bar.
class AMActionRunnerBottomBarCurrentView3 : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  For now, include AMActionRunner::s() for \c actionRunner.
	AMActionRunnerBottomBarCurrentView3(AMActionRunner3 *actionRunner, QWidget *parent = 0);

protected slots:
	/// When the current action in the AMActionRunner changes.
	void onCurrentActionChanged(AMAction3 *action);

	/// When the pause butto is clicked for the current action.
	void onPauseButtonClicked();
	/// When the skip button is pressed, it handles telling the action to skip.
	void onSkipButtonClicked();

	// Signals from the current action, forwarded through the Action Runner.
	/// When the action's status text changes.
	void onStatusTextChanged(const QString &newStatus);
	/// When the actions expected duration changes.
	void onExpectedDurationChanged(double totalSeconds);
	/// When the action's progress changes.
	void onProgressChanged(double numerator, double denominator);
	/// When the action's state changes.  We use this to change teh pause button to "Resume" if the action is paused.
	void onStateChanged(int state, int previousState);

	/// Called on a one-second timer to update the time elapsed, time remaining.
	void onTimeUpdateTimer();

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
};

#endif // AMACTIONRUNNERBOTOMBARCURRENTVIEW3_H
