#ifndef AMSLITS_H
#define AMSLITS_H

#include "beamline/AMControl.h"
#include "beamline/AMSlit.h"

class AMSlits : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMSlits(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSlits();

	/// Returns the upper blade value.
	double upperBladeValue() const;
	/// Returns the lower blade value.
	double lowerBladeValue() const;
	/// Returns the inboard blade value.
	double inboardBladeValue() const;
	/// Returns the outboard blade value.
	double outboardBladeValue() const;

	/// Returs the vertical gap value.
	double verticalGapValue() const;
	/// Returns the vertial center value.
	double verticalCenterValue() const;
	/// Returns the horizontal gap value.
	double horizontalGapValue() const;
	/// Returns the horizontal center value.
	double horizontalCenterValue() const;

	/// Returns the upper blade.
	AMControl* upperBlade() const;
	/// Reutrns the lower blade.
	AMControl* lowerBlade() const;
	/// Returns the inboard blade.
	AMControl* inboardBlade() const;
	/// Returns the outboard blade.
	AMControl* outboardBlade() const;

	/// Returns the vertical gap control.
	AMControl* verticalGap() const;
	/// Returns the vertical center control.
	AMControl* verticalCenter() const;
	/// Returns the horizontal gap control.
	AMControl* horizontalGap() const;
	/// Returns the horizontal center control.
	AMControl* horizontalCenter() const;

	/// Returns the vertical slit.
	AMSlit* verticalSlit() const { return verticalSlit_; }
	/// Returns the horizontal slit.
	AMSlit* horizontalSlit() const { return horizontalSlit_; }

signals:
	/// Notifier that the upper blade value has changed.
	void upperBladeValueChanged(double newValue);
	/// Notifier that the upper blade control has changed.
	void upperBladeChanged(AMControl *newControl);
	/// Notifier that the lower blade value has changed.
	void lowerBladeValueChanged(double newValue);
	/// Notifier that the lower blade control has changed.
	void lowerBladeChanged(AMControl *newControl);
	/// Notifier that the inboard blade value has changed.
	void inboardBladeValueChanged(double newValue);
	/// Notifier that the inboard blad control has changed.
	void inboardBladeChanged(AMControl *newControl);
	/// Notifier that the outboard blade value has changed.
	void outboardBladeValueChanged(double newValue);
	/// Notifier that the outboard blade control has changed.
	void outboardBladeChanged(AMControl *newControl);
	/// Notifier that the vertical gap value has changed.
	void verticalGapValueChanged(double newValue);
	/// Notifier that the vertical center value has changed.
	void verticalCenterValueChanged(double newValue);
	/// Notifier that the horizontal gap value changed.
	void horizontalGapValueChanged(double newValue);
	/// Notifier that the horizontal center value changed.
	void horizontalCenterValueChanged(double newValue);

public slots:
	/// Sets the upper blade control.
	void setUpperBlade(AMControl *newControl, AMSlit::BladeOrientation orientation);
	/// Sets the lower blade control.
	void setLowerBlade(AMControl *newControl, AMSlit::BladeOrientation orientation);
	/// Sets the inboard blade control.
	void setInboardBlade(AMControl *newControl, AMSlit::BladeOrientation orientation);
	/// Sets the outboard blade control.
	void setOutboardBlade(AMControl *newControl, AMSlit::BladeOrientation orientation);

protected:
	/// The vertical slit.
	AMSlit *verticalSlit_;
	/// The horizontal slit.
	AMSlit *horizontalSlit_;
};

#endif // AMSLITS_H
