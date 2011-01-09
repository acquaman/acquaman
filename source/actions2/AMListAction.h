#ifndef AMLISTACTION_H
#define AMLISTACTION_H

#include "actions2/AMAction.h"
#include "actions2/AMActionInfo.h"

/// This subclass of AMAction provides an easy way to run a list of sub-actions either sequentially or in parallel.
class AMListAction : public AMAction
{
	Q_OBJECT
public:

	/// Specifies whether the sub-actions should be run in parallel or sequentially.
	enum SubActionMode { SequentialMode = 0,
						 ParallelMode
					   };

	/// Constructor
	AMListAction(AMActionInfo* info, SubActionMode subActionMode = SequentialMode, QObject *parent = 0);

	/// Copy constructor. Takes care of making copies of the sub-actions
	AMListAction(const AMListAction& other);

	/// This virtual function takes the role of a virtual copy constructor. All actions MUST RE-IMPLEMENT to be able to create and return a fully-independent copy of themselves. This allows us to get a detailed sub-class copy without knowing the type of the action.
	/*! It's recommended to make use of the copy constructor when implementing this, to ensure that the base class is copied properly; this takes care of copying sub-actions and prerequisites. */
	virtual AMAction* createCopy() const { return new AMListAction(*this); }

	/// Destructor: deletes the sub-actions
	virtual ~AMListAction();

	// Sub-actions API
	//////////////////////

	/// Returns the number of sub-actions
	int subActionCount() const { return subActions_.count(); }
	/// Returns the currently-running sub-action, or 0 if none is running.
	const AMAction* currentSubAction() const { return subActionAt(currentSubActionIndex()); }
	/// Returns the currently-running sub-action, or 0 if none is running.
	AMAction* currentSubAction() { return subActionAt(currentSubActionIndex()); }
	/// Returns the index of the currently-running sub-action. Returns -1 if prior to running any sub-actions, and subActionCount() if after all sub-actions have been run.
	int currentSubActionIndex() const { return currentSubActionIndex_; }
	/// Returns the sub-action at the given index, or 0 if the index is out of range.
	AMAction* subActionAt(int index);
	/// Returns the sub-action at the given index, or 0 if the index is out of range.
	const AMAction* subActionAt(int index) const;
	/// Returns whether subactions are going to run in parallel or sequentially
	SubActionMode subActionMode() const { return subActionMode_; }




	/// Appends a sub-action to the end of the sub-actions list. (Fails and returns false if the action is already running.)
	/*! The action takes ownership of its sub-actions and will delete them when deleted */
	bool addSubAction(AMAction* action) { return insertSubAction(action, -1); }
	/// Inserts a sub-action into the sub-actions list. \c index is the position the sub-action will end up in. (Fails and returns false if the action is already running.) If \c index is 0, the sub-action is inserted at the beginning. If \c index >= subActionCount() or < 0, the sub-action is inserted at the end.
	/*! The action takes ownership of its sub-actions and will delete them when deleted */
	bool insertSubAction(AMAction* action, int index);

	/// Remove and return the sub-action at \c index. Returns 0 if the index is out of range, or if the action is already running and this is not allowed. Ownership of the sub-action becomes the responsibility of the caller.
	AMAction* takeSubActionAt(int index);

	/// Control whether to run sub-actions in parallel or sequentially. The default is to run them sequentially. (Returns false if the action is already running and you can't change this.)
	bool setSubActionMode(SubActionMode parallelOrSequential);

	// Re-implemented public functions
	///////////////////////

	/// Re-implemented from AMAction to indicate we can pause. In sequential mode we can pause if the current action can pause, or there's more than one action (ie: we can pause between actions). In parallel mode, we can pause if all of our parallel actions can pause.  If there are no sub-actions, we cannot pause because the action would run instantly.
	virtual bool canPause() const;

signals:

	// These signals help views that want to be informed when sub-actions are created or removed
	void subActionAboutToBeAdded(int index);
	void subActionAdded(int index);
	void subActionAboutToBeRemoved(int index);
	void subActionRemoved(int index);



public slots:


protected:

	// re-implemented from AMAction

	/// This function is called from the Starting state when the implementation should initiate the action. We start either our first action, or all of them in parallel.
	virtual void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. If running in sequential mode, we can pause -- either the current action, or pause between
	virtual void pauseImplementation();

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation();

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

private:
	QList<AMAction*> subActions_;
	SubActionMode subActionMode_;
	int currentSubActionIndex_;

	/// Helper function to connect a sub-action to our state-monitoring slots
	void internalConnectAction(AMAction* action);
	/// Helper function to disconnect a sub-action from our state-monitoring slots
	void internalDisconnectAction(AMAction* action);

private slots:
	/// Called when any of the sub-actions changes state.
	void internalOnSubActionStateChanged(int newstate, int oldstate);
	/// Called when any of the sub-actions emit progressChanged(). Time to re-calculate our progress.
	void internalOnSubActionProgressChanged(double numerator, double denominator);
	/// Called when any of the sub-actions emits statusTextChanged()
	void internalOnSubActionStatusTextChanged(const QString& statusText);

};

#endif // AMLISTACTION_H
