#include "SGMUserConfiguration.h"

SGMUserConfiguration::SGMUserConfiguration(QObject *parent)
	: AMDbObject(parent)
{
	setName("User Configuration");

}

SGMUserConfiguration::~SGMUserConfiguration()
{

}

void SGMUserConfiguration::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void SGMUserConfiguration::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

AMDbObjectList SGMUserConfiguration::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void SGMUserConfiguration::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

