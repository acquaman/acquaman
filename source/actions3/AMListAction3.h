#ifndef AMLISTACTION3_H
#define AMLISTACTION3_H

#include "actions3/AMAction3.h"
#include "actions3/AMActionInfo3.h"

/// This subclass of AMAction provides an easy way to run a list of sub-actions either sequentially or in parallel.
class AMListAction3 : public AMAction3
{
	Q_OBJECT
public:

	/// Specifies whether the sub-actions should be run in parallel or sequentially.
	enum SubActionMode { SequentialMode = 0,
						 ParallelMode
					   };

	/// Constructor
    AMListAction3(AMActionInfo3* info, SubActionMode subActionMode = SequentialMode, QObject *parent = 0);

	/// Copy constructor. Takes care of making copies of the sub-actions
    AMListAction3(const AMListAction3& other);

	/// This virtual function takes the role of a virtual copy constructor. All actions MUST RE-IMPLEMENT to be able to create and return a fully-independent copy of themselves. This allows us to get a detailed sub-class copy without knowing the type of the action.
	/*! It's recommended to make use of the copy constructor when implementing this, to ensure that the base class is copied properly; this takes care of copying sub-actions and prerequisites. */
    virtual AMAction3* createCopy() const { return new AMListAction3(*this); }

	/// Destructor: deletes the sub-actions
    virtual ~AMListAction3();

	// Sub-actions API
	//////////////////////

	// Accessor methods:

	/// Returns whether subactions are going to run in parallel or sequentially
	SubActionMode subActionMode() const { return subActionMode_; }

	/// Returns the number of sub-actions
	int subActionCount() const { return subActions_.count(); }
	/// Returns the sub-action at the given index, or 0 if the index is out of range.
    AMAction3* subActionAt(int index);
	/// Returns the sub-action at the given index, or 0 if the index is out of range.
    const AMAction3* subActionAt(int index) const;

	/// In SequentialMode, returns the currently-running sub-action, or 0 if none is running.  Always returns 0 in ParallelMode.
    const AMAction3* currentSubAction() const { return subActionAt(currentSubActionIndex()); }
	/// In SequentialMode, returns the currently-running sub-action, or 0 if none is running.  Always returns 0 in ParallelMode.
    AMAction3* currentSubAction() { return subActionAt(currentSubActionIndex()); }
	/// In SequentialMode, returns the index of the currently-running sub-action. Returns -1 if prior to running any sub-actions, and subActionCount() if all sub-actions have been run.  Always returns -1 in ParalleMode.
	int currentSubActionIndex() const { return currentSubActionIndex_; }


	// Modification methods:

	/// Appends a sub-action to the end of the sub-actions list. (Fails and returns false if the action is already running.)
	/*! The action takes ownership of its sub-actions, and will delete them when deleted */
    bool addSubAction(AMAction3* action) { return insertSubAction(action, -1); }
	/// Inserts a sub-action into the sub-actions list. \c index is the position the sub-action will end up in. (Fails and returns false if the action is already running.) If \c index is 0, the sub-action is inserted at the beginning. If \c index >= subActionCount() or < 0, the sub-action is inserted at the end.
	/*! The action takes ownership of its sub-actions, and will delete them when deleted */
    bool insertSubAction(AMAction3* action, int index);

	/// Remove and return the sub-action at \c index. Returns 0 if the index is out of range, or if the action is already running and this is not allowed. Ownership of the sub-action becomes the responsibility of the caller.
    AMAction3* takeSubActionAt(int index);

	/// Control whether to run sub-actions in parallel or sequentially. The default is to run them sequentially. (Returns false if the action is already running and you can't change this.)
	bool setSubActionMode(SubActionMode parallelOrSequential);

	// Re-implemented public functions
	///////////////////////

	/// Re-implemented from AMAction to indicate we can pause. In sequential mode we can pause if the current action is running and can pause, OR if there's more than one action (ie: we can pause between actions). In parallel mode, we can pause if all of our still-running parallel actions can pause.  If there are no sub-actions, we cannot pause because the action would run instantly -- there would be no time to pause.
	virtual bool canPause() const;

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


public slots:


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



private:
	/// Ordered list of sub-actions
    QList<AMAction3*> subActions_;
	/// Whether to run sub-actions sequentially or in parallel
	SubActionMode subActionMode_;
	/// In SequentialMode, indicates the current sub-action. This is -1 before starting, 0 immediately after starting, and equals subActionCount() when all actions are done.  It is updated immediately after the previous sub-action succeeds. If we are Pausing and supposed to stop between actions, it is still advanced, but the next action is not yet started().
	int currentSubActionIndex_;

	/// Helper function to connect a sub-action to our state-monitoring slots
    void internalConnectAction(AMAction3* action);
	/// Helper function to disconnect a sub-action from our state-monitoring slots
    void internalDisconnectAction(AMAction3* action);

private slots:
	/// Called when any of the sub-actions changes state.
	void internalOnSubActionStateChanged(int newstate, int oldstate);
	/// Called when any of the sub-actions emit progressChanged(). Time to re-calculate our progress.
	void internalOnSubActionProgressChanged(double numerator, double denominator);
	/// Called when any of the sub-actions emits statusTextChanged()
	void internalOnSubActionStatusTextChanged(const QString& statusText);

};

#endif // AMLISTACTION_H
