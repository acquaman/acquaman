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


#ifndef AMLISTACTION3_H
#define AMLISTACTION3_H

#include "actions3/AMAction3.h"
#include "actions3/AMListActionInfo3.h"

#define AMLISTACTION3_CANNOT_ADD_SUBACTION_ONCE_RUNNING 270801
#define AMLISTACTION3_CANNOT_REMOVE_SUBACTION_ONCE_RUNNING 270802
#define AMLISTACTION3_PAUSE_CALLED_WITH_NO_ACTION_RUNNING 270803
#define AMLISTACTION3_RESUME_CALLED_WHEN_NOT_PAUSED 270804
#define AMLISTACTION3_RESUME_CALLED_WITH_NO_ACTION_RUNNING 270805
#define AMLISTACTION3_SEQUENTIAL_SUBACTION_PAUSED_WITHOUT_PAUSING_PARENT 270806
#define AMLISTACTION3_SEQUENTIAL_SUBACTION_CANCELLED_WITHOUT_CANCELLING_PARENT 270807
#define AMLISTACTION3_PARALLEL_SUBACTION_PAUSED_WITHOUT_PAUSING_PARENT 270808
#define AMLISTACTION3_PARALLEL_SUBACTION_CANCELLED_WITHOUT_CANCELLING_PARENT 270809

class AMScanAction;

/// This subclass of AMAction provides an easy way to run a list of sub-actions either sequentially or in parallel.
class AMListAction3 : public AMAction3
{
	Q_OBJECT
public:

	/// Specifies whether the sub-actions should be run in parallel or sequentially.
	enum SubActionMode {
		Sequential = 0,
		Parallel
	};

	/// Constructor
	Q_INVOKABLE AMListAction3(AMListActionInfo3* info, AMListAction3::SubActionMode subActionMode = Sequential, QObject *parent = 0);

	/// Copy constructor. Takes care of making copies of the sub-actions
	AMListAction3(const AMListAction3& other);

	/// This virtual function takes the role of a virtual copy constructor. All actions MUST RE-IMPLEMENT to be able to create and return a fully-independent copy of themselves. This allows us to get a detailed sub-class copy without knowing the type of the action.
	/*! It's recommended to make use of the copy constructor when implementing this, to ensure that the base class is copied properly; this takes care of copying sub-actions and prerequisites. */
	virtual AMAction3* createCopy() const { return new AMListAction3(*this); }

	/// Destructor: deletes the sub-actions
	virtual ~AMListAction3();

	// Re-implemented public functions
	///////////////////////

	/// Re-implemented from AMAction to indicate we can pause. In sequential mode we can pause if the current action is running and can pause, OR if there's more than one action (ie: we can pause between actions). In parallel mode, we can pause if all of our still-running parallel actions can pause.  If there are no sub-actions, we cannot pause because the action would run instantly -- there would be no time to pause.
	virtual bool canPause() const;
	/// Re-implemented from AMAction to indicate we can skip.  Skipping is supported in sequential mode and NOT supported in parallel mode.
	virtual bool canSkip() const { return subActionMode_ == Sequential; }
	/// Returns the skip options that this action will support.
	virtual QStringList skipOptions() const ;

	/// Pure virtual function that denotes that this action has children underneath it or not.
	bool hasChildren() const { return true; }
	/// Pure virtual function that returns the number of children for this action.
	int numberOfChildren() const { return subActions_.size(); }

	// Sub-actions API
	//////////////////////

	// Accessor methods:

	/// Returns whether subactions are going to run in parallel or sequentially
	SubActionMode subActionMode() const { return subActionMode_; }

	/// Returns the number of sub-actions
	virtual int subActionCount() const { return subActions_.count(); }
	/// Returns the sub-action at the given index, or 0 if the index is out of range.
	virtual AMAction3* subActionAt(int index);
	/// Returns the sub-action at the given index, or 0 if the index is out of range.
	virtual const AMAction3* subActionAt(int index) const;

	/// In SequentialMode, returns the currently-running sub-action, or 0 if none is running.  Always returns 0 in ParallelMode.
	virtual const AMAction3* currentSubAction() const { return subActionAt(currentSubActionIndex()); }
	/// In SequentialMode, returns the currently-running sub-action, or 0 if none is running.  Always returns 0 in ParallelMode.
	virtual AMAction3* currentSubAction() { return subActionAt(currentSubActionIndex()); }
	/// In SequentialMode, returns the index of the currently-running sub-action. Returns -1 if prior to running any sub-actions, and subActionCount() if all sub-actions have been run.  Always returns -1 in ParalleMode.
	virtual int currentSubActionIndex() const { return currentSubActionIndex_; }

	/// Find the index of a sub-action \c action, or -1 if not found.
	int indexOfSubAction(const AMAction3* action) const;

	/// Find the database id of the log action for this list, or -1 if not found.
	int logActionId() const;

	// Modification methods:

	/// Appends a sub-action to the end of the sub-actions list. (Fails and returns false if the action is already running.)
	/*! The action takes ownership of its sub-actions, and will delete them when deleted */
	bool addSubAction(AMAction3* action) { return insertSubAction(action, -1); }
	/// Inserts a sub-action into the sub-actions list. \c index is the position the sub-action will end up in. (Fails and returns false if the action is already running.) If \c index is 0, the sub-action is inserted at the beginning. If \c index >= subActionCount() or < 0, the sub-action is inserted at the end.
	/*! The action takes ownership of its sub-actions, and will delete them when deleted */
	bool insertSubAction(AMAction3* action, int index);
	/// Public interface to delete the sub-action at \c index. Takes care of emitting signals subActionAboutToBeRemoved(), and subActionRemoved(). If \c index >= subActionCount(), returns false.  Only allowed while the action is in the AMAction::Constructed state... Once it's running, you can no longer remove sub-actions and this will return false.
	bool deleteSubAction(int index);

	/// Remove and return the sub-action at \c index. Returns 0 if the index is out of range, or if the action is already running and this is not allowed. Ownership of the sub-action becomes the responsibility of the caller.
	AMAction3* takeSubActionAt(int index);

	AMDatabase* loggingDatabase() const;

	// Other methods that should be reimplemented.
	////////////////////////////////////////////

	/// Specifies whether, when logging actions that are run with AMActionRunner, to log the entire action, or log the individual sub-actions as they complete.
	virtual bool shouldLogSubActionsSeparately() const { return logSubActionsSeparately_; }
	/// Set whether each individual sub-action should be logged separately as soon as it completes, or if the whole loop action should be logged as one long single action (in the user's action history log).
	void setShouldLogSubActionsSeparately(bool logSeparately) { logSubActionsSeparately_ = logSeparately; }

public slots:
	/// Sets the database id of the log action associated with this instance
	void setLogActionId(int logActionId);

	void setLoggingDatabase(AMDatabase *loggingDatabase);

signals:

	// These signals help views that want to be informed when sub-actions are created or removed
	/// Emitted before a sub-action is added at \c index.
	void subActionAboutToBeAdded(int index);
	/// Emitted right after a sub-action is added at \c index
	void subActionAdded(int index);
	/// Emitted right before the sub-action at \c index is removed
	void subActionAboutToBeRemoved(int index);
	/// Emitted right after the sub-action (that used to be at \c index) is removed.
	void subActionRemoved(int index);


	/// Only in Sequential Mode: Emitted when one action finishes. currentSubActionIndex() and currentSubAction() will be updated to refer to the next action _as soon as_ the preceeding action succeeds. Note that this will be emitted with \c newSubActionIndex == subActionCount() when finishing the last action.
	/*! (If we are Pausing and waiting to stop between sequential actions, this signal will still be emitted and currentSubAction() will point to the next action, but that action will not be started yet.)  */
	void currentSubActionChanged(int newSubActionIndex);
	/// Emitted when a sub-action completes.
	void subActionCompleted(AMAction3* completedAction);

	// Signals specific to AMScanAction.  Since other parts of the application will likely want to know some of these things.
	/// Notifier that the scan action has been created.  Note that a scan controller is not created at this point.
	void scanActionCreated(AMScanAction *);
	/// Notifier that the scan action has been started.
	void scanActionStarted(AMScanAction *);
	/// Notifier that the scan action has finished (made it to either Succeeded, Failed, or Cancelled).
	void scanActionFinished(AMScanAction *);

protected slots:
	/// Called when any of the sub-actions changes state.
	virtual void internalOnSubActionStateChanged(int fromState, int toState);
	/// Called when any of the sub-actions emit progressChanged(). Time to re-calculate our progress.
	virtual void internalOnSubActionProgressChanged(double numerator, double denominator);
	/// Called when any of the sub-actions emits statusTextChanged()
	virtual void internalOnSubActionStatusTextChanged(const QString &statusText);

protected:

	// Required implementation functions for AMAction classes.

	/// This function is called from the Starting state when the implementation should initiate the action. In SequentialMode, we start() the first action. In ParallelMode, we start() all of them.  If there are no sub-actions, we will report starting and finishing immediately.
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. If running in sequential mode, we can pause -- either the current action, or pause between actions. In parallel mode, we pause all the still-running actions. (If actions have finished or failed already, we ignore those and pause the ones still running.)
	virtual void pauseImplementation();
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. In SequentialMode, this will either resume the current action (if it supported pausing), or -- when paused between actions -- start the next one. In ParallelMode, this will resume all actions that haven't finished already.
	virtual void resumeImplementation();
	/// All implementations must support cancelling. This function will be called from the Cancelling state. In SequentialMode, we cancel() the currently running action (unless we're paused between actions.)  In ParallelMode, we cancel all the still-running actions.
	virtual void cancelImplementation();
	/// This method does the requisite work to ensure a sequential list can be skipped.  Does nothing if the list is in Parallel mode.
	virtual void skipImplementation(const QString &command);

	// Helper function to help move through a sequential list of actions.
	//////////////////////////////////////

	/// Helper function to step through the various actions in a sequential list.
	virtual void internalDoNextAction();

	// Helper functions to check the state of all actions when running in ParallelMode
	/////////////////
	/// Returns true if all actions are in a final state (Succeeded, Failed, or Cancelled)
	bool internalAllActionsInFinalState() const;
	/// Returns true if all actions are running, or in a final state
	bool internalAllActionsRunningOrFinal() const;
	/// Returns true if all actions are paused, or in a final state
	bool internalAllActionsPausedOrFinal() const;
	/// Returns true if ANY actions are in the failed state
	bool internalAnyActionsFailed() const;
	/// Returns true if ANY actions are in the cancelled state.
	bool internalAnyActionsCancelled() const;

	/// Returns true if all actions specify an expected duration (ie: AMActionInfo::expectedDuration() does not return -1)
	bool internalAllActionsHaveExpectedDuration() const;

	/// Helper function to connect a sub-action to our state-monitoring slots
	void internalConnectAction(AMAction3* action);
	/// Helper function to disconnect a sub-action from our state-monitoring slots
	void internalDisconnectAction(AMAction3* action);
	/// Virtual helper function to handle all clean up responsibilities for an action.  Disconnects and logs the action.  If no parameter is passed it will perform it's own custom cleanup based on the implementation.
	virtual void internalCleanupAction(AMAction3 *action = 0);

	/// Helper function that returns true if we should log a sub-action ourselves when it finishes. True if: we're running inside AMActionRunner, we're supposed to log our sub-actions separately, AND \c action is not itself a nested action that is supposed to log its own sub-actions seperately.  (If \c action IS a nested action, but it's supposed to be logged as one unit, then we'll still log it ourselves.)
	bool internalShouldLogSubAction(AMAction3* action);
	/// Helper function that writing logs
	void internalLogSubAction(AMAction3* action, int toState);

	/// Helper method that returns whether the current action is a scan action or not.
	bool isScanAction() const;

protected:
	/// Ordered list of sub-actions
	QList<AMAction3*> subActions_;
	/// Whether to run sub-actions sequentially or in parallel
	SubActionMode subActionMode_;
	/// In SequentialMode, indicates the current sub-action. This is -1 before starting, 0 immediately after starting, and equals subActionCount() when all actions are done.  It is updated immediately after the previous sub-action succeeds. If we are Pausing and supposed to stop between actions, it is still advanced, but the next action is not yet started().
	int currentSubActionIndex_;
	/// Flag that holds whether the sub actions should be logged separately.
	bool logSubActionsSeparately_;

	/// Holds the id of the log action (these actions are logged first when they start). Will hold -1 until it gets something valid
	int logActionId_;

	/// Flag used when skipping after the next action.
	bool skipAfterCurrentAction_;

	AMDatabase *loggingDatabase_;
};

/// This is a convenience class that builds a sequential list action.  Equivalent to AMListAction(info, SubActionMode::Sequential, parent).
class AMSequentialListAction3 : public AMListAction3
{
	Q_OBJECT

public:
	/// Constructor.  Builds a sequential list action.
 	virtual ~AMSequentialListAction3();
	Q_INVOKABLE AMSequentialListAction3(AMSequentialListActionInfo3 *info, QObject *parent = 0)
		: AMListAction3(info, Sequential, parent)
	{}
};

/// is a convenience class that builds a parallel list action.  Equivalent to AMListAction(info, SubActionMode::Parallel, parent).
class AMParallelListAction3 : public AMListAction3
{
	Q_OBJECT

public:
	/// Constructor.  Builds a parallel list action.
 	virtual ~AMParallelListAction3();
	Q_INVOKABLE AMParallelListAction3(AMParallelListActionInfo3 *info, QObject *parent = 0)
		: AMListAction3(info, Parallel, parent)
	{}
};

#endif // AMLISTACTION_H
