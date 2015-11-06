#include "BioXASCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent) :
	BioXASCarbonFilterFarmControl(deviceName, parent)
{
	setUpstreamActuator(new BioXASCarbonFilterFarmActuator(name()+"UpstreamActuator", this));
	setDownstreamActuator(new BioXASCarbonFilterFarmActuator(name()+"DownstreamActuator", this));

	setFilter(new BioXASCarbonFilterFarmFilterControl(name()+"Filter", "mm", this));
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}
