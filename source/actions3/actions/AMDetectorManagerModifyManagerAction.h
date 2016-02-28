#ifndef AMDETECTORMANAGERMODIFYMANAGERACTION_H
#define AMDETECTORMANAGERMODIFYMANAGERACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMDetectorManagerModifyManagerActionInfo.h"

#define AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_DETECTOR_MANAGER 9820
#define AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_OPTION 9821
#define AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_DETECTOR 9822
#define AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_MANAGER 9823
#define AMDETECTORMANAGERMODIFYMANAGERACTION_MODIFY_FAILED 9824

class AMControl;
class AMDetector;
class AMDetectorManager;

class AMDetectorManagerModifyManagerAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMDetectorManagerModifyManagerAction(AMDetectorManagerModifyManagerActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	AMDetectorManagerModifyManagerAction(const AMDetectorManagerModifyManagerAction &original);
	/// Destructor.
	virtual ~AMDetectorManagerModifyManagerAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new AMDetectorManagerModifyManagerAction(*this); }

	/// Returns the detector manager being modified.
	AMDetectorManager* detectorManager() const { return detectorManager_; }
	/// Returns the detector.
	AMDetector* detector() const { return detector_; }
	/// Returns the manager.
	AMDetectorManager* manager() const { return manager_; }

	/// Returns true if this action supports pausing (it does not).
	virtual bool canPause() const { return false; }
	/// Returns true if this action supports skipping (it does not).
	virtual bool canSkip() const { return false; }

	/// Returns true if this action has any child actions, false otherwise.
	virtual bool hasChildren() const { return false; }
	/// Returns the number of child actions.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the action info specific for this action.
	const AMDetectorManagerModifyManagerActionInfo* modifyManagerInfo() const { return qobject_cast<const AMDetectorManagerModifyManagerActionInfo*>(info()); }
	/// Returns the action info specific for this action.
	AMDetectorManagerModifyManagerActionInfo* modifyManagerInfo() { return qobject_cast<AMDetectorManagerModifyManagerActionInfo*>(info()); }

protected slots:
	/// Sets the detector manager being modified.
	void setDetectorManager(AMDetectorManager *newManager);
	/// Sets the detector.
	void setDetector(AMDetector *newDetector);
	/// Sets the manager.
	void setManager(AMDetectorManager *newManager);

protected:
	/// Returns true if the given modification option is supported.
	bool optionSupported(int option) const;

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
	/// The detector manager being modified.
	AMDetectorManager *detectorManager_;
	/// The detector.
	AMDetector *detector_;
	/// The manager.
	AMDetectorManager *manager_;
};

#endif // AMDETECTORMANAGERMODIFYMANAGERACTION_H
