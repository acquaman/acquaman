#ifndef AMDETECTORSETASDARKCURRENTTIMEACTION_H
#define AMDETECTORSETASDARKCURRENTTIMEACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMDetectorSetAsDarkCurrentTimeActionInfo.h"
#include "beamline/AMDetector.h"

class AMDetectorSetAsDarkCurrentTimeAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMDetectorSetAsDarkCurrentTimeAction(AMDetectorSetAsDarkCurrentTimeActionInfo *info, AMDetector *detector, QObject *parent = 0);
	/// Copy constructor.
	AMDetectorSetAsDarkCurrentTimeAction(const AMDetectorSetAsDarkCurrentTimeAction &other);
	/// Destructor.
	virtual ~AMDetectorSetAsDarkCurrentTimeAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new AMDetectorSetAsDarkCurrentTimeAction(*this); }
	/// Returns the detector.
	AMDetector* detector() const { return detector_; }
	/// Returns whether this action can be paused, false in this case.
	virtual bool canPause() const { return false; }
	/// Returns whether this action can be skipped, false in this case.
	virtual bool canSkip() const { return false; }
	/// Returns whether this action has child actions.
	virtual bool hasChildren() const { return false; }
	/// Returns the number of child actions.
	virtual int numberOfChildren() const { return 0; }

protected:
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation() { return; }
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation() { return; }
	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation() { setCancelled(); }
	/// Since this action does not support skipping, the method is empty.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

	/// Returns action-specific info.
	const AMDetectorSetAsDarkCurrentTimeActionInfo* detectorSetAsDarkCurrentTimeActionInfo() const { return qobject_cast<const AMDetectorSetAsDarkCurrentTimeActionInfo*>(info()); }
	/// Returns action-specific info.
	AMDetectorSetAsDarkCurrentTimeActionInfo* detectorSetAsDarkCurrentTimeActionInfo() { return qobject_cast<AMDetectorSetAsDarkCurrentTimeActionInfo*>(info()); }

protected:
	/// The detector.
	AMDetector *detector_;

};

#endif // AMDETECTORSETASDARKCURRENTTIMEACTION_H
