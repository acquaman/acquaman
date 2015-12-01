#ifndef BIOXASCARBONFILTERFARMACTUATOR_H
#define BIOXASCARBONFILTERFARMACTUATOR_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

class BioXASCarbonFilterFarmActuator : public BioXASCarbonFilterFarmActuatorControl
{
    Q_OBJECT

public:
	/// Enumerates the different actuator window options.
	class Window { public: enum Option { None = 0, Bottom = 1, Top = 2 }; };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuator(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuator();

	/// Returns a string representation of the given window option.
	static QString windowToString(double window);

public slots:
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
};

#endif // BIOXASCARBONFILTERFARMACTUATOR_H
