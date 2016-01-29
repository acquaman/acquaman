#ifndef BIOXASCARBONFILTERFARMACTUATOR_H
#define BIOXASCARBONFILTERFARMACTUATOR_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.h"
#include "beamline/AMPVControl.h"

// error codes.

#define BIOXAS_FILTER_FARM_NOT_CONNECTED 23048701
#define BIOXAS_FILTER_FARM_ALREADY_MOVING 23048702
#define BIOXAS_FILTER_FARM_INVALID_SETPOINT 23048703
#define BIOXAS_FILTER_FARM_MOVE_FAILED 23048704

class BioXASCarbonFilterFarmActuator : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Enumerates the different actuator window options.
	class Window { public: enum Option { None = 0, Bottom = 1, Top = 2 }; };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuator(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuator();

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the position control.
	AMControl* position() const;
	/// Returns the position status control.
	AMControl* positionStatus() const;
	/// Returns the window control.
	BioXASCarbonFilterFarmActuatorWindowControl* window() const { return window_; }
	/// Returns the filter control.
	BioXASCarbonFilterFarmActuatorFilterControl* filter() const { return filter_; }

	/// Returns a string representation of the given window option.
	static QString windowToString(double window);

signals:
	/// Notifier that the position control has changed.
	void positionChanged(AMControl *newControl);
	/// Notifier that the position status control has changed.
	void positionStatusChanged(AMControl *newControl);
	/// Notifier that the window control has changed.
	void windowChanged(AMControl *window);
	/// Notifier that the filter control has changed.
	void filterChanged(AMControl *filter);

public slots:
	/// Sets the position control.
	void setPositionControl(AMControl *newControl);
	/// Sets the position status control.
	void setPositionStatusControl(AMControl *newControl);

	/// Sets the window control.
	void setWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl);
	/// Sets the filter control.
	void setFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl);

	/// Adds a window option.
	void addWindow(Window::Option window, double positionSetpoint, double positionMin, double positionMax, double filter);
	/// Removes a window option.
	void removeWindow(Window::Option window);
	/// Clears all window options.
	void clearWindows();

	/// Sets a filter window preference.
	void setWindowPreference(double filter, Window::Option window);
	/// Removes a filter window preference.
	void removeWindowPreference(double filter);
	/// Clears all filter window preferences.
	void clearWindowPreferences();

protected slots:
	/// Sets the position control.
	bool setPosition(BioXASCarbonFilterFarmActuatorPositionControl *newControl);

	/// Updates the position control.
	void updatePosition();
	/// Updates the window control.
	void updateWindow();
	/// Updates the filter control.
	void updateFilter();

protected:
	/// The current position control.
	BioXASCarbonFilterFarmActuatorPositionControl *position_;
	/// The current window control.
	BioXASCarbonFilterFarmActuatorWindowControl *window_;
	/// The current filter control.
	BioXASCarbonFilterFarmActuatorFilterControl *filter_;
};

#endif // BIOXASCARBONFILTERFARMACTUATOR_H
