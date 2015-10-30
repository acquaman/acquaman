#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROL_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.h"

// error codes.

#define BIOXAS_FILTER_FARM_NOT_CONNECTED 23048701
#define BIOXAS_FILTER_FARM_ALREADY_MOVING 23048702
#define BIOXAS_FILTER_FARM_INVALID_SETPOINT 23048703
#define BIOXAS_FILTER_FARM_MOVE_FAILED 23048704

class BioXASCarbonFilterFarmActuatorControl : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorControl();

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the position control.
	BioXASCarbonFilterFarmActuatorPositionControl* position() const { return position_; }
	/// Returns the window control.
	BioXASCarbonFilterFarmActuatorWindowControl* window() const { return window_; }
	/// Returns the filter control.
	BioXASCarbonFilterFarmActuatorFilterControl* filter() const { return filter_; }

signals:
	/// Notifier that the position control has changed.
	void positionChanged(AMControl *newControl);
	/// Notifier that the window control has changed.
	void windowChanged(AMControl *newControl);
	/// Notifier that the filter control has changed.
	void filterChanged(AMControl *newControl);

public slots:
	/// Sets the position control.
	void setPosition(BioXASCarbonFilterFarmActuatorPositionControl *newControl);
	/// Sets the window control.
	void setWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl);
	/// Sets the filter control.
	void setFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl);

protected:
	/// The position control.
	BioXASCarbonFilterFarmActuatorPositionControl *position_;
	/// The window control.
	BioXASCarbonFilterFarmActuatorWindowControl *window_;
	/// The filter control.
	BioXASCarbonFilterFarmActuatorFilterControl *filter_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROL_H
