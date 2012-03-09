#ifndef AMACTIONRUNNERQUEUEVIEW_H
#define AMACTIONRUNNERQUEUEVIEW_H

#include <QWidget>

class AMActionRunner;
class QTreeView;
class QToolButton;
class QLabel;
class QPushButton;

#include <QStyledItemDelegate>
/// This delegate is used by the tree view in AMActionRunnerQueueView to show custom editor widgets depending on the action. The available editors depend on those that have been registered with AMActionRegistry.  You should never need to use this class directly.
class AMActionRunnerQueueItemDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:

	explicit AMActionRunnerQueueItemDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

//    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual bool eventFilter(QObject *object, QEvent *event);

};

/// This UI class provides a view of the upcoming (queued) actions in an AMActionRunner. It is part of the overall AMWorkflowView.
class AMActionRunnerQueueView : public QWidget
{
    Q_OBJECT
public:
	/// Constructor. Should pass in the instance of AMActionRunner::s() for \c actionRunner.
	AMActionRunnerQueueView(AMActionRunner* actionRunner, QWidget *parent = 0);

	/// Returns true if the view is currently collapsed to show only the header bar, and false if it is fully shown.
	bool isCollapsed() const { return isCollapsed_; }

signals:
	/// This signal is emitted with \c true when the view widget is collapsed to show only the header bar, and \c false when it is restored.
	void collapsed(bool isCollapsed);

public slots:
	/// Collapses the view to show only the header bar.
	void collapse(bool isCollapsed);

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

	bool isCollapsed_;


	/// Helper function to determine if all the selected indexes are at the same level of the hierarchy. We need this to be true in order to enable the 'duplicate actions' button.  Then a user can either duplicate top-level actions, or duplicate actions within an AMNestedAction, but there's no ambiguity as to what we would do for a combination of both.
	bool allSelectedIndexesAtSameLevel(const QModelIndexList& selectedIndexes);
};


#endif // AMACTIONRUNNERQUEUEVIEW_H
