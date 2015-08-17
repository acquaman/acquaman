#ifndef BIOXASMAINCARBONFILTERFARMCONTROL_H
#define BIOXASMAINCARBONFILTERFARMCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"

class BioXASMainCarbonFilterFarmControl : public BioXASCarbonFilterFarmControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainCarbonFilterFarmControl(BioXASCarbonFilterFarmActuatorControl *upstreamActuatorControl, BioXASCarbonFilterFarmActuatorControl *downstreamActuatorControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainCarbonFilterFarmControl();

	/// Returns true if the given value corresponds to a valid filter, false otherwise.
	virtual bool validFilter(double value);
	/// Returns true if the given value corresponds to a valid filter setpoint, false otherwise.
	virtual bool validFilterSetpoint(double value);
};

#endif // BIOXASMAINCARBONFILTERFARMCONTROL_H
