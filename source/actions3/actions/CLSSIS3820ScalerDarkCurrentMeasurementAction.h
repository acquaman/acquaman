#ifndef CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H
#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.h"

//#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_WAIT_SECONDS 0.5

#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_INVALID_SCALER 98230221
#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_INVALID_DWELL_TIME 20290
#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_ACTION_FAILED 293487982

class CLSSIS3820ScalerDarkCurrentMeasurementAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSSIS3820ScalerDarkCurrentMeasurementAction(CLSSIS3820ScalerDarkCurrentMeasurementActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	CLSSIS3820ScalerDarkCurrentMeasurementAction(const CLSSIS3820ScalerDarkCurrentMeasurementAction &other);
	/// Destructor.
	virtual ~CLSSIS3820ScalerDarkCurrentMeasurementAction();

	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new CLSSIS3820ScalerDarkCurrentMeasurementAction(*this); }

	/// Returns true if this action can be paused, always false for this action.
	virtual bool canPause() const { return false; }
	/// Returns true if this action can be skipped, always false for this action.
	virtual bool canSkip() const { return false; }

	/// Returns true if this action has child actions, always false for this action.
	virtual bool hasChildren() const { return false; }
	/// Returns the number of child actions, always 0 for this action.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the specific action info for this class.
	const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo* scalerDarkCurrentMeasurementActionInfo() const { return qobject_cast<const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo*>(info()); }
	/// Returns the specific action info for this class.
	CLSSIS3820ScalerDarkCurrentMeasurementActionInfo* scalerDarkCurrentMeasurementActionInfo() { return qobject_cast<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo*>(info()); }

protected slots:
	/// Handles emitting the appropriate signals when this action has started.
	void onStarted();
	/// Handles emitting the appropriate signals and performing cleanup when this action has failed.
	void onFailed(QObject *action);
	/// Handles emitting the appropriate signals when this action has succeeded.
	void onSucceeded(QObject *action);

	/// Handles action cleanup and restoring pre-measurement settings.
	void onMeasurementFinished(QObject *action);

	/// Updates the saved pre-measurement settings.
	virtual void measurementInitialization();
	/// Applies the saved pre-measurement settings.
	virtual void measurementCleanup();

protected:
	/// Handles action cleanup.
	void actionCleanup(QObject *action);

	/// Returns true if the given dwell time is valid, false otherwise.
	bool validDwellTime(double time) const;

	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action.
	virtual void pauseImplementation() { setPaused(); }
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action.
	virtual void resumeImplementation() { setResumed(); }
	/// All implementations must support cancelling. This function will be called from the Cancelling state when the implementation should cancel the action.
	virtual void cancelImplementation() { setCancelled(); }
	/// The function is called from the Skipping state when the implementation should skip the action. This implementation does not support skipping.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command) return; }

	/// Creates and returns the basic dark current measurement action: sets the dwell time, triggers acquisition, asks all detectors to update their dark current values.
	AMAction3* createMeasurementAction(double secondsDwell);

protected:
	/// The pre-measurement scaler dwell time.
	double oldTime_;

	/// The started mapper.
	QSignalMapper *startedMapper_;
	/// The failed mapper.
	QSignalMapper *failedMapper_;
	/// The succeeded mapper.
	QSignalMapper *succeededMapper_;
};

#endif // CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H
