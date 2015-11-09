#ifndef CLSJJSLITOPTIMIZATIONACTION_H
#define CLSJJSLITOPTIMIZATIONACTION_H

#include "actions3/AMAction3.h"
#include "actions3/CLS/CLSJJSlitOptimizationActionInfo.h"
#include "actions3/actions/AMScanAction.h"

class CLSJJSlitOptimizationAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitOptimizationAction(CLSJJSlitOptimizationActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitOptimizationAction(const CLSJJSlitOptimizationAction &original);
	/// Destructor.
	virtual ~CLSJJSlitOptimizationAction();

	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new CLSJJSlitOptimizationActionInfo(*this); }

	/// Returns whether this action can be paused, false for this action.
	virtual bool canPause() const { return false; }
	/// Returns whether this action can be skipped, false for this action.
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

protected slots:
	/// Handles

protected:

	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic, they only need to fill in their pieces.

	// These virtual functions allow us to implement our unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation() { setPaused(); }

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation() { setResumed(); }

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

	/// Since this action does not support skipping, the method is empty.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

};

#endif // CLSJJSLITOPTIMIZATIONACTION_H
