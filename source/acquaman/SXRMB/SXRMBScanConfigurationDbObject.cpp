#include "SXRMBScanConfigurationDbObject.h"

SXRMBScanConfigurationDbObject::SXRMBScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	endstation_ = SXRMB::UnkownEndstation;
	setFluorescenceDetector(SXRMB::BrukerDetector);
	x_ = 0.0;
	y_ = 0.0;
	z_ = 0.0;
	rotation_ = 0.0;
	energy_ = 0.0;
}

SXRMBScanConfigurationDbObject::SXRMBScanConfigurationDbObject(const SXRMBScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	endstation_ = original.endstation();
	fluorescenceDetector_ = original.fluorescenceDetector();
	x_ = original.x();
	y_ = original.y();
	z_ = original.z();
	rotation_ = original.rotation();
	energy_ = original.energy();

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

void SXRMBScanConfigurationDbObject::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
		if (regionToBeUpdated->name() == region->name()){

			regionToBeUpdated->setBoundingRange(region->boundingRange());
			setModified(true);
		}
}

void SXRMBScanConfigurationDbObject::setEndstation(SXRMB::Endstation endstation)
{
	if (endstation_ != endstation){

		SXRMB::Endstation fromEndstation = endstation_;
		endstation_ = endstation;
		emit endstationChanged(fromEndstation, endstation_);
		setModified(true);
	}
}

void SXRMBScanConfigurationDbObject::setFluorescenceDetector(SXRMB::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		emit fluorescenceDetectorChanged(fluorescenceDetector_);
		setModified(true);
	}
}

void SXRMBScanConfigurationDbObject::setFluorescenceDetector(int detector)
{
	setFluorescenceDetector(SXRMB::FluorescenceDetectors(detector));
}

void SXRMBScanConfigurationDbObject::setEndstation(int endstation)
{
	setEndstation(SXRMB::Endstation(endstation));
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
