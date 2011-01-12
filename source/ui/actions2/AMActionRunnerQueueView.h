#ifndef AMACTIONRUNNERQUEUEVIEW_H
#define AMACTIONRUNNERQUEUEVIEW_H

#include <QWidget>

class AMActionRunner;
class QTreeView;
class QToolButton;
class QLabel;
class QPushButton;

/// This UI class provides a view of the upcoming (queued) actions in an AMActionRunner. It is part of a the overall AMWorkflowView.
class AMActionRunnerQueueView : public QWidget
{
    Q_OBJECT
public:
	/// Constructor. Should pass in the instance of AMActionRunner::s() for \c actionRunner.
	AMActionRunnerQueueView(AMActionRunner* actionRunner, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the "Pause Queue" button is clicked or un-clicked
	void onPauseButtonClicked(bool isPaused);
	/// Called when actions are added or removed from the queue. We use this to update our subtitle text.
	void onQueueActionAddedOrRemoved();
	/// Called when the delete button is pushed to delete an action from the queue
	void onDeleteButtonClicked();
	/// Called when the duplicate button is pushed to duplicate one or more actions in the queue
	void onDuplicateButtonClicked();

	/// Called when the selected queue actions change. We use it to enable or disable the delete and duplicate buttons.
	void onSelectionChanged();
	/// Called when the queue notifies us that it's been paused or unpaused. We use it to set the state of the pause button
	void onQueuePaused(bool isPaused);



protected:
	AMActionRunner* actionRunner_;
	QTreeView* treeView_;
	QLabel* headerTitle_, *headerSubTitle_;

	QToolButton* hideButton_;
	QPushButton* pauseButton_;

	QPushButton* deleteButton_, *duplicateButton_;
};

///// This class wraps an AMActionRunner into a model that works with the Qt Model/View system. Nominally, it's usually a list model, but it uses the full tree model API so that actions inside AMLoopAction appear hierarchically.  There is only ever one column.
//class AMActionRunnerQueueModel : public QAbstractItemModel {
//	Q_OBJECT
//public:
//	/// Constructor.
//	AMActionRunnerQueueModel(AMActionRunner* actionRunner, QObject* parent = 0);

//	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

//	QModelIndex parent(const QModelIndex &child) const;

//	int rowCount(const QModelIndex &parent = QModelIndex()) const;

//	int columnCount(const QModelIndex &parent = QModelIndex()) const;

//	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

//	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

//	AMAction* actionAtIndex(const QModelIndex& index);


//protected:
//	AMActionRunner* actionRunner_;
//};

#endif // AMACTIONRUNNERQUEUEVIEW_H
