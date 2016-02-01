#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"


class BioXASCarbonFilterFarm : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the upstream actuator control.
	BioXASCarbonFilterFarmActuator* upstreamActuator() const { return upstreamActuator_; }
	/// Returns the downstream actuator control.
	BioXASCarbonFilterFarmActuator* downstreamActuator() const { return downstreamActuator_; }
	/// Returns the filter control.
	BioXASCarbonFilterFarmFilterControl* filter() const { return filter_; }

signals:
	/// Notifier that the actuator motor has changed.
	void actuatorMotorChanged(BioXASCarbonFilterFarmActuator::Location location, CLSMAXvMotor *newControl);
	/// Notifier that the actuator position status has changed.
	void actuatorPositionStatusChanged(BioXASCarbonFilterFarmActuator::Location location, AMControl *newControl);
	/// Notifier that the actuator windows have changed.
	void actuatorWindowsChanged(BioXASCarbonFilterFarmActuator::Location location);
	/// Notifier that the actuator window preferences have changed.
	void actuatorWindowPreferencesChanged(BioXASCarbonFilterFarmActuator::Location location);

public slots:
	/// Sets the motor control for the actuator at the given location.
	void setActuatorMotor(BioXASCarbonFilterFarmActuator::Location, CLSMAXvMotor *newControl);
	/// Sets the position status control for the actuator at the given location.
	void setActuatorPositionStatus(BioXASCarbonFilterFarmActuator::Location, AMControl *newControl);

	/// Adds a window option for the actuator at the given location.
	void addActuatorWindow(BioXASCarbonFilterFarmActuator::Location location, BioXASCarbonFilterFarmActuator::Window::Option window, double positionSetpoint, double positionMin, double positionMax, double filter);
	/// Removes a window option for the actuator at the given location.
	void removeActuatorWindow(BioXASCarbonFilterFarmActuator::Location location, BioXASCarbonFilterFarmActuator::Window::Option window);
	/// Clears all window options for the actuator at the given location.
	void clearActuatorWindows(BioXASCarbonFilterFarmActuator::Location location);
	/// Clears all window options.
	void clearWindows();

	/// Sets an actuator window preference for a particular filter, should there be multiple windows with the same filter on the same actuator.
	void setActuatorWindowPreference(BioXASCarbonFilterFarmActuator::Location location, double filter, BioXASCarbonFilterFarmActuator::Window::Option preference);

protected:
	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuator *upstreamActuator_;
	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuator *downstreamActuator_;
	/// The filter control.
	BioXASCarbonFilterFarmFilterControl *filter_;
};

#endif // BIOXASCARBONFILTERFARM_H
