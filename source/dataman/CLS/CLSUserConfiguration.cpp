#include "CLSUserConfiguration.h"

CLSUserConfiguration::CLSUserConfiguration(QObject *parent)
	: AMDbObject(parent)
{
	setName("User Configuration");
}

CLSUserConfiguration::~CLSUserConfiguration()
{
	regionsOfInterest_.clear();
}

void CLSUserConfiguration::addRegionOfInterest(AMRegionOfInterest *region)
{
	if ( !containsRegionOfInterest(region) ) {
		regionsOfInterest_.append(region);
		setModified(true);
	}
}

void CLSUserConfiguration::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

void CLSUserConfiguration::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
		if (regionToBeUpdated->name() == region->name()){

			regionToBeUpdated->setBoundingRange(region->boundingRange());
			setModified(true);
		}
}


AMDbObjectList CLSUserConfiguration::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void CLSUserConfiguration::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

bool CLSUserConfiguration::containsRegionOfInterest(AMRegionOfInterest *toFind) const
{
	bool regionOfInterestFound = false;

	if (!regionsOfInterest_.isEmpty() && toFind) {
		for (int i = 0, count = regionsOfInterest_.count(); i < count && !regionOfInterestFound; i++) {
			AMRegionOfInterest *regionOfInterest = regionsOfInterest_.at(i);

			if (regionOfInterest && regionOfInterest->name() == toFind->name())
				regionOfInterestFound = true;
		}
	}

	return regionOfInterestFound;
}
