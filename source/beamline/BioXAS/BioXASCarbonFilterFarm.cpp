#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &name, QObject *parent) :
	BioXASCarbonFilterFarmControl(name, parent)
{
	setUpstreamActuator(new BioXASCarbonFilterFarmActuatorControl(objectName()+"UpstreamActuator", this));
	setDownstreamActuator(new BioXASCarbonFilterFarmActuatorControl(objectName()+"DownstreamActuator", this));

	setFilter(new BioXASCarbonFilterFarmFilterControl(objectName()+"Filter", "mm", this));
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}
