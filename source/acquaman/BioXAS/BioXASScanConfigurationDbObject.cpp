#include "BioXASScanConfigurationDbObject.h"

BioXASScanConfigurationDbObject::BioXASScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	energy_ = 0.0;
	edge_ = "";
	usingXRFDetector_ = false;
}

BioXASScanConfigurationDbObject::BioXASScanConfigurationDbObject(const BioXASScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	energy_ = original.energy();
	edge_ = original.edge();
	usingXRFDetector_ = original.usingXRFDetector();

	foreach (AMRegionOfInterest *region, original.regionsOfInterest())
		addRegionOfInterest(region->createCopy());
}

BioXASScanConfigurationDbObject::~BioXASScanConfigurationDbObject()
{

}

void BioXASScanConfigurationDbObject::setEnergy(double newEnergy)
{
	if (energy_ != newEnergy){

		energy_ = newEnergy;
		emit energyChanged(energy_);
		setModified(true);
	}
}

void BioXASScanConfigurationDbObject::setEdge(const QString &newEdge)
{
	if (edge_ != newEdge){

		edge_ = newEdge;
		emit edgeChanged(edge_);
		setModified(true);
	}
}

void BioXASScanConfigurationDbObject::setUsingXRFDetector(bool hasXRF)
{
	if (usingXRFDetector_ != hasXRF){

		usingXRFDetector_ = hasXRF;
		emit usingXRFDetectorChanged(usingXRFDetector_);
		setModified(true);
	}
}

void BioXASScanConfigurationDbObject::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void BioXASScanConfigurationDbObject::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

AMDbObjectList BioXASScanConfigurationDbObject::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void BioXASScanConfigurationDbObject::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

