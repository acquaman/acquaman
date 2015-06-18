#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROL_H

#include <QSignalMapper>

#include "beamline/AMPVControl.h"
#include "beamline/AMPseudoMotorControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

// timeout.

#define TIMEOUT_MOVE 10

// error codes.

#define BIOXAS_FILTER_FARM_ACTUATOR_NOT_CONNECTED 773981
#define BIOXAS_FILTER_FARM_ACTUATOR_ALREADY_MOVING 773982
#define BIOXAS_FILTER_FARM_ACTUATOR_INVALID_SETPOINT 773983
#define BIOXAS_FILTER_FARM_ACTUATOR_MOVE_FAILED 773984

class BioXASCarbonFilterFarm;

class BioXASCarbonFilterFarmActuatorControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorControl(AMControl *positionControl, AMControl *statusControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorControl();

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

	/// Returns the position control.
	AMControl* positionControl() const { return position_; }
	/// Returns the position status control.
	AMControl* statusControl() const { return status_; }

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns a string representation of the given window value.
	static QString windowToString(double value);
	/// Returns the filter corresponding to the given string respresentation. Returns Window::None if no filter found.
	static double stringToWindow(const QString &string);

	/// Returns the window corresponding to the given position. Returns Window::None if no window found.
	double windowAtPosition(double position) const;
	/// Returns the position of the given window. Returns 0 if no position was found.
	double positionOfWindow(double window) const;

public slots:
	/// Sets a window to position mapping.
	void setWindowPosition(double window, double position);
	/// Sets the position control.
	void setPositionControl(AMControl *newControl);
	/// Sets the status control.
	void setStatusControl(AMControl *newControl);

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
	QMap<double, double> positionMap_;

	/// The position control.
	AMControl *position_;
	/// The position status control.
	AMControl *status_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROL_H
