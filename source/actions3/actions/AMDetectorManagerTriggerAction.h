#ifndef AMDETECTORMANAGERTRIGGERACTION_H
#define AMDETECTORMANAGERTRIGGERACTION_H

#include <QSignalMapper>

#include "actions3/AMAction3.h"
#include "actions3/actions/AMDetectorManagerTriggerActionInfo.h"

#define AMDETECTORMANAGERTRIGGERACTION_INVALID_MANAGER 38294
#define AMDETECTORMANAGERTRIGGERACTION_MANAGER_CANNOT_ARM 38295
#define AMDETECTORMANAGERTRIGGERACTION_INVALID_READ_MODE 38296
#define AMDETECTORMANAGERTRIGGERACTION_TRIGGER_FAILED 38297
#define AMDETECTORMANAGERTRIGGERACTION_MANAGER_CANNOT_TRIGGER 38298


class AMControl;
class AMDetector;
class AMDetectorManager;

class AMDetectorManagerTriggerAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMDetectorManagerTriggerAction(AMDetectorManagerTriggerActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	AMDetectorManagerTriggerAction(const AMDetectorManagerTriggerAction &original);
	/// Destructor.
	virtual ~AMDetectorManagerTriggerAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new AMDetectorManagerTriggerAction(*this); }

	/// Returns the detector manager this action will arm.
	AMDetectorManager* detectorManager() const { return detectorManager_; }

	/// Returns true if this action supports pausing (it does not).
	virtual bool canPause() const { return false; }
	/// Returns true if this action supports skipping (it does not).
	virtual bool canSkip() const { return false; }

	/// Returns true if this action has any child actions, false otherwise.
	virtual bool hasChildren() const { return false; }
	/// Returns the number of child actions.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the action info specific for this action.
	const AMDetectorManagerTriggerActionInfo* detectorManagerTriggerInfo() const { return qobject_cast<const AMDetectorManagerTriggerActionInfo*>(info()); }
	/// Returns the action info specific for this action.
	AMDetectorManagerTriggerActionInfo* detectorManagerTriggerInfo() { return qobject_cast<AMDetectorManagerTriggerActionInfo*>(info()); }

protected slots:
	/// Sets the detector manager.
	void setDetectorManager(AMControl *newManager);

	/// Sets the triggered status.
	void setTriggered(bool isTriggered);
	/// Updates the triggered status.
	void updateTriggered();

	/// Handles reporting that this action has started when the arming action has started.
	void onArmActionStarted();
	/// Handles reporting that this action has cancelled when the arming action reports as cancelled.
	void onArmActionCancelled();
	/// Handles reporting that this action has failed when the arming action reports as failed.
	void onArmActionFailed();
	/// Handles initiating the acquisition when the arming action reports as succeeded.
	void onArmActionSucceeded();

	/// Handles updating the triggered status when a detector reports as triggered.
	void onDetectorTriggered(QObject *detectorObject);
	/// Handles updating the triggered status when a detector manager reports as triggered.
	void onDetectorManagerTriggered(QObject *managerObject);

protected:
	/// Returns true if the given read mode is supported, false otherwise.
	bool supportedReadMode(int readMode) const;

	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action.
	virtual void pauseImplementation() { setPaused(); }
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action.
	virtual void resumeImplementation() { setResumed(); }
	/// All implementations must support cancelling. This function will be called from the Cancelling state when the implementation should cancel the action.
	virtual void cancelImplementation() { setCancelled(); }
	/// The function is called from the Skipping state when the implementation should skip the action. This implementation does not support skipping.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

protected:
	/// The detector manager.
	AMDetectorManager *detectorManager_;

	/// The list of detectors the detector manager is responsible for.
	QList<AMDetector*> detectors_;
	/// The list of detector managers the detector manager is responsible for.
	QList<AMDetectorManager*> detectorManagers_;

	/// The triggered status.
	bool triggered_;
	/// The detectors triggered status.
	bool detectorsTriggered_;
	/// The detector managers triggered status.
	bool detectorManagersTriggered_;

	/// The list of triggered detectors.
	QList<AMDetector*> triggeredDetectors_;
	/// The list of triggered detector managers.
	QList<AMDetectorManager*> triggeredDetectorManagers_;

	/// The detector triggering signal mapper.
	QSignalMapper *detectorTriggeringMapper_;
	/// The detector manager triggering signal mapper.
	QSignalMapper *detectorManagerTriggeringMapper_;
};

#endif // AMDETECTORMANAGERTRIGGERACTION_H
