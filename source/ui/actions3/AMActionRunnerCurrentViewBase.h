#ifndef AMACTIONRUNNERCURRENTVIEWBASE_H
#define AMACTIONRUNNERCURRENTVIEWBASE_H

#include <QWidget>

#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QProgressBar>

#include "ui/actions3/AMCancelActionPrompt.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMLoopAction3.h"

/*!
	This class handles all the interactions required by the current view.  Since there are at least
	two current views (current view and bottom bar view), this will ensure that if any changes are
	required for the logic of how action interactions work they will only be done in one location.
	The subclasses should only have to worry about the look and feel and not the guts.
  */
class AMActionRunnerCurrentViewBase : public QWidget
{
	Q_OBJECT

public:
	/// The constructor.  This will create all the components.  Subclasses will only have to customize them and not worry about the memory.
	AMActionRunnerCurrentViewBase(AMActionRunner3 *actionRunner, QWidget *parent = 0);

	/// Sets whether the cancel prompt should be shown.
	void setCancelPromptVisibility(bool showPrompt) { showCancelPrompt_ = showPrompt; }
	/// Returns whether the cancel prompt will be shown.
	bool showCancelPrompt() const { return showCancelPrompt_; }

signals:

public slots:

protected slots:
	/// When the current action in the AMActionRunner changes
	virtual void onCurrentActionChanged(AMAction3 *action);

	/// When the pause button is clicked for the current action
	void onPauseButtonClicked();
	/// When the skip button is pressed, it handles telling the action to skip.
	void onSkipButtonClicked();
	/// Handles when the cancel button is clicked.  Prompts the user if this was actually what they wanted to do.
	void onCancelButtonClicked();

	/// slot to handle the action runner pausable changed signal
	void onActionRunnerPausableChanged(bool pausable);
	/// When the action's status text changes
	virtual void onStatusTextChanged(const QString& newStatus) = 0;
	/// When the action's state changes. We use this to change the pause button to "Resume" if the action is paused.
	virtual void onStateChanged(int state, int previousState);
	/// When the action's expected duration changes
	void onExpectedDurationChanged(double totalSeconds);
	/// When the action's progress changes
	void onProgressChanged(double numerator, double denominator);
	/// Handles updating whatever description labels child views require.
	virtual void updateDescriptionViews(AMAction3 *action) = 0;

	/// Called on a one-second timer to update the time elapsed, time remaining
	void onTimeUpdateTimer();

protected:
	/// Helper function to format an amount of seconds into an hour-minute-seconds string.
	QString formatSeconds(double seconds);

	/// The pause button.
	QToolButton *pauseButton_;
	/// The cancel button.
	QToolButton *cancelButton_;
	/// The skip button.
	QToolButton *skipButton_;
	/// The progress bar.
	QProgressBar *progressBar_;
	/// The time elapsed label.
	QLabel *timeElapsedLabel_;
	/// The time remaining label.
	QLabel *timeRemainingLabel_;

	/// The action runner model.
	AMActionRunner3 *actionRunner_;

	/// This string holds a small message that states what is currently running.  This is used when someone might be interested on what is "hanging up" the current action, or if they are just curious where they are.
	QString whatIsRunning_;
	/// The flag for determinining whether the cancel prompt should be displayed.
	bool showCancelPrompt_;
};

#endif // AMACTIONRUNNERCURRENTVIEWBASE_H
