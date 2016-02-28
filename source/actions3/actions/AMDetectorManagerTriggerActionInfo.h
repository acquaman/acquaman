#ifndef AMDETECTORMANAGERTRIGGERACTIONINFO_H
#define AMDETECTORMANAGERTRIGGERACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorManagerTriggerActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(AMDbObject* detectorManagerInfo READ dbReadDetectorManagerInfo WRITE dbLoadDetectorManagerInfo)

public:
	/// Constructor.
	explicit AMDetectorManagerTriggerActionInfo(const AMControlInfo &detectorManagerInfo, AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead, QObject *parent = 0);
	/// Copy constructor.
	AMDetectorManagerTriggerActionInfo(const AMDetectorManagerTriggerActionInfo &original);
	/// Destructor.
	virtual ~AMDetectorManagerTriggerActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns the description of this action.
	virtual QString typeDescription() const { return "Trigger detector manager."; }

	/// Returns the detector manager info.
	const AMControlInfo& detectorManagerInfo() const { return detectorManagerInfo_; }
	/// Returns the read mode.
	int readMode() const { return readMode_; }

signals:

public slots:
	/// Sets the detector manager info.
	void setDetectorManagerInfo(const AMControlInfo &newInfo);
	/// Sets the read mode.
	void setReadMode(AMDetectorDefinitions::ReadMode newMode);

protected:
	/// Returns true if the given mode is a supported read mode, false otherwise.
	bool supportedReadMode(int mode) const;
	/// Returns a string representation of the given read mode.
	QString readModeToString(int mode) const;

	/// Updates the description text.
	void updateDescriptionText();

	/// For database storing of the detector manager info only.
	AMControlInfo* dbReadDetectorManagerInfo() { return &detectorManagerInfo_; }
	/// For database loading of the detector manager info only.
	void dbLoadDetectorManagerInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }
	/// For database storing of the read mode only.
	int dbReadReadMode() { return readMode_; }
	/// For database loading of the read mode only.
	void dbLoadReadMode(int readMode) { readMode_ = readMode; }

protected:
	/// The detector manager info.
	AMControlInfo detectorManagerInfo_;
	/// The read mode.
	int readMode_;
};

#endif // AMDETECTORMANAGERTRIGGERACTIONINFO_H
