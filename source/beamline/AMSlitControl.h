#ifndef AMSLITCONTROL_H
#define AMSLITCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class AMSlitControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Constructor.
	AMSlitControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSlitControl();

	/// Returns true if this control should be able to be measurable, false otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if this control should be able to be movable, false otherwise.
	virtual bool shouldMove() const { return true; }

	/// Returns true if this control is measurable right now, false otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control is movable right now, false otherwise.
	virtual bool canMove() const;

	/// Returns the first blade control's orientation.
	double firstBladeOrientation() const;
	/// Returns the second blade control's orientation.
	double secondBladeOrientation() const;

	/// Returns the first blade control.
	AMControl* firstBlade() const { return firstBlade_; }
	/// Returns the second blade control.
	AMControl* secondBlade() const { return secondBlade_; }

signals:
	/// Notifier that the first blade control has changed.
	void firstBladeChanged(AMControl *newControl);
	/// Notifier that the second blade control has changed.
	void secondBladeChanged(AMControl *newControl);

public slots:
	/// Sets the first blade control.
	void setFirstBlade(AMControl *newControl, double orientation);
	/// Sets the second blade control.
	void setSecondBlade(AMControl *newControl, double orientation);

	/// Removes the first blade control.
	void removeFirstBlade();
	/// Removes the second blade control.
	void removeSecondBlade();

protected slots:
	/// Updates the moving state.
	virtual void updateMoving();
	/// Updates the units.
	virtual void updateUnits();
	/// Updates the tolerance.
	virtual void updateTolerance();

protected:
	/// Removes the given blade control.
	bool removeBlade(AMControl *control);

	/// Returns the current gap value.
	virtual double currentGap() const;
	/// Returns the current center value.
	virtual double currentCenter() const;

	/// Calculates and returns the gap value from the given blade control values.
	virtual double calculateGap(double firstBladeValue, double firstBladeOrientation, double secondBladeValue, double secondBladeOrientation) const;
	/// Calculates and returns the center value from the given blade control values.
	virtual double calculateCenter(double firstBladeValue, double firstBladeOrientation, double secondBladeValue, double secondBladeOrientation) const;

	/// Calculates and returns the blade value for the given gap and center.
	virtual double calculateBladeValue(double bladeOrientation, double gap, double center) const;

protected:
	/// The first blade control.
	AMControl *firstBlade_;
	/// The second blade control.
	AMControl *secondBlade_;
	/// The blade control orientation map.
	QMap<AMControl*, double> bladeOrientationMap_;
};

#endif // AMSLITCONTROL_H
