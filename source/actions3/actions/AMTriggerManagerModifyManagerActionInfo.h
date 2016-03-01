#ifndef AMTRIGGERMANAGERMODIFYMANAGERACTIONINFO_H
#define AMTRIGGERMANAGERMODIFYMANAGERACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"
#include "dataman/info/AMDetectorInfo.h"

class AMTriggerManagerModifyManagerActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(AMDbObject* triggerManagerInfo READ dbReadTriggerManagerInfo WRITE dbLoadTriggerManagerInfo)
	Q_PROPERTY(int option READ dbReadOption WRITE dbLoadOption)
	Q_PROPERTY(AMDbObject* detectorInfo READ dbReadDetectorInfo WRITE dbLoadDetectorInfo)
	Q_PROPERTY(AMDbObject* managerInfo READ dbReadManagerInfo WRITE dbLoadManagerInfo)

public:
	/// Enumeration of the supported detector modifications.
	enum Option { AddDetector = 0, RemoveDetector = 1, ClearDetectors = 2, AddManager = 3, RemoveManager = 4, ClearManagers = 5 };

	/// Constructor.
	explicit AMTriggerManagerModifyManagerActionInfo(const AMControlInfo &triggerManagerInfo = AMControlInfo(), AMTriggerManagerModifyManagerActionInfo::Option option = ClearDetectors, const AMDetectorInfo &detectorInfo = AMDetectorInfo(), const AMControlInfo &managerInfo = AMControlInfo(), QObject *parent = 0);
	/// Copy constructor.
	AMTriggerManagerModifyManagerActionInfo(const AMTriggerManagerModifyManagerActionInfo &original);
	/// Destructor.
	virtual ~AMTriggerManagerModifyManagerActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns the type description.
	QString typeDescription() const { return "Modify trigger manager."; }

	/// Returns the trigger manager info.
	const AMControlInfo& triggerManagerInfo() const { return triggerManagerInfo_; }
	/// Returns the modification option.
	int option() const { return option_; }
	/// Returns the detector info.
	const AMDetectorInfo& detectorInfo() const { return detectorInfo_; }
	/// Returns the manager info.
	const AMControlInfo& managerInfo() const { return managerInfo_; }

protected slots:
	/// Updates the description text.
	void updateDescriptionText();

protected:
	/// For database storing of the trigger manager info only.
	AMControlInfo* dbReadTriggerManagerInfo() { return &triggerManagerInfo_; }
	/// For database loading of the trigger manager info only.
	void dbLoadTriggerManagerInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }
	/// For database storing of the option only.
	int dbReadOption() { return option_; }
	/// For database loading of the option only.
	void dbLoadOption(int option) { option_ = option; }
	/// For database storing of the detector info only.
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading of the detector info only.
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }
	/// For database storing of the manager info only.
	AMControlInfo* dbReadManagerInfo() { return &managerInfo_; }
	/// For database loading of the manager info only.
	void dbLoadManagerInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The trigger manager info.
	AMControlInfo triggerManagerInfo_;
	/// The modification option.
	int option_;
	/// The detector info.
	AMDetectorInfo detectorInfo_;
	/// The manager info.
	AMControlInfo managerInfo_;
};

#endif // AMTRIGGERMANAGERMODIFYMANAGERACTIONINFO_H
