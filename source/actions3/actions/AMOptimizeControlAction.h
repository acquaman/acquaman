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

	/// Returns true if this action has child actions, false otherwise. True for this control.
	virtual bool hasChildren() const { return true; }
	/// Returns the number of children for this action.
	virtual int numberOfChildren() const { return 2; }

	/// Returns the specific action info for this class.
	const AMOptimizeControlActionInfo* optimizeControlActionInfo() const { return qobject_cast<const AMOptimizeControlActionInfo*>(info()); }
	/// Returns the specific action info for this class.
	const AMOptimizeControlActionInfo* optimizeControlActionInfo() { return qobject_cast<AMOptimizeControlActionInfo*>(info()); }


protected slots:
	/// Sets the configuration.
	void setConfiguration(AMGenericStepScanConfiguration *newConfiguration);
	/// Sets the control being optimized.
	void setControl(AMControl *newControl);

	/// Handles updating the control in response to a change in the configuration.
	void onConfigurationControlChanged();

	/// Handles setting up the mechanism for determining the optimal control position.
	void onScanActionStarted();
	/// Handles situation where the scan action is cancelled.
	void onScanActionCancelled();
	/// Handles situation where the scan action has failed.
	void onScanActionFailed();
	/// Handles identifying the optimal control position.
	void onScanActionSucceeded();

	/// Handles situation where the control move action has cancelled.
	void onControlMoveActionCancelled();
	/// Handles situation where the control move action has failed.
	void onControlMoveActionFailed();
	/// Handles situation where the control move action has succeeded. This means this action has also succeeded.
	void onControlMoveActionSucceeded();

protected:
	/// Returns true if the configuration can be changed, false otherwise. The configuration cannot be changed if the action has started executing.
	bool canChangeConfiguration() const;
	/// Returns true if the control being optimized can be changed, false otherwise. The control cannot be changed if the action has started executing.
	bool canChangeControl() const;

	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();
	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

protected:
	/// The scan configuration.
	AMGenericStepScanConfiguration *configuration_;
	/// The control being optimized.
	AMControl *control_;

	/// The analysis block that helps determine the optimal position.
	AMAnalysisBlock *optimalPositionAB_;

	/// The scan action.
	AMScanAction *scanAction_;
	/// The control move action.
	AMAction3 *controlMoveAction_;
};

#endif // AMOPTIMIZECONTROLACTION_H
