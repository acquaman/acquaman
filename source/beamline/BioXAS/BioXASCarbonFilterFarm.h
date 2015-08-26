#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASCarbonFilterFarmActuatorControl;
class BioXASCarbonFilterFarmActuatorWindowControl;
class BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl;
class BioXASCarbonFilterFarmFilterThicknessControl;

class BioXASCarbonFilterFarm : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Nested class that outlines some basic actuator properties.
	class Actuator {
	public:
		/// Enum describing the actuator windows.
		enum Window { Invalid = 0, None, Bottom, Top };
		/// Enum describing the actuator position status options.
		enum Status { NotInPosition = 0, InPosition };
		/// Enum describing the actuator location.
		enum Location { Unknown = 0, Upstream, Downstream };
	};

	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns true if the filter farm is connected, false otherwise.
	bool isConnected() const;

	/// Returns the upstream position control.
	AMControl* upstreamPositionControl();
	/// Returns the upstream status control.
	AMControl* upstreamStatusControl();
	/// Returns the upstream actuator control.
	AMControl* upstreamActuatorControl();
	/// Returns the upstream actuator window control.
	AMControl* upstreamActuatorWindowControl();
	/// Returns the upstream actuator filter thickness control.
	AMControl* upstreamActuatorFilterThicknessControl();

	/// Returns the downstream position control.
	AMControl* downstreamPositionControl();
	/// Returns the downstream status control.
	AMControl* downstreamStatusControl();
	/// Returns the downstream actuator control.
	AMControl* downstreamActuatorControl();
	/// Returns the downstream actuator window control.
	AMControl* downstreamActuatorWindowControl();
	/// Returns the downstream actuator filter thickness control.
	AMControl* downstreamActuatorFilterThicknessControl();

	/// Returns the total filter thickness control.
	AMControl* filterThicknessControl();

protected:

	/// The upstream position motor control.
	AMControl *upstreamPosition_;
	/// The upstream status control.
	AMControl *upstreamStatus_;
	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;
	/// The upstream actuator window control.
	BioXASCarbonFilterFarmActuatorWindowControl *upstreamActuatorWindow_;
	/// The upstream actuator filter thickness control.
	BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *upstreamActuatorFilterThickness_;

	/// The downstream position motor control.
	AMControl *downstreamPosition_;
	/// The downstream status control.
	AMControl *downstreamStatus_;
	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;
	/// The downstream actuator window control.
	BioXASCarbonFilterFarmActuatorWindowControl *downstreamActuatorWindow_;
	/// The downstream actuator filter thickness control.
	BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *downstreamActuatorFilterThickness_;

	/// The total filter control.
	BioXASCarbonFilterFarmFilterThicknessControl *filterThickness_;
};

#endif // BIOXASCARBONFILTERFARM_H
