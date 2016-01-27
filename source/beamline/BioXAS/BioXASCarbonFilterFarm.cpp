#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent) :
	BioXASCarbonFilterFarmControl(deviceName, parent)
{
	setFilter(new BioXASCarbonFilterFarmFilterControl(name()+"Filter", "um", this));
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}
