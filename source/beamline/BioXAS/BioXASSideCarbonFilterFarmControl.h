#ifndef BIOXASSIDECARBONFILTERFARMCONTROL_H
#define BIOXASSIDECARBONFILTERFARMCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"

// valid actuator positions.

#define BIOXAS_FILTER_FARM_UPSTREAM_OUT 18.996
#define BIOXAS_FILTER_FARM_UPSTREAM_BOTTOM -3.5
#define BIOXAS_FILTER_FARM_UPSTREAM_TOP -15.1
#define BIOXAS_FILTER_FARM_DOWNSTREAM_OUT 19.15
#define BIOXAS_FILTER_FARM_DOWNSTREAM_BOTTOM -3.5
#define BIOXAS_FILTER_FARM_DOWNSTREAM_TOP -15.2

class BioXASSideCarbonFilterFarmControl : public BioXASCarbonFilterFarmControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideCarbonFilterFarmControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideCarbonFilterFarmControl();

	/// Returns true if the given value corresponds to a valid filter, false otherwise.
	virtual bool validFilter(double value);
	/// Returns true if the given value corresponds to a valid filter setpoint, false otherwise.
	virtual bool validFilterSetpoint(double value);

protected:
	/// Returns a new action that moves the actuators to the desired filter thickness setpoint.
	virtual AMAction3* createMoveAction(double setpoint);
};

#endif // BIOXASSIDECARBONFILTERFARMCONTROL_H
