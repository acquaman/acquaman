#ifndef AMTRIGGERMANAGERARMACTION_H
#define AMTRIGGERMANAGERARMACTION_H

#include <QSignalMapper>

#include "actions3/AMAction3.h"
#include "actions3/actions/AMTriggerManagerArmActionInfo.h"

#define AMTRIGGERMANAGERARMACTION_INVALID_MANAGER 38292
#define AMTRIGGERMANAGERARMACTION_MANAGER_CANNOT_ARM 38293

class AMControl;
class AMDetector;
class AMTriggerManager;

class AMTriggerManagerArmAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMTriggerManagerArmAction(AMTriggerManagerArmActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	AMTriggerManagerArmAction(const AMTriggerManagerArmAction &original);
	/// Destructor.
	virtual ~AMTriggerManagerArmAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new AMTriggerManagerArmAction(*this); }

	/// Returns the trigger manager this action will arm.
	AMTriggerManager* triggerManager() const { return triggerManager_; }

	/// Returns true if this action supports pausing (it does not).
	virtual bool canPause() const { return false; }
	/// Returns true if this action supports skipping (it does not).
	virtual bool canSkip() const { return false; }

	/// Returns true if this action has any child actions, false otherwise.
	virtual bool hasChildren() const { return false; }
	/// Returns the number of child actions.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the action info specific for this action.
	const AMTriggerManagerArmActionInfo* triggerManagerArmInfo() const { return qobject_cast<const AMTriggerManagerArmActionInfo*>(info()); }
	/// Returns the action info specific for this action.
	AMTriggerManagerArmActionInfo* triggerManagerArmInfo() { return qobject_cast<AMTriggerManagerArmActionInfo*>(info()); }

protected slots:
	/// Sets the armed status.
	void setArmed(bool isArmed);
	/// Updates the armed status.
	void updateArmed();

	/// Handles updating the armed status when a detector reports as armed.
	void onDetectorArmed(QObject *detector);
	/// Handles updating the armed status when a trigger manager reports as armed.
	void onTriggerManagerArmed(QObject *manager);

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
	/// The trigger manager.
	AMTriggerManager *triggerManager_;

	/// The list of detectors the trigger manager is responsible for.
	QList<AMDetector*> detectors_;
	/// The list of trigger managers the trigger manager is responsible for.
	QList<AMTriggerManager*> triggerManagers_;

	/// The armed status.
	bool armed_;
	/// The detectors armed status.
	bool detectorsArmed_;
	/// The trigger managers armed status.
	bool triggerManagersArmed_;

	/// The list of armed detectors.
	QList<AMDetector*> armedDetectors_;
	/// The list of armed trigger managers.
	QList<AMTriggerManager*> armedTriggerManagers_;

	/// The detectors arming signal mapper.
	QSignalMapper *detectorArmingMapper_;
	/// The trigger managers arming signal mapper.
	QSignalMapper *triggerManagerArmingMapper_;
};

#endif // AMTRIGGERMANAGERARMACTION_H
