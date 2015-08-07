#include "IDEASScanConfigurationDbObject.h"

IDEASScanConfigurationDbObject::IDEASScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	fluorescenceDetector_ = IDEAS::NoXRF;
}

IDEASScanConfigurationDbObject::IDEASScanConfigurationDbObject(const IDEASScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	fluorescenceDetector_ = original.fluorescenceDetector();

	foreach (AMRegionOfInterest *region, original.regionsOfInterest())
		addRegionOfInterest(region->createCopy());
}

IDEASScanConfigurationDbObject::~IDEASScanConfigurationDbObject()
{

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
