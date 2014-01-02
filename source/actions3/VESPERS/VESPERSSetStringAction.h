#ifndef VESPERSSETSTRINGACTION_H
#define VESPERSSETSTRINGACTION_H

#include "actions3/AMAction3.h"
#include "actions3/VESPERS/VESPERSSetStringActionInfo.h"
#include "beamline/AMPVControl.h"

#define VESPERSSETSTRINGACTION_NOCONTROL 674600
#define VESPERSSETSTRINGACTION_SUCCEEDED 674601
#define VESPERSSETSTRINGACTION_FAILED 674602

/// VESPERS specific action that sets the specific strings to PVs that are ascii arrays instead of strings.
class VESPERSSetStringAction : public AMAction3
{
	Q_OBJECT

public:
	/// Constructor.  Requires and takes ownership of an existing VESPERSSetStringActionInfo.  Requires a control to act on as well.
 	virtual ~VESPERSSetStringAction();
	Q_INVOKABLE VESPERSSetStringAction(VESPERSSetStringActionInfo *info, AMControl *control = 0, QObject *parent = 0);
	/// Copy constructor.
	VESPERSSetStringAction(const VESPERSSetStringAction &other);
	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const { return new VESPERSSetStringAction(*this); }

	/// Returns the control that this action will move.
	AMControl *control() const { return control_; }
	/// Sets the control used by this action.  This will generally not be used because the control will be provided by the constructor.
	void setControl(AMControl *control) { control_ = control; }

	// Re-implemented public functions
	///////////////////////////////

	/// Specify that we cannot pause (since AMControl cannot pause).  If we wanted to get fancy, we might check if the control could stop, (and stop it for pause, and then start it again to resume). But this is much simpler for now.
	virtual bool canPause() const { return false; }
	/// This action cannot skip.
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

protected slots:
	/// Slot that handles the value changed signal after changing the value.
	void onValueChanged();

protected:

	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic; they only need to fill in their pieces.

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

	// Internal variables:

	/// Timer used to issue progress updates on a per-second basis
	QTimer progressTick_;

	/// A pointer to the AMControl we use to implement the action
	AMControl* control_;
};

#endif // VESPERSSETSTRINGACTION_H
