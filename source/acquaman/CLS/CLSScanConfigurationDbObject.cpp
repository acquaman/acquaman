#include "CLSScanConfigurationDbObject.h"

CLSScanConfigurationDbObject::CLSScanConfigurationDbObject(QObject *parent) :
    AMDbObject(parent)
{
}


CLSScanConfigurationDbObject::CLSScanConfigurationDbObject(const CLSScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	foreach (AMRegionOfInterest *region, original.regionsOfInterest())
		addRegionOfInterest(region->createCopy());
}

CLSScanConfigurationDbObject::~CLSScanConfigurationDbObject()
{
	regionsOfInterest_.clear();
}

AMDbObjectList CLSScanConfigurationDbObject::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void CLSScanConfigurationDbObject::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

void CLSScanConfigurationDbObject::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void CLSScanConfigurationDbObject::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

void CLSScanConfigurationDbObject::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
		if (regionToBeUpdated->name() == region->name()){

			regionToBeUpdated->setBoundingRange(region->boundingRange());
			setModified(true);
		}
}
