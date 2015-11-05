#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent) :
	BioXASCarbonFilterFarmControl(deviceName, parent)
{
	setUpstreamActuator(new BioXASCarbonFilterFarmActuatorControl(name()+"UpstreamActuator", this));
	setDownstreamActuator(new BioXASCarbonFilterFarmActuatorControl(name()+"DownstreamActuator", this));

	setFilter(new BioXASCarbonFilterFarmFilterControl(name()+"Filter", "mm", this));
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}
