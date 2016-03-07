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
	/// Handles reporting this action as succeeded, in response to the trigger manager becoming armed.
	void onTriggerManagerArmed();

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
};

#endif // AMTRIGGERMANAGERARMACTION_H
