#include "BioXASCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

BioXASCarbonFilterFarmControl::BioXASCarbonFilterFarmControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{

}

BioXASCarbonFilterFarmControl::~BioXASCarbonFilterFarmControl()
{

}
