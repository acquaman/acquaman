#include "SXRMBScanConfigurationDbObject.h"

SXRMBScanConfigurationDbObject::SXRMBScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	normalPosition_ = 0.0;
}

SXRMBScanConfigurationDbObject::SXRMBScanConfigurationDbObject(const SXRMBScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	normalPosition_ = original.normalPosition();

	foreach (AMRegionOfInterest *region, original.regionsOfInterest())
		addRegionOfInterest(region->createCopy());
}

SXRMBScanConfigurationDbObject::~SXRMBScanConfigurationDbObject()
{

}

void SXRMBScanConfigurationDbObject::setNormalPosition(double newPosition)
{
	if (normalPosition_ != newPosition){

		normalPosition_ = newPosition;
		emit normalPositionChanged(newPosition);
		setModified(true);
	}
}

AMDbObjectList SXRMBScanConfigurationDbObject::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void SXRMBScanConfigurationDbObject::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

void SXRMBScanConfigurationDbObject::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void SXRMBScanConfigurationDbObject::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

