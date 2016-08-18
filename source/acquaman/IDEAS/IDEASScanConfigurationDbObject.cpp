#include "IDEASScanConfigurationDbObject.h"

#include "util/AMErrorMonitor.h"

IDEASScanConfigurationDbObject::IDEASScanConfigurationDbObject(QObject *parent)
	: CLSScanConfigurationDbObject(parent)
{
	energy_ = 0;
	fluorescenceDetector_ = IDEAS::NoXRF;
}

IDEASScanConfigurationDbObject::IDEASScanConfigurationDbObject(const CLSScanConfigurationDbObject &original)
	: CLSScanConfigurationDbObject(original)
{
	const IDEASScanConfigurationDbObject *dbObject = qobject_cast<const IDEASScanConfigurationDbObject *>(&original);
	if (!dbObject) {
		AMErrorMon::alert(this, ERR_IDEAS_SCAN_CONFIGURATION_DB_OBJECT_INVALID_COPY_CONSTRUCTOR,
						  QString("Invalid scan configuration Db object class: %1").arg(original.metaObject()->className()));

	} else {
		energy_ = dbObject->energy();
		fluorescenceDetector_ = dbObject->fluorescenceDetector();

//		foreach (AMRegionOfInterest *region, dbObject->regionsOfInterest())
//			addRegionOfInterest(region->createCopy());
	}
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

//AMDbObjectList IDEASScanConfigurationDbObject::dbReadRegionsOfInterest()
//{
//	AMDbObjectList listToBeSaved;

//	foreach (AMRegionOfInterest *region, regionsOfInterest_)
//		listToBeSaved << region;

//	return listToBeSaved;
//}

//void IDEASScanConfigurationDbObject::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
//{
//	regionsOfInterest_.clear();

//	foreach (AMDbObject *newObject, newRegions){

//		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

//		if (region)
//			regionsOfInterest_.append(region);
//	}
//}

//void IDEASScanConfigurationDbObject::addRegionOfInterest(AMRegionOfInterest *region)
//{
//	regionsOfInterest_.append(region);
//	setModified(true);
//}

//void IDEASScanConfigurationDbObject::removeRegionOfInterest(AMRegionOfInterest *region)
//{
//	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
//		if (regionToBeRemoved->name() == region->name()){

//			regionsOfInterest_.removeOne(regionToBeRemoved);
//			setModified(true);
//		}
//}

//void IDEASScanConfigurationDbObject::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
//{
//	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
//		if (regionToBeUpdated->name() == region->name()){

//			regionToBeUpdated->setBoundingRange(region->boundingRange());
//			setModified(true);
//		}
//}
