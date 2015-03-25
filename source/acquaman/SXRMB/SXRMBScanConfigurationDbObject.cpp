#include "SXRMBScanConfigurationDbObject.h"

SXRMBScanConfigurationDbObject::SXRMBScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	y_ = 0.0;
	enableBrukerDetector_ = true;
}

SXRMBScanConfigurationDbObject::SXRMBScanConfigurationDbObject(const SXRMBScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	y_ = original.y();
	enableBrukerDetector_ = original.enableBrukerDetector();

	foreach (AMRegionOfInterest *region, original.regionsOfInterest())
		addRegionOfInterest(region->createCopy());
}

SXRMBScanConfigurationDbObject::~SXRMBScanConfigurationDbObject()
{

}

void SXRMBScanConfigurationDbObject::setY(double newY)
{
	if (y_ != newY){

		y_ = newY;
		emit yChanged(y_);
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

void SXRMBScanConfigurationDbObject::setEnableBrukerDetector(bool enableDetector)
{
	if (enableBrukerDetector_ != enableDetector)
		enableBrukerDetector_ = enableDetector;
}

void SXRMBScanConfigurationDbObject::setEndstation(SXRMB::Endsation endstation)
{
	if (endstation_ != endstation){

		endstation_ = endstation;
		emit endstationChanged(endstation_);
		setModified(true);
	}
}

void SXRMBScanConfigurationDbObject::setFluorescenceDetectors(SXRMB::FluorescenceDetectors detector)
{
	if (fluorescenceDetectors_ != detector){

		fluorescenceDetectors_ = detector;
		emit fluorescenceDetectorsChanged(fluorescenceDetectors_);
		setModified(true);
	}
}

void SXRMBScanConfigurationDbObject::setEndstation(int endstation)
{
	setEndstation(SXRMB::Endsation(endstation));
}

void SXRMBScanConfigurationDbObject::setFluorescenceDetectors(int detector)
{
	setFluorescenceDetectors(SXRMB::FluorescenceDetectors(detector));
}

void SXRMBScanConfigurationDbObject::setX(double newX)
{
	if (x_ != newX){

		x_ = newX;
		emit xChanged(x_);
		setModified(true);
	}
}

void SXRMBScanConfigurationDbObject::setZ(double newZ)
{
	if (z_ != newZ){

		z_ = newZ;
		emit zChanged(z_);
		setModified(true);
	}
}

void SXRMBScanConfigurationDbObject::setRotation(double newRotation)
{
	if (rotation_ != newRotation){

		rotation_ = newRotation;
		emit rotationChanged(rotation_);
		setModified(true);
	}
}

void SXRMBScanConfigurationDbObject::setEnergy(double newEnergy)
{
	if (energy_ != newEnergy){

		energy_ = newEnergy;
		emit energyChanged(energy_);
		setModified(true);
	}
}
