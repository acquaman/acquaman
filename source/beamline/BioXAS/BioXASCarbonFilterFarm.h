#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASCarbonFilterFarmActuatorWindowControl;
class BioXASCarbonFilterFarmActuatorFilterThicknessControl;
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
	};

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
	/// Returns the upstream actuator window control.
	AMControl* upstreamActuatorWindowControl() const { return upstreamActuatorWindow_; }
	/// Returns the upstream actuator filter thickness control.
	AMControl* upstreamActuatorFilterThicknessControl() const { return upstreamActuatorFilterThickness_; }

	/// Returns the downstream position control.
	AMControl* downstreamPositionControl() const { return downstreamPosition_; }
	/// Returns the downstream status control.
	AMControl* downstreamStatusControl() const { return downstreamStatus_; }
	/// Returns the downstream actuator window control.
	AMControl* downstreamActuatorWindowControl() const { return downstreamActuatorWindow_; }
	/// Returns the downstream actuator filter thickness control.
	AMControl* downstreamActuatorFilterThicknessControl() const { return downstreamActuatorFilterThickness_; }

	/// Returns the total filter thickness control.
	AMControl* filterThicknessControl() const { return filterThickness_; }

protected:

	/// The upstream position motor control.
	AMControl *upstreamPosition_;
	/// The upstream status control.
	AMControl *upstreamStatus_;
	/// The upstream actuator window control.
	BioXASCarbonFilterFarmActuatorWindowControl *upstreamActuatorWindow_;
	/// The upstream actuator filter thickness control.
	BioXASCarbonFilterFarmActuatorFilterThicknessControl *upstreamActuatorFilterThickness_;

	/// The downstream position motor control.
	AMControl *downstreamPosition_;
	/// The downstream status control.
	AMControl *downstreamStatus_;
	/// The downstream actuator window control.
	BioXASCarbonFilterFarmActuatorWindowControl *downstreamActuatorWindow_;
	/// The downstream actuator filter thickness control.
	BioXASCarbonFilterFarmActuatorFilterThicknessControl *downstreamActuatorFilterThickness_;

	/// The total filter control.
	BioXASCarbonFilterFarmFilterThicknessControl *filterThickness_;
};

#endif // BIOXASCARBONFILTERFARM_H
