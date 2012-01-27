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

#include <QStandardItem>

/// This QAbstractItemModel wraps the current action of AMActionRunner, and is used by AMActionRunnerCurrentView. It is hierarchical to display nested actions as they run.  You should never need to use this class directly.
class AMActionRunnerCurrentModel : public QAbstractItemModel {
	Q_OBJECT
public:
	/// Constructor.
	AMActionRunnerCurrentModel(AMActionRunner* actionRunner, QObject* parent = 0);

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	QModelIndex parent(const QModelIndex &child) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

	AMAction* actionAtIndex(const QModelIndex& index) const;
	// requires a linear search of the \c action's parentAction(), or of the actionRunner queue if the action is at the top level.
	QModelIndex indexForAction(AMAction* action) const;

protected slots:
	void onCurrentActionChanged(AMAction* currentAction);

protected:
	AMActionRunner* actionRunner_;
	AMAction* currentAction_;
};

///// This subclass of QStandardItem is used inside AMActionRunnerCurrentView's QTreeView to display actions.  You should never need to use this class directly.
//class AMActionQueueModelItem : public QStandardItem {
//public:
//	/// Used to identify a specific type of item subclass
//	enum Type { AMActionQueueModelItemType = QStandardItem::UserType + 1049 };
//	/// Construct a model item for the given \c action.
//	AMActionQueueModelItem(AMAction* action);
//	/// The two things that separate us from QStandardItems: we have an associated action, and we return data based on that action
//	AMAction* action() { return action_; }
//	/// Action-specific data: we return the action's info's shortDescription() for Qt::DisplayRole, and cache and return pixmaps based on the info's iconFileName() for Qt::DecorationRole
//	virtual QVariant data(int role) const;
//	/// Used to identify a specific type of item subclass
//	virtual int type() const { return AMActionQueueModelItemType; }
//protected:
//	/// Pointer to our associated action
//	AMAction* action_;
//};

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
//	QStandardItemModel* currentActionModel_;

	QPushButton* cancelButton_, *pauseButton_;
	QProgressBar* progressBar_;
	QLabel* timeElapsedLabel_, * timeRemainingLabel_, *headerTitle_, *headerSubTitle_;

	AMActionRunner* actionRunner_;


};

#endif // AMACTIONRUNNERCURRENTVIEW_H
