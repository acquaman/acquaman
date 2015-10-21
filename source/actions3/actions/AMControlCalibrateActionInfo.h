#ifndef AMCONTROLCALIBRATEACTIONINFO_H
#define AMCONTROLCALIBRATEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

class AMControlCalibrateActionInfo : public AMActionInfo3
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMControlCalibrateActionInfo(const AMControlInfo &control = AMControlInfo(), double oldValue = 0, double newValue = 0, QObject *parent = 0);
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
	double oldValue() const { return oldValue_; }
	/// Returns the control's new info.
	double newValue() const { return newValue_; }

signals:


public slots:
	/// Sets the control info.
	void setControlInfo(const AMControlInfo &newInfo);
	/// Sets the control's old value.
	void setOldValue(double newValue);
	/// Sets the control's new value.
	void setNewValue(double newValue);

protected:
	/// Updates the description.
	void updateDescriptionText();

	/// For database storing only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading only.
	void dbLoadControlInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The control info.
	AMControlInfo controlInfo_;
	/// The control's old value.
	double oldValue_;
	/// The control's new value.
	double newValue_;
};

#endif // AMCONTROLCALIBRATEACTIONINFO_H
