#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"

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
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, const QString &units, AMControl *positionControl, AMControl *statusControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowControl();

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns a string representation of the given window value.
	static QString windowToString(BioXASCarbonFilterFarm::Actuator::Window window);

	/// Returns the window corresponding to the given position. Returns Window::None if no window found.
	BioXASCarbonFilterFarm::Actuator::Window windowAtPosition(double position) const;
	/// Returns the position of the given window. Returns 0 if no position was found.
	double positionOfWindow(BioXASCarbonFilterFarm::Actuator::Window window) const;

public slots:
	/// Sets a window to position mapping.
	void setWindowPosition(BioXASCarbonFilterFarm::Actuator::Window window, double position);

protected slots:
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint. Returns 0 if unable to move or if the setpoint is invalid.
	virtual AMAction3* createMoveAction(double windowSetpoint);

protected:
	/// The mapping between window enum and physical window position.
	QMap<BioXASCarbonFilterFarm::Actuator::Window, double> positionMap_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
