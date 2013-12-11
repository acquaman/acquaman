#include "VESPERSUserConfiguration.h"

VESPERSUserConfiguration::VESPERSUserConfiguration(QObject *parent)
	: AMDbObject(parent)
{
	I0_ = VESPERS::Imini;
	It_ = VESPERS::Ipost;
	fluorescenceDetector_ = VESPERS::NoXRF;
	ccdDetector_ = VESPERS::NoCCD;
	motor_ = VESPERS::NoMotor;
}

void VESPERSUserConfiguration::setIncomingChoice(VESPERS::IonChamber I0)
 {
	if (I0_ != I0){

		I0_ = I0;
		setModified(true);
	}
}

void VESPERSUserConfiguration::setTransmissionChoice(VESPERS::IonChamber It)
{
	if (It_ != It){

		It_ = It;
		setModified(true);
	}
}

void VESPERSUserConfiguration::setFluorescenceDetector(VESPERS::FluorescenceDetector detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		setModified(true);
	}
}

void VESPERSUserConfiguration::setMotor(VESPERS::Motor choice)
{
	if (motor_ != choice) {

		motor_ = choice;
		setModified(true);
	}
}

void VESPERSUserConfiguration::setCCDDetector(VESPERS::CCDDetector ccd)
{
	if (ccdDetector_ != ccd){

		ccdDetector_ = ccd;
		setModified(true);
	}
}

AMDbObjectList VESPERSUserConfiguration::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void VESPERSUserConfiguration::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

void VESPERSUserConfiguration::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void VESPERSUserConfiguration::removeRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.removeOne(region);
	setModified(true);
}
