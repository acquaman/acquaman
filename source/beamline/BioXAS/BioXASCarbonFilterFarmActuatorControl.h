#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROL_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmWindowOption.h"
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
	explicit BioXASCarbonFilterFarmActuatorControl(const QString &actuatorName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorControl();

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the current position control.
	AMControl* currentPosition() const { return currentPosition_; }
	/// Returns the current window control.
	BioXASCarbonFilterFarmActuatorWindowControl* currentWindow() const { return currentWindow_; }
	/// Returns the current filter control.
	BioXASCarbonFilterFarmActuatorFilterControl* currentFilter() const { return currentFilter_; }

signals:
	/// Notifier that the position control has changed.
	void currentPositionChanged(AMControl *newControl);

public slots:
	/// Sets the current position control.
	void setCurrentPosition(AMControl *newControl);

protected:
	/// The current position control.
	AMControl *currentPosition_;
	/// The current window control.
	BioXASCarbonFilterFarmActuatorWindowControl *currentWindow_;
	/// The current filter control.
	BioXASCarbonFilterFarmActuatorFilterControl *currentFilter_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROL_H
