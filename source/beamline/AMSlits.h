#ifndef AMSLITS_H
#define AMSLITS_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/AMSlit.h"
#include "beamline/AMControlSet.h"

class AMSlits : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMSlits(const QString &name, QObject*parent = 0);
	/// Destructor
	virtual ~AMSlits();

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const;

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

	/// Adds a child control. Reimplemented to add the child to the set of all subcontrols.
    virtual bool addChildControl(AMControl *control);
	/// Removes a child control. Reimplemented to remove the child from the set of all subcontrols.
    virtual bool removeChildControl(AMControl *control);

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
	void setUpperBlade(AMControl *newControl, AMSlit::BladeOrientation orientation = AMSlit::OpensNegatively);
	/// Sets the lower blade control.
	void setLowerBlade(AMControl *newControl, AMSlit::BladeOrientation orientation = AMSlit::OpensPositively);
	/// Sets the inboard blade control.
	void setInboardBlade(AMControl *newControl, AMSlit::BladeOrientation orientation = AMSlit::OpensPositively);
	/// Sets the outboard blade control.
	void setOutboardBlade(AMControl *newControl, AMSlit::BladeOrientation orientation = AMSlit::OpensNegatively);

	/// Sets the vertical maximum gap. The 'Open' setpoints for each blade motor are calculated from this value. Opening the slit becomes enabled when this value is set. For blade motors that don't have encoders, this value should be larger than their maximum value.
	void setVerticalSlitOpenGapValue(double openGap);
	/// Removes the vertical maximum gap set.
	void removeVerticalSlitOpenGapValue();

	/// Sets the horizontal maximum gap. The 'Open' setpoints for each blade motor are calculated from this value. Opening the slit becomes enabled when this value is set. For blade motors that don't have encoders, this value should be larger than their maximum value.
	void setHorizontalSlitOpenGapValue(double openGap);
	/// Removes the horizontal maximum gap set.
	void removeHorizontalSlitOpenGapValue();

	/// Sets the vertical minimum gap. The 'Close' setpoints for each blade motor are calculated from this value. Closing the slit becomes enabled when this value is set.
	void setVerticalSlitClosedGapValue(double closeGap);
	/// Removes the vertical minimum gap set.
	void removeVerticalSlitClosedGapValue();

	/// Sets the horizontal minimum gap. The 'Close' setpoints for each blade motor are calculated from this value. Closing the slit becomes enabled when this value is set.
	void setHorizontalSlitClosedGapValue(double closeGap);
	/// Removes the horizontal minimum gap set.
	void removeHorizontalClosedGapValue();

protected:
	/// The set of subcontrols.
	AMControlSet *allControls_;

	/// The vertical slit.
	AMSlit *verticalSlit_;
	/// The horizontal slit.
	AMSlit *horizontalSlit_;
};

#endif // AMSLITS_H
