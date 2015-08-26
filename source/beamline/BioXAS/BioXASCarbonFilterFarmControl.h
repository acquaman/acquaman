#ifndef BIOXASCARBONFILTERFARMCONTROL_H
#define BIOXASCARBONFILTERFARMCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"

// error codes.

#define BIOXAS_FILTER_FARM_NOT_CONNECTED 23048701
#define BIOXAS_FILTER_FARM_ALREADY_MOVING 23048702
#define BIOXAS_FILTER_FARM_INVALID_SETPOINT 23048703
#define BIOXAS_FILTER_FARM_MOVE_FAILED 23048704

class BioXASCarbonFilterFarmControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmControl();

	/// Returns true if the total filter thickness is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new total filter thickness is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }
};

#endif // BIOXASCARBONFILTERFARMCONTROL_H
