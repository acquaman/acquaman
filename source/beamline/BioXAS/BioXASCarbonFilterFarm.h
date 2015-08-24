#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/AMControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"

class BioXASCarbonFilterFarm : public BioXASBeamlineComponent
{
	Q_OBJECT

public:

	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns true if the filter farm is connected, false otherwise.
	bool isConnected() const;

	/// Returns the upstream position control.
	AMControl* upstreamPositionControl() const { return upstreamPosition_; }
	/// Returns the upstream status control.
	AMControl* upstreamStatusControl() const { return upstreamStatus_; }
	/// Returns the upstream actuator control.
	AMControl* upstreamActuatorControl() const { return upstreamActuator_; }

	/// Returns the downstream position control.
	AMControl* downstreamPositionControl() const { return downstreamPosition_; }
	/// Returns the downstream status control.
	AMControl* downstreamStatusControl() const { return downstreamStatus_; }
	/// Returns the downstream actuator control.
	AMControl* downstreamActuatorControl() const { return downstreamActuator_; }

	/// Returns the total filter control.
	AMControl* filterControl() const { return filter_; }

protected:

	/// The upstream position motor control.
	AMControl *upstreamPosition_;
	/// The upstream status control.
	AMControl *upstreamStatus_;
	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;

	/// The downstream position motor control.
	AMControl *downstreamPosition_;
	/// The downstream status control.
	AMControl *downstreamStatus_;
	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;

	/// The total filter control.
	BioXASCarbonFilterFarmControl *filter_;
};

#endif // BIOXASCARBONFILTERFARM_H
