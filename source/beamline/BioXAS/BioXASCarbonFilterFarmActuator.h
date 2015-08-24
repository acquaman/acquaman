#ifndef BIOXASCARBONFILTERFARMACTUATOR_H
#define BIOXASCARBONFILTERFARMACTUATOR_H

#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

class BioXASCarbonFilterFarmActuator : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Enum describing the possible options for the active window.
	enum Window { Unknown = 0, Bottom, Top };
	/// Enum describing the possible position status states, whether or not the actuator position is a valid one.
	enum Position { Invalid = 0, Valid };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuator();

	/// Returns the current connected state, determined by asking for the state of each subcontrol.
	bool isConnected() const;

	/// Returns the position control.
	AMControl* positionControl() const { return position_; }
	/// Returns the status control.
	AMControl* statusControl() const { return status_; }
	/// Returns the actuator control.
	AMControl* actuatorControl() const { return actuator_; }

signals:

public slots:

protected:
	/// The position control.
	AMControl *position_;
	/// The status control.
	AMControl *status_;
	/// The actuator control.
	BioXASCarbonFilterFarmActuatorControl *actuator_;
};

#endif // BIOXASCARBONFILTERFARMACTUATOR_H
