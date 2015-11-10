#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent) :
	BioXASCarbonFilterFarmControl(deviceName, parent)
{
	setFilter(new BioXASCarbonFilterFarmFilterControl(name()+"Filter", "mm", this));
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}
