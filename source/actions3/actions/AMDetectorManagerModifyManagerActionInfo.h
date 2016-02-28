#ifndef AMDETECTORMANAGERMODIFYMANAGERACTIONINFO_H
#define AMDETECTORMANAGERMODIFYMANAGERACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorManagerModifyManagerActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(AMDbObject* detectorManagerInfo READ dbReadDetectorManagerInfo WRITE dbLoadDetectorManagerInfo)
	Q_PROPERTY(int option READ dbReadOption WRITE dbLoadOption)
	Q_PROPERTY(AMDbObject* detectorInfo READ dbReadDetectorInfo WRITE dbLoadDetectorInfo)
	Q_PROPERTY(AMDbObject* managerInfo READ dbReadManagerInfo WRITE dbLoadManagerInfo)

public:
	/// Enumeration of the supported detector modifications.
	enum Option { AddDetector = 0, RemoveDetector = 1, ClearDetectors = 2, AddManager = 3, RemoveManager = 4, ClearManagers = 5 };

	/// Constructor.
	explicit AMDetectorManagerModifyManagerActionInfo(const AMControlInfo &detectorManagerInfo = AMControlInfo(), AMDetectorManagerModifyManagerActionInfo::Option option = ClearDetectors, const AMDetectorInfo &detectorInfo = AMDetectorInfo(), const AMControlInfo &managerInfo = AMControlInfo(), QObject *parent = 0);
	/// Copy constructor.
	AMDetectorManagerModifyManagerActionInfo(const AMDetectorManagerModifyManagerActionInfo &original);
	/// Destructor.
	virtual ~AMDetectorManagerModifyManagerActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns the type description.
	QString typeDescription() const { return "Modify detector manager."; }

	/// Returns the detector manager info.
	const AMControlInfo& detectorManagerInfo() const { return detectorManagerInfo_; }
	/// Returns the modification option.
	int option() const { return option_; }
	/// Returns the detector info.
	const AMDetectorInfo& detectorInfo() const { return detectorInfo_; }
	/// Returns the manager info.
	const AMControlInfo& managerInfo() const { return managerInfo_; }

public slots:
	/// Sets the detector manager info.
	void setDetectorManagerInfo(const AMControlInfo &newInfo);
	/// Sets the modification option.
	void setOption(AMDetectorManagerModifyManagerActionInfo::Option newOption);
	/// Sets the detector info.
	void setDetectorInfo(const AMDetectorInfo &newInfo);
	/// Stets the manager info.
	void setManagerInfo(const AMControlInfo &newInfo);

protected slots:
	/// Updates the description text.
	void updateDescriptionText();

protected:
	/// For database storing of the detector manager info only.
	AMControlInfo* dbReadDetectorManagerInfo() { return &detectorManagerInfo_; }
	/// For database loading of the detector manager info only.
	void dbLoadDetectorManagerInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }
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
	/// The detector manager info.
	AMControlInfo detectorManagerInfo_;
	/// The modification option.
	int option_;
	/// The detector info.
	AMDetectorInfo detectorInfo_;
	/// The manager info.
	AMControlInfo managerInfo_;
};

#endif // AMDETECTORMANAGERMODIFYMANAGERACTIONINFO_H
