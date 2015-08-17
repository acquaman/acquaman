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
	/// Enum defining all possible filter thickness combinations.
	class Filter { public: enum Thickness { Invalid = 0, None, Fifty, SeventyFive, FiveHundred, FiveHundredSeventyFive, SevenHundred, SevenHundredFifty }; };
	/// Enum defining the different actuators.
	class Actuator { public: enum Position { Upstream = 0, Downstream }; };
	/// Enum defining the active window options. None, Bottom, and Top are all valid options; Invalid would represent some kind of error state.
	class Window { public: enum Selection { Invalid = 0, None, Bottom, Top }; };
	/// Enum defining the possible position states.
	class Position { public: enum State { Invalid = 0, Valid }; };

	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns true if the filter farm is connected, false otherwise.
	bool isConnected() const;

	/// Returns the total filter control.
	AMControl* filterControl() const { return filter_; }

	/// Returns the upstream actuator control.
	AMControl* upstreamActuatorControl() const { return upstreamActuator_; }
	/// Returns the upstream position control.
	AMControl* upstreamPositionControl() const { return upstreamPosition_; }
	/// Returns the upstream status control.
	AMControl* upstreamStatusControl() const { return upstreamStatus_; }

	/// Returns the downstream actuator control.
	AMControl* downstreamActuatorControl() const { return downstreamActuator_; }
	/// Returns the downstream position control.
	AMControl* downstreamPositionControl() const { return downstreamPosition_; }
	/// Returns the downstream status control.
	AMControl* downstreamStatusControl() const { return downstreamStatus_; }

protected:
	/// The total filter control.
	BioXASCarbonFilterFarmControl *filter_;

	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;
	/// The upstream position motor control.
	AMControl *upstreamPosition_;
	/// The upstream status control.
	AMControl *upstreamStatus_;

	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;
	/// The downstream position motor control.
	AMControl *downstreamPosition_;
	/// The downstream status control.
	AMControl *downstreamStatus_;
};

#endif // BIOXASCARBONFILTERFARM_H
