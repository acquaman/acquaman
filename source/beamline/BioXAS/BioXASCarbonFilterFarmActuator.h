#ifndef BIOXASCARBONFILTERFARMACTUATOR_H
#define BIOXASCARBONFILTERFARMACTUATOR_H

#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASCarbonFilterFarmActuator : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuator(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuator();

	/// Returns the current position value.
	double positionValue() const;
	/// Returns the current position status.
	double positionStatusValue() const;
	/// Returns the current window value.
	double windowValue() const;
	/// Returns the current filter value.
	double filterValue() const;

	/// Returns the motor control.
	CLSMAXvMotor* motor() const;
	/// Returns the position status control.
	AMControl* positionStatus() const;

	/// Returns the position control.
	BioXASCarbonFilterFarmActuatorPositionControl* position() const { return position_; }
	/// Returns the window control.
	BioXASCarbonFilterFarmActuatorWindowControl* window() const { return window_; }
	/// Returns the filter control.
	BioXASCarbonFilterFarmActuatorFilterControl* filter() const { return filter_; }

signals:
	/// Notifier that the motor control has changed.
	void motorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the position status control has changed.
	void positionStatusChanged(AMControl *newControl);
	/// Notifier that the position value has changed.
	void positionValueChanged(double newValue);
	/// Notifier that the position status value has changed.
	void positionStatusValueChanged(double newValue);
	/// Notifier that the window value has changed.
	void windowValueChanged(double newValue);
	/// Notifier that the window value options have changed.
	void windowsChanged();
	/// Notifier that the window preferences have changed.
	void windowPreferencesChanged();
	/// Notifier that the filter value has changed.
	void filterValueChanged(double newValue);
	/// Notifier that the filter value options have changed.
	void filtersChanged();

public slots:
	/// Sets the motor control. Returns true if successful, false otherwise.
	bool setMotor(CLSMAXvMotor *newControl);
	/// Sets the position status control. Returns true if successful, false otherwise.
	bool setPositionStatus(AMControl *newControl);

	/// Adds a window option.
	void addWindow(int windowIndex, const QString &windowName, double positionSetpoint, double positionMin, double positionMax, double filter);
	/// Removes a window option.
	void removeWindow(int windowIndex);
	/// Clears all window options.
	void clearWindows();

	/// Sets a filter window preference.
	void setWindowPreference(double filter, int windowIndex);
	/// Removes a filter window preference.
	void removeWindowPreference(double filter);
	/// Clears all filter window preferences.
	void clearWindowPreferences();

protected slots:
	/// Handles emitting the position value changed signal.
	void onPositionControlValueChanged();
	/// Handles emitting the position status value changed signal.
	void onPositionControlStatusValueChanged();
	/// Handles emitting the window value changed signal.
	void onWindowControlValueChanged();
	/// Handles emitting the filter value changed signal.
	void onFilterControlValueChanged();

protected:
	/// The position control.
	BioXASCarbonFilterFarmActuatorPositionControl *position_;
	/// The window control.
	BioXASCarbonFilterFarmActuatorWindowControl *window_;
	/// The filter control.
	BioXASCarbonFilterFarmActuatorFilterControl *filter_;
};

#endif // BIOXASCARBONFILTERFARMACTUATOR_H
