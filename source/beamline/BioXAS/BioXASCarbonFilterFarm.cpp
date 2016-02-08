#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent) :
	BioXASCarbonFilterFarmControl(deviceName, parent)
{
	setFilter(new BioXASCarbonFilterFarmFilterControl(QString("%1%2").arg(name()).arg("Filter"), "um", this));
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}
