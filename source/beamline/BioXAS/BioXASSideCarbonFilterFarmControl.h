#ifndef BIOXASSIDECARBONFILTERFARMCONTROL_H
#define BIOXASSIDECARBONFILTERFARMCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"

class BioXASSideCarbonFilterFarmControl : public BioXASCarbonFilterFarmControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideCarbonFilterFarmControl(BioXASCarbonFilterFarmActuatorControl *upstreamActuatorControl, BioXASCarbonFilterFarmActuatorControl *downstreamActuatorControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideCarbonFilterFarmControl();

	/// Returns true if the given value corresponds to a valid filter, false otherwise.
	virtual bool validFilter(double value);
	/// Returns true if the given value corresponds to a valid filter setpoint, false otherwise.
	virtual bool validFilterSetpoint(double value);

};

#endif // BIOXASSIDECARBONFILTERFARMCONTROL_H
