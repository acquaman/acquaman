#ifndef BIOXASZEBRAMODIFYDETECTORSACTIONINFO_H
#define BIOXASZEBRAMODIFYDETECTORSACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class BioXASZebraModifyDetectorsActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(int option READ dbReadOption WRITE dbLoadOption)
	Q_PROPERTY(AMDbObject* detectorInfo READ dbReadDetectorInfo WRITE dbLoadDetectorInfo)

public:
	/// Enumeration of the different ways to modify the detectors.
	enum Option { AddDetector = 0, RemoveDetector = 1, ClearDetectors = 2 };

	/// Constructor.
	Q_INVOKABLE BioXASZebraModifyDetectorsActionInfo(BioXASZebraModifyDetectorsActionInfo::Option option = ClearDetectors, const AMDetectorInfo &info = AMDetectorInfo(), QObject *parent = 0);
	/// Copy constructor.
	BioXASZebraModifyDetectorsActionInfo(const BioXASZebraModifyDetectorsActionInfo &original);
	/// Destructor.
	virtual ~BioXASZebraModifyDetectorsActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns a description of this action.
	virtual QString typeDescription() const { return "Modify Zebra detectors"; }

	/// Returns the modification option.
	int option() const { return option_; }
	/// Returns the detector info.
	const AMDetectorInfo& detectorInfo() const { return detectorInfo_; }

protected slots:
	/// Updates the description text.
	void updateDescriptionText();

protected:
	/// For database storing of the action only.
	int dbReadOption() { return option_; }
	/// For database loading of the action only.
	void dbLoadOption(int action) { option_ = action; }
	/// For database storing of the detector info only.
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading of the detector info only.
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The modification option.
	int option_;
	/// The detector involved.
	AMDetectorInfo detectorInfo_;
};

#endif // BIOXASZEBRAMODIFYDETECTORSACTIONINFO_H
