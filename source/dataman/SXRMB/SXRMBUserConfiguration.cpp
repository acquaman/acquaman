#include "SXRMBUserConfiguration.h"

SXRMBUserConfiguration::SXRMBUserConfiguration(QObject *parent)
	: AMDbObject(parent)
{
	setName("User Configuration");
}

SXRMBUserConfiguration::~SXRMBUserConfiguration()
{

}

AMDbObjectList SXRMBUserConfiguration::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void SXRMBUserConfiguration::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

void SXRMBUserConfiguration::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void SXRMBUserConfiguration::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}
