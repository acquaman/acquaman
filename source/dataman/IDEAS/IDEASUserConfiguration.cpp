#include "IDEASUserConfiguration.h"

IDEASUserConfiguration::IDEASUserConfiguration(QObject *parent)
	: AMDbObject(parent)
{
	setName("User Configuration");
	setFluorescenceDetector(IDEAS::Ketek);
}

IDEASUserConfiguration::~IDEASUserConfiguration()
{

}

AMDbObjectList IDEASUserConfiguration::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void IDEASUserConfiguration::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

void IDEASUserConfiguration::dbLoadFluorescenceDetector(int detector)
{
	setFluorescenceDetector((IDEAS::FluorescenceDetectors)detector);
}

void IDEASUserConfiguration::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void IDEASUserConfiguration::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

void IDEASUserConfiguration::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
		if (regionToBeUpdated->name() == region->name()){

			regionToBeUpdated->setBoundingRange(region->boundingRange());
			setModified(true);
		}
}

void IDEASUserConfiguration::setFluorescenceDetector(IDEAS::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		setModified(true);
	}
}
