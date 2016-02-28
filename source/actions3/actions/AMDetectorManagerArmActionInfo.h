#ifndef AMDETECTORMANAGERARMACTIONINFO_H
#define AMDETECTORMANAGERARMACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

class AMDetectorManagerArmActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(AMDbObject* detectorManagerInfo READ dbReadDetectorManagerInfo WRITE dbLoadDetectorManagerInfo)

public:
	/// Constructor.
	explicit AMDetectorManagerArmActionInfo(const AMControlInfo &detectorManagerInfo, QObject *parent = 0);
	/// Copy constructor.
	AMDetectorManagerArmActionInfo(const AMDetectorManagerArmActionInfo &original);
	/// Destructor.
	virtual ~AMDetectorManagerArmActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns the description of this action.
	virtual QString typeDescription() const { return "Arm detector manager."; }

	/// Returns the detector manager info.
	const AMControlInfo& detectorManagerInfo() const { return detectorManagerInfo_; }

signals:

public slots:
	/// Sets the detector manager info.
	void setDetectorManagerInfo(const AMControlInfo &newInfo);

protected:
	/// Updates the description text.
	void updateDescriptionText();

	/// For database storing of the detector manager info only.
	AMControlInfo* dbReadDetectorManagerInfo() { return &detectorManagerInfo_; }
	/// For database loading of the detector manager info only.
	void dbLoadDetectorManagerInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The detector manager info.
	AMControlInfo detectorManagerInfo_;
};

#endif // AMDETECTORMANAGERARMACTIONINFO_H
