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


#ifndef AMACTIONRUNNER3_H
#define AMACTIONRUNNER3_H

#include <QObject>
#include <QList>
#include <QStandardItemModel>
#include <QStandardItem>

#include "actions3/actions/AMScanAction.h"

#define AMACTIONRUNNER_MODELINDEX_REQUESTED_CHILD_OF_NON_LIST_TYPE 215001
#define AMACTIONRUNNER_MODELDATA_NO_ACTION_AT_INDEX 215002
#define AMACTIONRUNNER_RETRIEVE_INDEX_FAILED_NO_MATCHING_ACTION_IN_QUEUE 215003
#define AMACTIONRUNNER_RETRIEVE_INDEX_FAILED_NO_MATCHING_ACTION_IN_LIST_TYPE 215004
#define AMACTIONRUNNER_CANNOT_ADD_SUBBACTION_WITH_INVALID_PARENT 215005
#define AMACTIONRUNNER_CANNOT_ADD_SUBACTION_WITH_PARENT_NOT_IN_QUEUE 215006
#define AMACTIONRUNNER_CANNOT_ADD_SUBACTION_CALL_MISMATCH 215007
#define AMACTIONRUNNER_CANNOT_REMOVE_SUBBACTION_WITH_INVALID_PARENT 215008
#define AMACTIONRUNNER_CANNOT_REMOVE_SUBACTION_WITH_PARENT_NOT_IN_QUEUE 215009
#define AMACTIONRUNNER_CANNOT_REMOVE_SUBACTION_CALL_MISMATCH 215010
#define AMACTIONRUNNER_FAILED_TO_DROP_INSIDE_INVALID_LIST_TYPE 215011
#define AMACTIONRUNNER_FAILED_TO_MOVE_INSIDE_INVALID_LIST_TYPE 215012
#define AMACTIONRUNNER_FAILED_TO_REARRANGE_INVALID_ITEM_FOUND 215013
#define AMACTIONRUNNER_FAILED_TO_MOVE_INTO_INVALID_LIST_TYPE 215014
#define AMACTIONRUNNER_IGNORING_REQUEST_FROM_VIEW_TO_REMOVE_ROWS 215015

class AMAction3;
class AMActionRunnerQueueModel3;
class AMScanController;

/// This singleton class provides the API for Acquaman's workflow manager. You can queue up actions for it to run, manage the queue of upcoming actions, and receive notifications when actions are completed. After an action is completed, it will automatically log the actions with AMActionLog::logCompletedAction().  The AMActionRunnerCurrentView and AMActionRunnerQueueView provide graphical user interfaces to this API.
/*! Note that the AMActionRunner's queue is initially paused when it is first created. To have it start executing actions as soon as they become available, call AMActionRunner::s()->setPaused(false).*/
class AMActionRunner3 : public QObject
{
	Q_OBJECT
public:
	/// This is a singleton class. You access the only instance of it using AMActionRunner::workflow().
	static AMActionRunner3* workflow();
	/// Release and delete the singleton instance
	static void releaseWorkflow();

	/// The singleton access to the scan action runner.
	static AMActionRunner3* scanActionRunner();
	/// Release and delete the singleton instance.
	static void releaseScanActionRunner();

	/// Unique name for one of the preset action runner instances
	QString actionRunnerTitle() { return actionRunnerTitle_; }

	// The Queue of Upcoming Actions:  (These all refer to top-level actions. To manage ordering and moving sub-actions within list actions like loop actions, programmers should use the AMNestedAction interface.)
	///////////////////////////////////

	/// The number of actions that are in the queue. (Does not include the possible currently-running action.)
	int queuedActionCount() const { return queuedActions_.count(); }
	/// Returns a pointer to an action in the queue, or 0 if the index is out of range.
	const AMAction3* queuedActionAt(int index) const { if(index < 0 || index >= queuedActions_.count()) return 0; return queuedActions_.at(index); }
	/// Returns a pointer to an action in the queue, or 0 if the index is out of range.
	AMAction3* queuedActionAt(int index) { if(index < 0 || index >= queuedActions_.count()) return 0; return queuedActions_.at(index); }
	/// Returns the index of an action in the (top level of the) queue, or -1 if not found.
	int indexOfQueuedAction(const AMAction3* action);

	/// Add an action to the end of the queue. This class takes ownership of the \c action and will log and delete it after running it.
	void addActionToQueue(AMAction3* action) { insertActionInQueue(action, -1); }
	/// Insert an action at a position \c index in the queue.  If \c index is -1 or > queuedActionCount(), will append to the end of the queue. If \c index is 0, will insert at the beginning of the queue. This class takes ownership of the \c action and will log and delete it after running it.
	void insertActionInQueue(AMAction3* action, int index);
	/// Insert an action at a position \c index in the queue.  If \c index is -1 or > queuedActionCount(), will append to the end of the queue. If \c index is 0, will insert at the beginning of the queue. This class takes ownership of the \c action and will log and delete it after running it.
	bool InsertActionToQueue(AMAction3 *action, int index);
	/// remove the action at \c index in the queue. This will remove it from the queue. Returns the action (0, if it is out of index).
	AMAction3* removeActionFromQueue(int index);
	/// Delete the action at \c index in the queue. This will remove it from the queue and delete the action object. Returns false if \c index out of range.
	bool deleteActionInQueue(int index);
	/// Duplicate an action in the queue. The new copy will be inserted immediately after \c index. Returns false if \c index out of range.
	bool duplicateActionInQueue(int index);
	/// Duplicate a set of actions in the queue. The new copies will be inserted, in their original order, immediately after the last index in \c indexesToCopy. Returns false if any in \c indexesToCopy are out of range.
	bool duplicateActionsInQueue(const QList<int>& indexesToCopy);
	/// Move an action up or down the queue. \c currentIndex is the action to move, and \c finalIndex is the index you want it to end up in when everything is done.  (\c finalIndex can be 0 for the beginning, or -1 to go to the end.) Returns false if \c currentIndex is out of range, or if \c currentIndex == \c finalIndex.
	bool moveActionInQueue(int currentIndex, int finalIndex);

	/// Assuming no actions are running (ie: actionRunning() returns false), sometimes it's desireable to run an action immediately without affecting other actions that might already be in the queue. This function will run a single action immediately by adding it to the front of the queue, starting it, and then restoring the queuePaused() state. Obviously, this is not possible if there is already an action running, in that case, it will simply return false.
	/*! If instead you want to run an action immediately "in the background" regardless of whether actions are running in the queue, use the immediate mode interface with runActionImmediately() */
	bool runActionImmediatelyInQueue(AMAction3* action);

	/// returns whether the action runner is pausable at this moment
	bool isActionRunnerPausable() const;
	/// Whether the queue is paused or running. If the queue is running, it will advance automatically to the next action whenever there are actions in the queue.  \see setQueuePaused().
	bool queuePaused() const { return isPaused_; }


	/// This QAbstractItemModel is a useful model for standard Qt views of the AMActionRunner's queue (like AMActionRunnerQueueView). It simply wraps ourself (AMActionRunner) in the interface required of Qt models.
	AMActionRunnerQueueModel3* queueModel() { return queueModel_; }

	AMDatabase* loggingDatabase() const { return loggingDatabase_; }

	int cachedLogCount() const { return cachedLogCount_; }


	// The Current Action that we (might be)/are executing
	////////////////////////////

	/// Whether there is an action currently running.
	bool actionRunning() const { return currentAction_ != 0; }
	/// A pointer to the current action. Will be 0 if actionRunning() is false, when no action is running.
	AMAction3* currentAction() { return currentAction_; }
	/// A pointer to the current action. Will be 0 if actionRunning() is false, when no action is running.
	const AMAction3* currentAction() const { return currentAction_; }


	// Immediate-mode interface
	////////////////////////////////

	/// Instead of going through the queue, it's possible to ask the Action Runner to take an action and run it immediately.  Immediate actions will be run in parallel/independently of the currentAction() if there is one, (although they might wait for their prerequisites to be satisfied, just like any other action.)  When the action finishes, the Action Runner will take care of logging and deleting it.  If it fails, the action's failureResponseInActionRunner() will be respected.
	void runActionImmediately(AMAction3* action);
	/// Returns the number of immediate actions currently running
	int immediateActionsCount() const { return immediateActions_.count(); }
	/// Returns the immediately-running action at \c index (which must be >= 0 and < immediateActionsCount()
	AMAction3* immediateActionAt(int index);


signals:

	// Signals regarding the currently-running item
	////////////////////////////

	/// Emitted whenever the currently-running action changes.  If the queue reaches the end and is empty, this will be emitted with 0 (for no current action).
	void currentActionChanged(AMAction3* action);

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
	/// the signal is emitted wheneve the pausable of the action runner is changed
	void actionRunnerPausableChanged(bool isPausable);
	/// This signal is emitted whenever the paused/running state of the queue changes. ie: queuePaused() changes.
	void queuePausedChanged(bool isPaused);



	// Signals used by the Qt model-wrapper (AMActionRunnerQueueModel)
	/////////////////////////////////////////
	void queuedActionAboutToBeAdded(int index);
	void queuedActionAdded(int index);
	void queuedActionAboutToBeRemoved(int index);
	void queuedActionRemoved(int index);
	void queuedActionInfoChanged();

	// Signals specific to AMScanAction.  Since other parts of the application will likely want to know some of these things.
	/// Notifier that the scan action has been created.  Note that a scan controller is not created at this point.
	void scanActionCreated(AMScanAction *);
	/// Notifier that the scan action has been started.
	void scanActionStarted(AMScanAction *);
	/// Notifier that the scan action has finished (made it to either Succeeded, Failed, or Cancelled).
	void scanActionFinished(AMScanAction *);

public slots:
	/// slot to update the pausable state of the action runner
	void updateActionRunnerPausable();
	/// Set whether the queue is paused or running.  If the queue is running, it will advance automatically to the next action whenever there are actions in the queue, or when an action is added to an empty queue. Note that setting the queue to paused does not pause the current action... It only pauses the workflow from moving on to the <i>next</i> action after the current one is completed.
	void setQueuePaused(bool isPaused);

	// These slots are useful to controller views of the current action. This saves them from having to deal with the individual currentAction()
	/// Cancel the currently-running action, if there is one. (Returns true if so.)
	bool cancelCurrentAction();
	/// Pause the current action, if there is one, and it can be paused. (Returns true if so.)
	bool pauseCurrentAction();
	/// Resume the current action, if there is one, and it is paused. (Returns true if so)
	bool resumeCurrentAction();

	/// This slot can be used to cancel all immediately-running actions. Returns true if there were any to cancel.
	bool cancelImmediateActions();

	void incrementCachedLogCount();
	void resetCachedLogCount();

protected slots:
	/// Notifier that the scan action has been created.  Note that a scan controller is not created at this point.
	void onScanActionCreated(AMScanAction *);
	/// Notifier that the scan action has been started.
	void onScanActionStarted(AMScanAction *);
	/// Notifier that the scan action has finished (made it to either Succeeded, Failed, or Cancelled).
	void onScanActionFinished(AMScanAction *);

	/// Respond internally whenever the state of the currently-running action changes.
	void onCurrentActionStateChanged(int state, int previousState);

	/// Respond internally whenever the state of any immediate-run action changes.
	void onImmediateActionStateChanged(int state, int previousState);

protected:
	/// Helper method that returns whether the current action is a scan action or not.
	bool isScanAction() const;
	/// set the current action, return true if the current action is added successfully
	void setCurrentAction(AMAction3 *newAction);
	/// check the validity of the given action, return true if the validation is successful
	bool validateAction(AMAction3 *newAction, const QString &okButtonText, const QString &cancelButtonText);

	void onCurrentActionStarting();
	void onCurrentActionRunning();
	void onCurrentActionFailed();
	void onCurrentActionFinished();

protected:
	AMAction3* currentAction_;
	/// flag to identify whether the action runner was pausable or not
	bool wasActionRunnerPausable_;
	/// flag to identify whether the action runner is enabled to be paused or not
	bool isActionRunnerPauseEnabled_;
	bool isPaused_;
	QList<AMAction3*> immediateActions_;
	QList<AMAction3*> queuedActions_;

	AMActionRunnerQueueModel3* queueModel_;
	AMDatabase *loggingDatabase_;
	int cachedLogCount_;

	/// Holds a unique title for the different action runner instances
	QString actionRunnerTitle_;

	/// Helper function called when the previous action finishes. If the queue is running and there are more actions, it starts the next one. If the queue is paused or if we're out of actions, it sets the currentAction() to 0. Either way, it emits the currentActionChanged() signal.
	void internalDoNextAction();
	/// Helper function to prompt the user about what to do given that the current action failed, and it specified a "prompt user" failure response. Do they want to retry or move on?
	int internalAskUserWhatToDoAboutFailedAction(AMAction3* action);

private:
	/// This is a singleton class, so the constructor is private. Access the only instance of it via s().
	explicit AMActionRunner3(AMDatabase *loggingDatabase, const QString &actionRunnerTitle, QObject *parent = 0);
	/// Destructor. Deletes any actions in the queue. For implementation reasons, does not delete the currently running action, because it might take this action a while to stop and clean-up.
	virtual ~AMActionRunner3();

	/// The singleton instance for the workflow
	static AMActionRunner3 *workflowInstance_;

	/// The singleton instance for the scan action runner
	static AMActionRunner3 *scanActionRunnerInstance_;

};

/// This class wraps an AMActionRunner into a model that works with the Qt Model/View system. Nominally, it's usually a list model, but it uses the full tree model API so that actions inside list actions (AMNestedAction subclasses) appear hierarchically.  There is only ever one column.
class AMActionRunnerQueueModel3 : public QAbstractItemModel {
	Q_OBJECT
public:
	/// Constructor.
	virtual ~AMActionRunnerQueueModel3();
	AMActionRunnerQueueModel3(AMActionRunner3* actionRunner, QObject* parent = 0);

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	QModelIndex parent(const QModelIndex &child) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

	AMAction3* actionAtIndex(const QModelIndex& index) const;
	// requires a linear search of the \c action's parentAction(), or of the actionRunner queue if the action is at the top level.
	QModelIndex indexForAction(AMAction3* action) const;


	// Drag and Drop functions:
	////////////////////////////////

	/// Re-implemented from QAbstractItemModel to deal with dropping when re-ordering the queue via drag-and-drop.
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	/// Re-implemented from QAbstractItemModel to deal with dragging when re-ordering the queue via drag-and-drop.
	virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
	/// Re-implemented from QAbstractItemModel to deal with dragging when re-ordering the queue via drag-and-drop.
	virtual QStringList mimeTypes() const;
	/// Re-implemented from QAbstractItemModel to deal with dropping when re-ordering the queue via drag-and-drop
	virtual Qt::DropActions supportedDropActions() const { return (Qt::MoveAction | Qt::IgnoreAction); }

	/// Re-implemented only for debugging purposes (temporary).
	bool removeRows(int row, int count, const QModelIndex &parent);

	// extra public functions
	///////////////////////////
	/// This version of deleteActionsInQueue is  used by the AMActionRunnerQueueView, since it has a list of QModelIndex that it needs to delete. If \c indexesToDelete contains non-top-level model indexes, this will use the AMNestedAction API to delete sub-actions inside list actions.
	void deleteActionsInQueue(QModelIndexList indexesToDelete);
	/// This version of duplicateActionsInQueue is used by the AMActionRunnerQueueView, since it has a list of QModelIndex that it needs to duplicate.  If \c indexesToDuplicate contains non-top-level model indexes, this will use the AMNestedAction API to duplicate the sub-actions within the list action.  If it contains all top-level model indexes, then it simply calls the regular duplicateActionsInQueue().  If there is a combination of top-level and list indexes, it does nothing (Since what should be done in that situation is ambiguous as to where the duplicated actions should end up.)
	void duplicateActionsInQueue(const QModelIndexList& indexesToCopy);

protected slots:
	void onActionAboutToBeAdded(int index);
	void onActionAdded(int index);
	void onActionAboutToBeRemoved(int index);
	void onActionRemoved(int index);

	void onActionInfoChanged();

	// called when any list action
	void onSubActionAboutToBeAdded(int index);
	void onSubActionAdded(int index);
	void onSubActionAboutToBeRemoved(int index);
	void onSubActionRemoved(int index);

protected:
	AMActionRunner3* actionRunner_;

	AMAction3* lastSender_;

	void internalConnectListActions(AMAction3* action);
	void internalDisconnectListActions(AMAction3* action);
};

#include <QMimeData>
#include <QPersistentModelIndex>

/// This subclass of QMimeData is used to pass a list of persistent model indexes for the purpose of internal drag-and-drop inside Qt model views. It is not suitable for passing generic mime data to other applications.
/*! \note We use persistent model indexes to guarantee safety in case the model changes (rows added/removed) while the drag is in progress.  Therefore, it's possible that some indexes could be invalid by the time you access them in the drop event via modelIndexList(), if the corresponding row has been removed from the model in the meantime. */
class AMModelIndexListMimeData3 : public QMimeData {
	Q_OBJECT
public:
	/// Constructor
	virtual ~AMModelIndexListMimeData3();
	AMModelIndexListMimeData3(const QModelIndexList& mil) : QMimeData() {
		foreach(const QModelIndex& mi, mil)
			mil_ << mi;
	}

	/// Access the model index list
	QList<QPersistentModelIndex> modelIndexList() const { return mil_; }

	/// Returns the formats we have: only an application-specific binary format. (Internal use only, other apps should not look at this.)
	virtual QStringList formats() const { return QStringList() << "application/octet-stream"; }

protected:
	/// holds a list of QPersistentModelIndex that were the source of the drag event (with the assumption that they pertain to the same model as the drop destination)
	QList<QPersistentModelIndex> mil_;
};

#endif // AMACTIONRUNNER3_H
