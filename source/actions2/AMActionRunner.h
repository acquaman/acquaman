#ifndef AMACTIONRUNNER_H
#define AMACTIONRUNNER_H

#include <QObject>
#include <QList>
#include <QStandardItemModel>
#include <QStandardItem>

class AMAction;
class AMActionRunnerQueueModel;

///// This subclass of QStandardItem is used inside AMActionQueueModel to represent actions in the AMActionRunner queue.  You should never need to use this class directly.
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


///// This subclass of QStandardItemModel is used internally by AMActionRunner for its queue of upcoming actions, and to handle drag-and-drop interaction with AMActionRunnerQueueView. You should never need to use this class directly; instead, programmers should use the AMActionRunner API to manage queued actions.
///*!
//Implementation Notes

//- We use a possibly hierarchical model so that we can support nesting of "For-loop"-type actions (anything that implements AMNestedAction).
//- Subclassing QStandardItemModel is only necessary because we want to enable drag-and-drop reordering of actions (AMActionQueueModelItems) in the AMActionRunner; otherwise we could use a normal QStandardItemModel.
//- IMPORANT: There is not two-way data flow/synchronization between this model and AMActionRunner.  Instead, the AMActionRunner must be the only object to send modification commands to this model.  For example, it is NOT supported to remove items from the AMActionRunner queue by calling removeRows() on this model.
//*/
//class AMActionQueueModel : public QStandardItemModel {
//public:
//	/// Constructor. Requires a pointer back to the AMActionRunner.
//	AMActionQueueModel(AMActionRunner* actionRunner, QObject* parent = 0);

//	/// Re-implemented from QStandardItemModel to deal with dropping when re-ordering the queue via drag-and-drop.
//	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
//	/// Re-implemented from QStandardItemModel to deal with dragging when re-ordering the queue via drag-and-drop.
//	virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
//	/// Re-implemented from QStandardItemModel to deal with dragging when re-ordering the queue via drag-and-drop.
//	virtual QStringList mimeTypes() const;
//	/// Re-implemented from QStandardItemModel to deal with dropping when re-ordering the queue via drag-and-drop
//	virtual Qt::DropActions supportedDropActions() const { return Qt::MoveAction; }

//	/// Re-implemented only for debugging purposes (temporary). Same behaviour as QStandardItemView.
//	bool removeRows(int row, int count, const QModelIndex &parent);

//protected:
//	/// Pointer back to our action runner, that we use to request re-ordering during drag-and-drop
//	AMActionRunner* actionRunner_;

//	// traverses the tree (depth first) and appends all the action short descriptions to outstring. Uses the model indexes for structure
//	void traverse1(const QModelIndex& parent, QString& outstring, int level = 0);
//	// traverses the tree (depth first) and appends all the action short descriptions to outstring. Uses the AMActionRunner/AMNestedAction apis for structure.
//	void traverse2(const AMNestedAction* parent, QString& outstring, int level = 0);

//	void checkTreeConsistency();

//	QString nSpaces(int n);
//};


/// This singleton class provides the API for Acquaman's workflow manager. You can queue up actions for it to run, manage the queue of upcoming actions, and receive notifications when actions are completed. After an action is completed, it will automatically log the actions with AMActionLog::logCompletedAction().  The AMActionRunnerCurrentView and AMActionRunnerQueueView provide graphical user interfaces to this API.
/*! Note that the AMActionRunner's queue is initially paused when it is first created. To have it start executing actions as soon as they become available, call AMActionRunner::s()->setPaused(false).*/
class AMActionRunner : public QObject
{
    Q_OBJECT
public:
	/// This is a singleton class. You access the only instance of it using AMActionRunner::s().
	static AMActionRunner* s();
	/// Release and delete the singleton instance
	static void releaseActionRunner();

	// The Queue of Upcoming Actions:  (These all refer to top-level actions. To manage ordering and moving sub-actions within nested actions like loop actions, programmers should use the AMNestedAction interface.)
	///////////////////////////////////

	/// The number of actions that are in the queue. (Does not include the possible currently-running action.)
	int queuedActionCount() const { return queuedActions_.count(); }
	/// Returns a pointer to an action in the queue, or 0 if the index is out of range.
	const AMAction* queuedActionAt(int index) const { if(index < 0 || index >= queuedActions_.count()) return 0; return queuedActions_.at(index); }
	/// Returns a pointer to an action in the queue, or 0 if the index is out of range.
	AMAction* queuedActionAt(int index) { if(index < 0 || index >= queuedActions_.count()) return 0; return queuedActions_.at(index); }
	/// Returns the index of an action in the (top level of the) queue, or -1 if not found.
	int indexOfQueuedAction(const AMAction* action);

	/// Add an action to the end of the queue. This class takes ownership of the \c action and will log and delete it after running it.
	void addActionToQueue(AMAction* action) { insertActionInQueue(action, -1); }
	/// Insert an action at a position \c index in the queue.  If \c index is -1 or > queuedActionCount(), will append to the end of the queue. If \c index is 0, will insert at the beginning of the queue. This class takes ownership of the \c action and will log and delete it after running it.
	void insertActionInQueue(AMAction* action, int index);
	/// Delete the action at \c index in the queue. This will remove it from the queue and delete the action object. Returns false if \c index out of range.
	bool deleteActionInQueue(int index);
	/// Duplicate an action in the queue. The new copy will be inserted immediately after \c index. Returns false if \c index out of range.
	bool duplicateActionInQueue(int index);
	/// Duplicate a set of actions in the queue. The new copies will be inserted, in their original order, immediately after the last index in \c indexesToCopy. Returns false if any in \c indexesToCopy are out of range.
	bool duplicateActionsInQueue(const QList<int>& indexesToCopy);
	/// Move an action up or down the queue. \c currentIndex is the action to move, and \c finalIndex is the index you want it to end up in when everything is done.  (\c finalIndex can be 0 for the beginning, or -1 to go to the end.) Returns false if \c currentIndex is out of range, or if \c currentIndex == \c finalIndex.
	bool moveActionInQueue(int currentIndex, int finalIndex);



	/// Whether the queue is paused or running. If the queue is running, it will advance automatically to the next action whenever there are actions in the queue.  \see setQueuePaused().
	bool queuePaused() const { return isPaused_; }

	/// This QAbstractItemModel is a useful model for standard Qt views of the AMActionRunner's queue (like AMActionRunnerQueueView). It simply wraps ourself (AMActionRunner) in the interface required of Qt models.
	AMActionRunnerQueueModel* queueModel() { return queueModel_; }


	// The Current Action that we (might be)/are executing
	////////////////////////////

	/// Whether there is an action currently running.
	bool actionRunning() const { return currentAction_ != 0; }
	/// A pointer to the current action. Will be 0 if actionRunning() is false, when no action is running.
	AMAction* currentAction() { return currentAction_; }
	/// A pointer to the current action. Will be 0 if actionRunning() is false, when no action is running.
	const AMAction* currentAction() const { return currentAction_; }


	// Immediate-mode interface
	////////////////////////////////

	/// Instead of going through the queue, it's possible to ask the Action Runner to take an action and run it immediately.  Immediate actions will be run in parallel/independently of the currentAction() if there is one, (although they might wait for their prerequisites to be satisfied, just like any other action.)  When the action finishes, the Action Runner will take care of logging and deleting it.  If it fails, the action's failureResponseInActionRunner() will be respected.
	void runActionImmediately(AMAction* action);
	/// Returns the number of immediate actions currently running
	int immediateActionsCount() const { return immediateActions_.count(); }
	/// Returns the immediately-running action at \c index (which must be >= 0 and < immediateActionsCount()
	AMAction* immediateActionAt(int index);


signals:

	// Signals regarding the currently-running item
	////////////////////////////

	/// Emitted whenever the currently-running action changes.  If the queue reaches the end and is empty, this will be emitted with 0 (for no current action).
	void currentActionChanged(AMAction* action);

	// These signals are useful to views of the current action. This saves them from having to connect and disconnect from all the individual actions
	/// Forwards the progressChanged() signal from the current action, for convenience
	void currentActionProgressChanged(double numerator, double denominator);
	/// Forward the statusTextChanged() signal from the current action, for convenience.
	void currentActionStatusTextChanged(const QString& status);
	/// Forward the expectedDurationChanged() signal from the current action, for convenience
	void currentActionExpectedDurationChanged(double expectedSecondsTotal);
	/// Forward the stateChanged() signal from the current action, for convenience
	void currentActionStateChanged(int state, int previousState);

	// Signals regarding the state of the queue.
	////////////////////////////

	/// This signal is emitted whenever the paused/running state of the queue changes. ie: queuePaused() changes.
	void queuePausedChanged(bool isPaused);



	// Signals used by the Qt model-wrapper (AMActionRunnerQueueModel)
	/////////////////////////////////////////
	void queuedActionAboutToBeAdded(int index);
	void queuedActionAdded(int index);
	void queuedActionAboutToBeRemoved(int index);
	void queuedActionRemoved(int index);

public slots:

	/// Set whether the queue is paused or running.  If the queue is running, it will advance automatically to the next action whenever there are actions in the queue, or when an action is added to an empty queue. Note that setting the queue to paused does not pause the current action... It only pauses the workflow from moving on to the <i>next</i> action after the current one is completed.
	void setQueuePaused(bool isPaused);

	// These slots are useful to controller views of the current action. This saves them from having to deal with the individual currentAction();
	/// Cancel the currently-running action, if there is one. (Returns true if so.)
	bool cancelCurrentAction();
	/// Pause the current action, if there is one, and it can be paused. (Returns true if so.)
	bool pauseCurrentAction();
	/// Resume the current action, if there is one, and it is paused. (Returns true if so)
	bool resumeCurrentAction();

	/// This slot can be used to cancel all immediately-running actions. Returns true if there were any to cancel.
	bool cancelImmediateActions();

protected slots:
	/// Respond internally whenever the state of the currently-running action changes.
	void onCurrentActionStateChanged(int state, int previousState);

	/// Respond internally whenever the state of any immediate-run action changes.
	void onImmediateActionStateChanged(int state, int previousState);


//	/// Respond internally when a nested action adds a subAction
//	void onNestedActionSubActionAboutToBeAdded(int index);
//	/// Respond internally when a nested action adds a subAction
//	void onNestedActionSubActionAdded(int index);
//	/// Respond internally when a nested action removes a subAction
//	void onNestedActionSubActionAboutToBeRemoved(int index);
//	/// Respond internally when a nested action removes a subAction
//	void onNestedActionSubActionRemoved(int index);


protected:
	AMAction* currentAction_;
	bool isPaused_;
	QList<AMAction*> immediateActions_;
	QList<AMAction*> queuedActions_;

	AMActionRunnerQueueModel* queueModel_;

	/// Helper function called when the previous action finishes. If the queue is running and there are more actions, it starts the next one. If the queue is paused or if we're out of actions, it sets the currentAction() to 0. Either way, it emits the currentActionChanged() signal.
	void internalDoNextAction();
	/// Helper function to prompt the user about what to do given that the current action failed, and it specified a "prompt user" failure response. Do they want to retry or move on?
	int internalAskUserWhatToDoAboutFailedAction(AMAction* action);

private:
	/// This is a singleton class, so the constructor is private. Access the only instance of it via s().
	explicit AMActionRunner(QObject *parent = 0);
	/// Destructor. Deletes any actions in the queue. For implementation reasons, does not delete the currently running action, because it might take this action a while to stop and clean-up.
	~AMActionRunner();

	/// The singleton instance
	static AMActionRunner* instance_;

};

/// This class wraps an AMActionRunner into a model that works with the Qt Model/View system. Nominally, it's usually a list model, but it uses the full tree model API so that actions inside nested actions (AMNestedAction subclasses) appear hierarchically.  There is only ever one column.
class AMActionRunnerQueueModel : public QAbstractItemModel {
	Q_OBJECT
public:
	/// Constructor.
	AMActionRunnerQueueModel(AMActionRunner* actionRunner, QObject* parent = 0);

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


	// extra public functions
	///////////////////////////
	/// This version of deleteActionsInQueue is  used by the AMActionRunnerQueueView, since it has a list of QModelIndex that it needs to delete. If \c indexesToDelete contains non-top-level model indexes, this will use the AMNestedAction API to delete sub-actions inside nested actions.
	void deleteActionsInQueue(QModelIndexList indexesToDelete);
	/// This version of duplicateActionsInQueue is used by the AMActionRunnerQueueView, since it has a list of QModelIndex that it needs to duplicate.  If \c indexesToDuplicate contains non-top-level model indexes, this will use the AMNestedAction API to duplicate the sub-actions within the nested action.  If it contains all top-level model indexes, then it simply calls the regular duplicateActionsInQueue().  If there is a combination of top-level and nested indexes, it does nothing (Since what should be done in that situation is ambiguous as to where the duplicated actions should end up.)
	void duplicateActionsInQueue(const QModelIndexList& indexesToCopy);

protected slots:
	void onActionAboutToBeAdded(int index);
	void onActionAdded(int index);
	void onActionAboutToBeRemoved(int index);
	void onActionRemoved(int index);

protected:
	AMActionRunner* actionRunner_;
};

#include <QMimeData>
#include <QPersistentModelIndex>

/// This subclass of QMimeData is used to pass a list of persistent model indexes for the purpose of internal drag-and-drop inside Qt model views. It is not suitable for passing generic mime data to other applications.
/*! \note We use persistent model indexes to guarantee safety in case the model changes (rows added/removed) while the drag is in progress.  Therefore, it's possible that some indexes could be invalid by the time you access them in the drop event via modelIndexList(), if the corresponding row has been removed from the model in the meantime. */
class AMModelIndexListMimeData : public QMimeData {
	Q_OBJECT
public:
	/// Constructor
	AMModelIndexListMimeData(const QModelIndexList& mil) : QMimeData() {
		foreach(const QModelIndex& mi, mil)
			mil_ << mi;
	}

	/// Access the model index list
	QList<QPersistentModelIndex> modelIndexList() const { return mil_; }

	/// Returns the formats we have: only an application-specific binary format. (Internal use only; other apps should not look at this.)
	virtual QStringList formats() const { return QStringList() << "application/octet-stream"; }

protected:
	/// holds a list of QPersistentModelIndex that were the source of the drag event (with the assumption that they pertain to the same model as the drop destination)
	QList<QPersistentModelIndex> mil_;
};

#endif // AMACTIONRUNNER_H
