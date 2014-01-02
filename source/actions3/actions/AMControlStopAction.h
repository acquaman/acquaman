#ifndef AMCONTROLSTOPACTION_H
#define AMCONTROLSTOPACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMControlStopActionInfo.h"

class AMControl;

#define AMCONTROLSTOPACTION_NO_CONTROL 909000
#define AMCONTROLSTOPACTION_CANT_STOP 909001
#define AMCONTROLSTOPACTION_IMMEDIATE_FAILURE 909002
#define AMCONTROLSTOPACTION_COULDNT_STOP_FAILURE 909003

/// This implementation takes a control and will stop it and notify about the success of that process.
class AMControlStopAction : public AMAction3
{
	Q_OBJECT

public:
	/// Constructor.  Requires and takes ownership of an existing AMControlStopActionInfo.  Provides an AMControl \param control that will be stopped, if possible.
 	virtual ~AMControlStopAction();
	Q_INVOKABLE AMControlStopAction(AMControlStopActionInfo *info, AMControl *control, QObject *parent = 0);
	/// Copy constructor.
	AMControlStopAction(const AMControlStopAction &other);
	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const { return new AMControlStopAction(*this); }

	/// Returns the control that this action will move.
	AMControl *control() const { return control_; }
	/// Sets the control used by this action.  This will genearlly not be used because the control will be provided by the constructor.
	void setControl(AMControl *control) { control_ = control; }

	// Re-implemented public functions.
	///////////////////////////////////////////////

	/// Specifies that we cannot pause (since AMControl cannot pause).
	virtual bool canPause() const { return false; }
	/// Specifies that we cannot skip (since AMControl cannot be skipped).
	virtual bool canSkip() const { return false; }
	/// Specifies that there are no children associated with the action.
	virtual bool hasChildren() const { return false; }
	/// Specifies the number of children, which is identically 0 for all actions of this type.
	virtual int numberOfChildren() const { return 0; }

protected slots:
	/// Handles making sure that the control has stopped after the moving changed signal has been emitted.
	void onMovingChanged(bool isMoving);

protected:
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
	virtual void cancelImplementation() { setCancelled(); }

	/// Since this action does not support skipping, the method is empty.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

	/// A pointer to the AMControl we use to implement the action
	AMControl* control_;
};

#endif // AMCONTROLSTOPACTION_H
