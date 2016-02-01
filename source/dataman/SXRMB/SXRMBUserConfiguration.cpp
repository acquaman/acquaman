#include "SXRMBUserConfiguration.h"

SXRMBUserConfiguration::SXRMBUserConfiguration(QObject *parent)
	: AMDbObject(parent)
{
	setName("User Configuration");

	setFluorescenceDetector(SXRMB::BrukerDetector);
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

void SXRMBUserConfiguration::dbLoadFluorescenceDetector(int detector) {
	setFluorescenceDetector((SXRMB::FluorescenceDetectors)detector);
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

void SXRMBUserConfiguration::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
		if (regionToBeUpdated->name() == region->name()){

			regionToBeUpdated->setBoundingRange(region->boundingRange());
			setModified(true);
		}
}

void SXRMBUserConfiguration::setFluorescenceDetector(SXRMB::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		setModified(true);
	}
}
