#ifndef BIOXASCARBONFILTERFARMACTUATOR_H
#define BIOXASCARBONFILTERFARMACTUATOR_H

#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

class BioXASCarbonFilterFarmActuator : public BioXASCarbonFilterFarmActuatorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuator();
};

#endif // BIOXASCARBONFILTERFARMACTUATOR_H
