#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"


class BioXASCarbonFilterFarm : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Enumeration of the different actuator windows.
	class Window { public: enum Option { None = 0, Bottom = 1, Top = 2 }; };

	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const;

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
	/// Notifier that the upstream actuator windows have changed.
	void upstreamActuatorWindowsChanged();
	/// Notifier that the upstream actuator window preferences have changed.
	void upstreamActuatorWindowPreferencesChanged();
	/// Notifier that the downstream actuator motor control has changed.
	void downstreamActuatorMotorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the downstream actuator position status control has changed.
	void downstreamActuatorPositionStatusChanged(AMControl *newControl);
	/// Notifier that the downstream actuator windows have changed.
	void downstreamActuatorWindowsChanged();
	/// Notifier that the downstream actuator window preferences have changed.
	void downstreamActuatorWindowPreferencesChanged();

public slots:
	/// Sets the upstream actuator motor control.
	void setUpstreamActuatorMotor(CLSMAXvMotor *newControl);
	/// Sets the upstream actuator position status control.
	void setUpstreamActuatorPositionStatus(AMControl *newControl);
	/// Adds a window to the upstream actuator.
	void addUpstreamActuatorWindow(int windowIndex, double positionSetpoint, double positionMin, double positionMax, double filter);
	/// Removes a window from the upstream actuator.
	void removeUpstreamActuatorWindow(int windowIndex);
	/// Clears the windows on the upstream actuator.
	void clearUpstreamActuatorWindows();
	/// Sets a window preference for the upstream actuator.
	void setUpstreamActuatorWindowPreference(double filter, int windowIndex);
	/// Removes a window preference for the upstream actuator.
	void removeUpstreamActuatorWindowPreference(double filter);
	/// Clears the upstream actuator's window preferences.
	void clearUpstreamActuatorWindowPreferences();

	/// Sets the downstream actuator motor control.
	void setDownstreamActuatorMotor(CLSMAXvMotor *newControl);
	/// Sets the downstream actuator position status control.
	void setDownstreamActuatorPositionStatus(AMControl *newControl);
	/// Adds a window to the downstream actuator.
	void addDownstreamActuatorWindow(int windowIndex, double positionSetpoint, double positionMin, double positionMax, double filter);
	/// Removes a window from the downstream actuator.
	void removeDownstreamActuatorWindow(int windowIndex);
	/// Clears the windows on the downstream actuator.
	void clearDownstreamActuatorWindows();
	/// Sets a window preference for the downstream actuator.
	void setDownstreamActuatorWindowPreference(double filter, int windowIndex);
	/// Removes a window preferences from the downstream actuator.
	void removeDownstreamActuatorWindowPreference(double filter);
	/// Clears the downstream actuator's window preferences.
	void clearDownstreamActuatorWindowPreferences();

	/// Clears all window options.
	void clearWindows();
	/// Clears all window preferences.
	void clearWindowPreferences();

protected:
	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuator *upstreamActuator_;
	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuator *downstreamActuator_;
	/// The filter control.
	BioXASCarbonFilterFarmFilterControl *filter_;
};

#endif // BIOXASCARBONFILTERFARM_H
