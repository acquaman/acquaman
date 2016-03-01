#ifndef AMTRIGGERMANAGERARMACTIONINFO_H
#define AMTRIGGERMANAGERARMACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

class AMTriggerManagerArmActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(AMDbObject* triggerManagerInfo READ dbReadTriggerManagerInfo WRITE dbLoadTriggerManagerInfo)

public:
	/// Constructor.
	explicit AMTriggerManagerArmActionInfo(const AMControlInfo &triggerManagerInfo, QObject *parent = 0);
	/// Copy constructor.
	AMTriggerManagerArmActionInfo(const AMTriggerManagerArmActionInfo &original);
	/// Destructor.
	virtual ~AMTriggerManagerArmActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns the description of this action.
	virtual QString typeDescription() const { return "Arm trigger manager."; }

	/// Returns the trigger manager info.
	const AMControlInfo& triggerManagerInfo() const { return triggerManagerInfo_; }

protected:
	/// Updates the description text.
	void updateDescriptionText();

	/// For database storing of the trigger manager info only.
	AMControlInfo* dbReadTriggerManagerInfo() { return &triggerManagerInfo_; }
	/// For database loading of the trigger manager info only.
	void dbLoadTriggerManagerInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The trigger manager info.
	AMControlInfo triggerManagerInfo_;
};

#endif // AMTRIGGERMANAGERARMACTIONINFO_H
