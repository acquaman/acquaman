#ifndef AMDETECTORMANAGERARMACTION_H
#define AMDETECTORMANAGERARMACTION_H

#include <QSignalMapper>

#include "actions3/AMAction3.h"
#include "actions3/actions/AMDetectorManagerArmActionInfo.h"

#define AMDETECTORMANAGERARMACTION_INVALID_MANAGER 38292
#define AMDETECTORMANAGERARMACTION_MANAGER_CANNOT_ARM 38293

class AMControl;
class AMDetector;
class AMDetectorManager;

class AMDetectorManagerArmAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMDetectorManagerArmAction(AMDetectorManagerArmActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	AMDetectorManagerArmAction(const AMDetectorManagerArmAction &original);
	/// Destructor.
	virtual ~AMDetectorManagerArmAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new AMDetectorManagerArmAction(*this); }

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
	const AMDetectorManagerArmActionInfo* detectorManagerArmInfo() const { return qobject_cast<const AMDetectorManagerArmActionInfo*>(info()); }
	/// Returns the action info specific for this action.
	AMDetectorManagerArmActionInfo* detectorManagerArmInfo() { return qobject_cast<AMDetectorManagerArmActionInfo*>(info()); }

protected slots:
	/// Sets the detector manager.
	void setDetectorManager(AMControl *newManager);

	/// Sets the armed status.
	void setArmed(bool isArmed);
	/// Updates the armed status.
	void updateArmed();

	/// Handles updating the armed status when a detector reports as armed.
	void onDetectorArmed(QObject *detector);
	/// Handles updating the armed status when a detector manager reports as armed.
	void onDetectorManagerArmed(QObject *manager);

protected:
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

	/// The armed status.
	bool armed_;
	/// The detectors armed status.
	bool detectorsArmed_;
	/// The detector managers armed status.
	bool detectorManagersArmed_;

	/// The list of armed detectors.
	QList<AMDetector*> armedDetectors_;
	/// The list of armed detector managers.
	QList<AMDetectorManager*> armedDetectorManagers_;

	/// The detectors arming signal mapper.
	QSignalMapper *detectorArmingMapper_;
	/// The detector managers arming signal mapper.
	QSignalMapper *detectorManagerArmingMapper_;
};

#endif // AMDETECTORMANAGERARMACTION_H
