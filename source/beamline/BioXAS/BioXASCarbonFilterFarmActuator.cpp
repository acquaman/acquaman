#include "BioXASCarbonFilterFarmActuator.h"

BioXASCarbonFilterFarmActuator::BioXASCarbonFilterFarmActuator(const QString &name, QObject *parent) :
	BioXASCarbonFilterFarmActuatorControl(name, parent)
{
	setPosition(new BioXASCarbonFilterFarmActuatorPositionControl("Position", "mm", this));
	setWindow(new BioXASCarbonFilterFarmActuatorWindowControl("Window", this));
	setFilter(new BioXASCarbonFilterFarmActuatorFilterControl("Filter", "mm", this));
}

BioXASCarbonFilterFarmActuator::~BioXASCarbonFilterFarmActuator()
{

}
