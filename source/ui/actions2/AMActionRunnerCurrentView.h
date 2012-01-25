#ifndef AMACTIONRUNNERCURRENTVIEW_H
#define AMACTIONRUNNERCURRENTVIEW_H

#include <QWidget>

class AMActionRunner;
class AMAction;
class QTreeView;
class QToolButton;
class QLabel;
class QPushButton;
class QStandardItemModel;
class QProgressBar;

/// This class provides a view of the currently-running action in the AMActionRunner. It is part of the overall AMWorkflowView.
class AMActionRunnerCurrentView : public QWidget
{
    Q_OBJECT
public:
	/// Constructor. Should pass in AMActionRunner::s() for \c actionRunner
	AMActionRunnerCurrentView(AMActionRunner* actionRunner, QWidget *parent = 0);


signals:

public slots:

protected slots:
	/// When the current action in the AMActionRunner changes
	void onCurrentActionChanged(AMAction*);

	/// When the pause button is clicked for the current action
	void onPauseButtonClicked();

	// Signals from the current action, forwarded through the Action Runner
	/// When the action's status text changes
	void onStatusTextChanged(const QString& newStatus);
	/// When the action's expected duration changes
	void onExpectedDurationChanged(double totalSeconds);
	/// When the action's progress changes
	void onProgressChanged(double numerator, double denominator);
	/// When the action's state changes. We use this to change the pause button to "Resume" if the action is paused.
	void onStateChanged(int state, int previousState);

	/// Called on a one-second timer to update the time elapsed, time remaining
	void onTimeUpdateTimer();

protected:

	/// Helper function to format an amount of seconds into an hour-minute-seconds string.
	QString formatSeconds(double seconds);

	QTreeView* currentActionView_;
	QStandardItemModel* currentActionModel_;

	QPushButton* cancelButton_, *pauseButton_;
	QProgressBar* progressBar_;
	QLabel* timeElapsedLabel_, * timeRemainingLabel_, *headerTitle_, *headerSubTitle_;

	AMActionRunner* actionRunner_;


};

#endif // AMACTIONRUNNERCURRENTVIEW_H
