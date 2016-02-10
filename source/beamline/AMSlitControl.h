#ifndef AMSLITCONTROL_H
#define AMSLITCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class AMSlit;

class AMSlitControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Constructor.
	AMSlitControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSlitControl();

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the upper blade control value.
	double upperBladeValue() const;
	/// Returns the lower blade control value.
	double lowerBladeValue() const;

	/// Returns the upper blade control.
	AMControl* upperBlade() const { return upperBlade_; }
	/// Returns the lower blade control.
	AMControl* lowerBlade() const { return lowerBlade_; }

signals:
	/// Notifier that the upper blade control value has changed.
	void upperBladeValueChanged(double newValue);
	/// Notifier that the upper blade control has changed.
	void upperBladeChanged(AMControl *newControl);
	/// Notifier that the lower blade control value has changed.
	void lowerBladeValueChanged(double newValue);
	/// Notifier that the lower blade control has changed.
	void lowerBladeChanged(AMControl *newControl);

public slots:
	/// Sets the upper blade control.
	void setUpperBlade(AMControl *newControl, double orientation);
	/// Sets the lower blade control.
	void setLowerBlade(AMControl *newControl, double orientation);

protected:
	/// Calculates and returns the gap value from the given blade control values.
	static double calculateGap(double upperBladeValue, double upperBladeOrientation, double lowerBladeValue, double lowerBladeOrientation);
	/// Calculates and returns the center value from the given blade control values.
	static double calculateCenter(double upperBladeValue, double upperBladeOrientation, double lowerBladeValue, double lowerBladeOrientation);

protected:
	/// The upper blade control.
	AMControl *upperBlade_;
	/// The lower blade control.
	AMControl *lowerBlade_;
	/// The blade control orientation map.
	QMap<AMControl*, double> bladeOrientationMap_;
};

#endif // AMSLITCONTROL_H
