#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class BioXASCarbonFilterFarmActuatorWindowControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Enum describing the actuator windows.
	enum Value { Top = 0, Bottom = 1, None = 2, Invalid = 3 };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns the actuator position control.
	AMControl* positionControl() { return actuatorPosition_; }

	/// Returns a string representation of the given window value.
	static QString windowToString(double window);

	/// Returns the window corresponding to the given position. Returns Window::None if no window found.
	double windowAtPosition(double position) const;
	/// Returns the position of the given window. Returns 0 if no position was found.
	double positionOfWindow(double window) const;

signals:
	/// Notifier that the actuator control has changed.
	void actuatorPositionChanged(AMControl *newControl);

public slots:
	/// Sets the actuator control.
	void setActuatorPosition(AMControl *newControl);
	/// Sets a window to position mapping.
	void setWindowPosition(double window, double minPosition, double maxPosition);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint. Returns 0 if unable to move or if the setpoint is invalid.
	virtual AMAction3* createMoveAction(double windowSetpoint);

protected:
	/// The actuator position control.
	AMControl *actuatorPosition_;

	/// The mapping between window enum and physical window position (min value).
	QMap<double, double> windowMinPositionMap_;
	/// The mapping between window enum and physical window position (max value).
	QMap<double, double> windowMaxPositionMap_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
