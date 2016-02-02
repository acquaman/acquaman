#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"


class BioXASCarbonFilterFarm : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Enumeration of the different actuator locations.
	class Location { public: enum Option { Upstream = 0, Downstream = 1 }; };
	/// Enumeration of the different actuator windows.
	class Window { public: enum Option { None = 0, Bottom = 1, Top = 2 }; };

	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns the filter value.
	double filterValue() const;

	/// Returns the upstream actuator motor control.
	CLSMAXvMotor* upstreamActuatorMotor() const;
	/// Returns the upstream actuator position status control.
	AMControl* upstreamActuatorPositionStatus() const;
	/// Returns the downstream actuator motor control.
	CLSMAXvMotor* downstreamActuatorMotor() const;
	/// Returns the downstream actuator position status control.
	AMControl* downstreamActuatorPositionStatus() const;

	/// Returns the upstream actuator control.
	BioXASCarbonFilterFarmActuator* upstreamActuator() const { return upstreamActuator_; }
	/// Returns the downstream actuator control.
	BioXASCarbonFilterFarmActuator* downstreamActuator() const { return downstreamActuator_; }
	/// Returns the filter control.
	BioXASCarbonFilterFarmFilterControl* filter() const { return filter_; }

	/// Returns a string representation of the given window option.
	QString windowToString(double window) const;

signals:
	/// Notifier that the filter value has changed.
	void filterValueChanged(double newValue);
	/// Notifier that the upstream actuator motor control has changed.
	void upstreamActuatorMotorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the upstream actuator position status control has changed.
	void upstreamActuatorPositionStatusChanged(AMControl *newControl);
	/// Notifier that the downstream actuator motor control has changed.
	void downstreamActuatorMotorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the downstream actuator position status control has changed.
	void downstreamActuatorPositionStatusChanged(AMControl *newControl);
	/// Notifier that the upstream actuator windows have changed.
	void upstreamActuatorWindowsChanged();
	/// Notifier that the upstream actuator window preferences have changed.
	void upstreamActuatorWindowPreferencesChanged();
	/// Notifier that the downstream actuator window preferences have changed.
	void downstreamActuatorWindowPreferencesChanged();


	/// Notifier that the actuator motor has changed.
	void actuatorMotorChanged(Location location, CLSMAXvMotor *newControl);
	/// Notifier that the actuator position status has changed.
	void actuatorPositionStatusChanged(Location location, AMControl *newControl);
	/// Notifier that the actuator windows have changed.
	void actuatorWindowsChanged(Location location);
	/// Notifier that the actuator window preferences have changed.
	void actuatorWindowPreferencesChanged(Location location);

public slots:
	/// Sets the motor control for the actuator at the given location.
	void setActuatorMotor(Location, CLSMAXvMotor *newControl);
	/// Sets the position status control for the actuator at the given location.
	void setActuatorPositionStatus(Location, AMControl *newControl);

	/// Adds a window option for the actuator at the given location.
	void addActuatorWindow(Location location, Window::Option window, double positionSetpoint, double positionMin, double positionMax, double filter);
	/// Removes a window option for the actuator at the given location.
	void removeActuatorWindow(Location location, Window::Option window);
	/// Clears all window options for the actuator at the given location.
	void clearActuatorWindows(Location location);
	/// Clears all window options.
	void clearWindows();

	/// Sets an actuator window preference for a particular filter, should there be multiple windows with the same filter on the same actuator.
	void setActuatorWindowPreference(Location location, double filter, Window::Option preference);

protected:
	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuator *upstreamActuator_;
	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuator *downstreamActuator_;
	/// The filter control.
	BioXASCarbonFilterFarmFilterControl *filter_;
};

#endif // BIOXASCARBONFILTERFARM_H
