#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &name, QObject *parent) :
	BioXASCarbonFilterFarmControl(name, parent)
{
	setUpstreamActuator(new BioXASCarbonFilterFarmActuator("UpstreamActuator", this));
	setDownstreamActuator(new BioXASCarbonFilterFarmActuator("DownstreamActuator", this));

	setFilter(new BioXASCarbonFilterFarmFilterControl("Filter", "mm", this));
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}
