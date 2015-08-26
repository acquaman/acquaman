#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

class BioXASCarbonFilterFarmActuatorWindowControl : public BioXASCarbonFilterFarmControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, const QString &units, BioXASCarbonFilterFarmActuatorControl *actuatorControl, QObject *parent = 0);
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

	/// Returns the actuator control.
	AMControl* actuatorControl() { return actuator_; }

	/// Returns a string representation of the given window value.
	static QString windowToString(BioXASCarbonFilterFarm::Actuator::Window window);

	/// Returns the window corresponding to the given position. Returns Window::None if no window found.
	BioXASCarbonFilterFarm::Actuator::Window windowAtPosition(double position) const;
	/// Returns the position of the given window. Returns 0 if no position was found.
	double positionOfWindow(BioXASCarbonFilterFarm::Actuator::Window window) const;

signals:
	/// Notifier that the actuator control has changed.
	void actuatorControlChanged(AMControl *newControl);

public slots:
	/// Sets the actuator control.
	void setActuatorControl(BioXASCarbonFilterFarmActuatorControl *newControl);
	/// Sets a window to position mapping.
	void setWindowPosition(BioXASCarbonFilterFarm::Actuator::Window window, double position);

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
	/// The actuator control.
	BioXASCarbonFilterFarmActuatorControl *actuator_;
	/// The mapping between window enum and physical window position.
	QMap<BioXASCarbonFilterFarm::Actuator::Window, double> positionMap_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
