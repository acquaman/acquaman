#ifndef AMOPTIMIZECONTROLACTION_H
#define AMOPTIMIZECONTROLACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMOptimizeControlActionInfo.h"
#include "actions3/actions/AMScanAction.h"
#include "beamline/AMControl.h"
#include "dataman/AMAnalysisBlock.h"

class AMOptimizeControlAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMOptimizeControlAction(AMOptimizeControlActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	AMOptimizeControlAction(const AMOptimizeControlAction &original);
	/// Destructor.
	virtual ~AMOptimizeControlAction();

	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new AMOptimizeControlAction(*this); }

	/// Returns whether this action can be paused, false for this action.
	virtual bool canPause() const { return false; }
	/// Returns whether this action can be skipped, false for this action.
	virtual bool canSkip() const { return false; }

	/// Returns true if this action can report child actions, false otherwise. False for this control.
	virtual bool hasChildren() const { return false; }
	/// Returns the number of children for this action, if any.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the specific action info for this class.
	const AMOptimizeControlActionInfo* optimizeControlActionInfo() const { return qobject_cast<const AMOptimizeControlActionInfo*>(info()); }
	/// Returns the specific action info for this class.
	const AMOptimizeControlActionInfo* optimizeControlActionInfo() { return qobject_cast<AMOptimizeControlActionInfo*>(info()); }

protected slots:
	/// Handles setting up the mechanism for determining the optimal control position and notifying listeners that this action has started.
	void onScanActionStarted();
	/// Handles identifying the optimal control position, and creating the control move action.
	void onScanActionSucceeded();
	/// Handles notifying listeners that this action has succeeded, and deletes the child actions.
	void onControlMoveActionSucceeded();

	/// Handles notifying listeners that this action has been cancelled.
	void onChildActionCancelled();
	/// Handles notifying listeners that this action has failed.
	void onChildActionFailed();
	/// Handles notifying listeners that this action has been paused.
	void onChildActionPaused();
	/// Handles notifying listeners that this action has been resumed.
	void onChildActionResumed();

protected:
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, calls notifyStarted().
	virtual void startImplementation();
	/// This function is called from the Pausing state when the implementation should pause the action. This action does not support pausing.
	virtual void pauseImplementation() { return; }
	/// This function is called from the Paused state when the implementation should resume the action. This action does not support resuming.
	virtual void resumeImplementation();
	/// This function is called from the Cancelling state when the implementation should cancel the action. Once the action is cancelled and can be deleted, calls notifyCancelled().
	virtual void cancelImplementation();
	/// This function is called when the implementation should stop the action. This action does not support skipping.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command) return; }

	/// Creates and returns a connected scan action using the given configuration. Returns 0 if the configuration is invalid.
	virtual AMScanAction* createScanAction(AMScanConfiguration *configuration);
	/// Creates and returns a connected control move action using the given configuration and position setpoint. Returns 0 if the configuration is invalid.
	virtual AMAction3* createControlMoveAction(AMScanConfiguration *configuration, double optimalPosition);

protected:
	/// The scan action.
	AMScanAction *scanAction_;
	/// The control move action.
	AMAction3 *controlMoveAction_;
};

#endif // AMOPTIMIZECONTROLACTION_H
