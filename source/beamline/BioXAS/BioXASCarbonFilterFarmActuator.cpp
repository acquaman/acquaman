#include "BioXASCarbonFilterFarmActuator.h"

BioXASCarbonFilterFarmActuator::BioXASCarbonFilterFarmActuator(const QString &deviceName, QObject *parent) :
	BioXASCarbonFilterFarmActuatorControl(deviceName, parent)
{	
	setPosition(new BioXASCarbonFilterFarmActuatorPositionControl(name()+"Position", "mm", this));
	setWindow(new BioXASCarbonFilterFarmActuatorWindowControl(name()+"Window", this));
	setFilter(new BioXASCarbonFilterFarmActuatorFilterControl(name()+"Filter", "mm", this));
}

BioXASCarbonFilterFarmActuator::~BioXASCarbonFilterFarmActuator()
{

}
