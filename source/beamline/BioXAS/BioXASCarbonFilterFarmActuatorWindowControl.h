#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

// timeout.

#define TIMEOUT_MOVE 10

// error codes.

#define BIOXAS_FILTER_FARM_ACTUATOR_NOT_CONNECTED 773981
#define BIOXAS_FILTER_FARM_ACTUATOR_ALREADY_MOVING 773982
#define BIOXAS_FILTER_FARM_ACTUATOR_INVALID_SETPOINT 773983
#define BIOXAS_FILTER_FARM_ACTUATOR_MOVE_FAILED 773984

class BioXASCarbonFilterFarmActuatorWindowControl : public BioXASCarbonFilterFarmActuatorControl
{
	Q_OBJECT

public:
	/// Enum describing the possible active windows, the value for this control.
	enum Window { Invalid = 0, None, Bottom, Top };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(AMControl *positionControl, AMControl *statusControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowControl();

	/// Returns the current active window.
	virtual double value() const { return value_; }
	/// Returns the desired active window setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns the smallest value this control can take.
	virtual double minimumValue() const;
	/// Returns the largest value this control can take.
	virtual double maximumValue() const;

	/// Returns true if the active window is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new active window is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }

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

	/// Returns a string representation of the given window value.
	static QString windowToString(Window window);

	/// Returns the window corresponding to the given position. Returns Window::None if no window found.
	Window windowAtPosition(double position) const;
	/// Returns the position of the given window. Returns 0 if no position was found.
	double positionOfWindow(Window window) const;

public slots:
	/// Sets a window to position mapping.
	void setWindowPosition(Window window, double position);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint. Returns 0 if unable to move or if the setpoint is invalid.
	virtual AMAction3* createMoveAction(double windowSetpoint);

protected:
	/// The mapping between window enum and physical window position.
	QMap<Window, double> positionMap_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
