#ifndef AMTRIGGERMANAGERTRIGGERACTIONINFO_H
#define AMTRIGGERMANAGERTRIGGERACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"
#include "dataman/info/AMDetectorInfo.h"

class AMTriggerManagerTriggerActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(AMDbObject* triggerManagerInfo READ dbReadTriggerManagerInfo WRITE dbLoadTriggerManagerInfo)
	Q_PROPERTY(int readMode READ dbReadReadMode WRITE dbLoadReadMode)

public:
	/// Constructor.
	explicit AMTriggerManagerTriggerActionInfo(const AMControlInfo &triggerManagerInfo, AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead, QObject *parent = 0);
	/// Copy constructor.
	AMTriggerManagerTriggerActionInfo(const AMTriggerManagerTriggerActionInfo &original);
	/// Destructor.
	virtual ~AMTriggerManagerTriggerActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns the description of this action.
	virtual QString typeDescription() const { return "Triggers the trigger manager."; }

	/// Returns the trigger manager info.
	const AMControlInfo& triggerManagerInfo() const { return triggerManagerInfo_; }
	/// Returns the read mode.
	int readMode() const { return readMode_; }

protected:
	/// Returns true if the given mode is a supported read mode, false otherwise.
	bool supportedReadMode(int mode) const;
	/// Returns a string representation of the given read mode.
	QString readModeToString(int mode) const;

	/// Updates the description text.
	void updateDescriptionText();

	/// For database storing of the trigger manager info only.
	AMControlInfo* dbReadTriggerManagerInfo() { return &triggerManagerInfo_; }
	/// For database loading of the trigger manager info only.
	void dbLoadTriggerManagerInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }
	/// For database storing of the read mode only.
	int dbReadReadMode() { return readMode_; }
	/// For database loading of the read mode only.
	void dbLoadReadMode(int readMode) { readMode_ = readMode; }

protected:
	/// The trigger manager info.
	AMControlInfo triggerManagerInfo_;
	/// The read mode.
	int readMode_;
};

#endif // AMTRIGGERMANAGERTRIGGERACTIONINFO_H
