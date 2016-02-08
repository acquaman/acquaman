#include "IDEASScanConfigurationDbObject.h"

IDEASScanConfigurationDbObject::IDEASScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	energy_ = 0;
	fluorescenceDetector_ = IDEAS::NoXRF;
}

IDEASScanConfigurationDbObject::IDEASScanConfigurationDbObject(const IDEASScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	energy_ = original.energy();
	fluorescenceDetector_ = original.fluorescenceDetector();

	foreach (AMRegionOfInterest *region, original.regionsOfInterest())
		addRegionOfInterest(region->createCopy());
}

IDEASScanConfigurationDbObject::~IDEASScanConfigurationDbObject()
{

}

void IDEASScanConfigurationDbObject::setEnergy(double edgeEnergy)
{
	if (energy_ != edgeEnergy){

		energy_ = edgeEnergy;
		emit energyChanged(energy_);
		setModified(true);
	}
}

void IDEASScanConfigurationDbObject::setFluorescenceDetector(IDEAS::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		emit fluorescenceDetectorChanged(fluorescenceDetector_);
		emit fluorescenceDetectorChanged(int(fluorescenceDetector_));
		setModified(true);
	}
}

AMDbObjectList IDEASScanConfigurationDbObject::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void IDEASScanConfigurationDbObject::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

void IDEASScanConfigurationDbObject::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void IDEASScanConfigurationDbObject::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

void IDEASScanConfigurationDbObject::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
		if (regionToBeUpdated->name() == region->name()){

			regionToBeUpdated->setBoundingRange(region->boundingRange());
			setModified(true);
		}
}
