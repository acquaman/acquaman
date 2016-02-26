#ifndef BIOXASZEBRAMODIFYDETECTORSACTIONINFO_H
#define BIOXASZEBRAMODIFYDETECTORSACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class BioXASZebraModifyDetectorsActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(int action READ dbReadAction WRITE dbLoadAction)
	Q_PROPERTY(AMDbObject* detectorInfo READ dbReadDetectorInfo WRITE dbLoadDetectorInfo)

public:
	/// Enumeration of the different ways to modify the detectors.
	enum Action { AddDetector = 0, RemoveDetector = 1, ClearDetectors = 2 };

	/// Constructor.
	explicit BioXASZebraModifyDetectorsActionInfo(BioXASZebraModifyDetectorsActionInfo::Action action = ClearDetectors, const AMDetectorInfo &info = AMDetectorInfo(), QObject *parent = 0);
	/// Copy constructor.
	BioXASZebraModifyDetectorsActionInfo(const BioXASZebraModifyDetectorsActionInfo &original);
	/// Destructor.
	virtual ~BioXASZebraModifyDetectorsActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns a description of this action.
	virtual QString typeDescription() const { return "Modify Zebra detectors"; }

	/// Returns the modification action.
	int action() const { return action_; }
	/// Returns the detector info.
	const AMDetectorInfo& detectorInfo() const { return detectorInfo_; }

	/// Returns true if the given action is a valid action, false otherwise.
	bool validAction(BioXASZebraModifyDetectorsActionInfo::Action action) const;

signals:

public slots:
	/// Sets the modification action.
	void setAction(BioXASZebraModifyDetectorsActionInfo::Action newAction);
	/// Sets the detector info.
	void setDetectorInfo(const AMDetectorInfo &newInfo);

protected slots:
	/// Updates the description text.
	void updateDescriptionText();

protected:
	/// For database storing of the action only.
	int dbReadAction() { return action_; }
	/// For database loading of the action only.
	void dbLoadAction(int action) { action_ = action; }
	/// For database storing of the detector info only.
	AMDetectorInfo* dbReadControlInfo() { return &detectorInfo_; }
	/// For database loading of the detector info only.
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The modification action.
	int action_;
	/// The detector involved.
	AMDetectorInfo detectorInfo_;
};

#endif // BIOXASZEBRAMODIFYDETECTORSACTIONINFO_H
