#ifndef AMCONTROLCALIBRATEACTIONINFO_H
#define AMCONTROLCALIBRATEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"
#include "dataman/AMNumber.h"

class AMControlCalibrateActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)
	Q_PROPERTY(double oldValue READ dbReadOldValue WRITE dbLoadOldValue)
	Q_PROPERTY(double newValue READ dbReadNewValue WRITE dbLoadNewValue)
public:
	/// Constructor.
	explicit AMControlCalibrateActionInfo(const AMControlInfo &control = AMControlInfo(), const AMNumber &oldValue = AMNumber::InvalidError, const AMNumber &newValue = AMNumber::InvalidError, QObject *parent = 0);
	/// Copy constructor.
	AMControlCalibrateActionInfo(const AMControlCalibrateActionInfo &original);
	/// Destructor.
	virtual ~AMControlCalibrateActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns a description of this action.
	virtual QString typeDescription() const { return "Control Calibrate"; }

	/// Returns the control info.
	const AMControlInfo& controlInfo() const { return controlInfo_; }

	/// Returns the control's old value.
	AMNumber oldValue() const { return oldValue_; }
	/// Returns the control's new info.
	AMNumber newValue() const { return newValue_; }

	/// Returns true if the old calibration value is valid, false otherwise.
	bool oldValueValid() const { return (oldValue_.isValid()); }
	/// Returns true if the new calibration value is valid, false otherwise.
	bool newValueValid() const { return (newValue_.isValid()); }

signals:

public slots:
	/// Sets the control info.
	void setControlInfo(const AMControlInfo &newInfo);
	/// Sets the control's old value.
	void setOldValue(const AMNumber &newValue);
	/// Sets the control's new value.
	void setNewValue(const AMNumber &newValue);

protected:
	/// Updates the description.
	void updateDescriptionText();

	/// For database storing of the control info only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading of the control info only.
	void dbLoadControlInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }
	/// For database storing of the old value only.
	double dbReadOldValue() { return double(oldValue_); }
	/// For database loading of the old value only.
	void dbLoadOldValue(double oldValue) { oldValue_ = AMNumber(oldValue); }
	/// For database storing of the new value only.
	double dbReadNewValue() { return double(newValue_); }
	/// For database loading of the new value only.
	void dbLoadNewValue(double newValue) { newValue_ = AMNumber(newValue); }

protected:
	/// The control info.
	AMControlInfo controlInfo_;
	/// The control's old value.
	AMNumber oldValue_;
	/// The control's new value.
	AMNumber newValue_;
};

#endif // AMCONTROLCALIBRATEACTIONINFO_H
