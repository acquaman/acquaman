#ifndef CLSSIS3820SCALERTRIGGERACTION_H
#define CLSSIS3820SCALERTRIGGERACTION_H

#include <QSignalMapper>

#include "actions3/AMAction3.h"
#include "actions3/CLS/CLSSIS3820ScalerTriggerActionInfo.h"

#define CLSSIS3820SCALERTRIGGERACTION_INVALID_SCALER 23801
#define CLSSIS3820SCALERTRIGGERACTION_INVALID_TRIGGER_SOURCE 23802
#define CLSSIS3820SCALERTRIGGERACTION_INVALID_READ_MODE 23803

class CLSSIS3820ScalerTriggerAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSSIS3820ScalerTriggerAction(CLSSIS3820ScalerTriggerActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	CLSSIS3820ScalerTriggerAction(const CLSSIS3820ScalerTriggerAction &original);
	/// Destructor.
	virtual ~CLSSIS3820ScalerTriggerAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new CLSSIS3820ScalerTriggerAction(*this); }

	/// Returns true if this action can be paused, always false for this action.
	virtual bool canPause() const { return false; }
	/// Returns true if this action can be skipped, always false for this action.
	virtual bool canSkip() const { return false; }

	/// Returns true if this action has child actions, always false for this action.
	virtual bool hasChildren() const { return false; }
	/// Returns the number of child actions, always 0 for this action.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the action info specific for this action.
	const CLSSIS3820ScalerTriggerActionInfo* scalerTriggerInfo() const { return qobject_cast<const CLSSIS3820ScalerTriggerActionInfo*>(info()); }
	/// Returns the action info specific for this action.
	CLSSIS3820ScalerTriggerActionInfo* scalerTriggerInfo() { return qobject_cast<CLSSIS3820ScalerTriggerActionInfo*>(info()); }

protected slots:
	/// Handles emitting the appropriate signals when this action has started.
	void onStarted();
	/// Handles emitting the appropriate signals when this action has failed.
	void onFailed();
	/// Handles emitting the appropriate signals when this action has succeeded.
	void onSucceeded();

protected:
	/// Returns true if the given value is a supported detector read mode, false otherwise.
	bool supportedReadMode(int mode) const;

	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action.
	virtual void pauseImplementation() {}
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action.
	virtual void resumeImplementation() {}
	/// All implementations must support cancelling. This function will be called from the Cancelling state when the implementation should cancel the action.
	virtual void cancelImplementation() { setCancelled(); }
	/// The function is called from the Skipping state when the implementation should skip the action. This implementation does not support skipping.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command) return; }
};

#endif // CLSSIS3820SCALERTRIGGERACTION_H
