#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROL_H

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

class BioXASCarbonFilterFarmActuatorControl : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorControl(const QString &actuatorName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorControl();

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

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROL_H
