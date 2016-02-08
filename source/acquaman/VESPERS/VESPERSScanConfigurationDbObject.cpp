/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "VESPERSScanConfigurationDbObject.h"

VESPERSScanConfigurationDbObject::~VESPERSScanConfigurationDbObject(){}

VESPERSScanConfigurationDbObject::VESPERSScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	I0_ = VESPERS::Imini;
	It_ = VESPERS::Ipost;
	fluorescenceDetector_ = VESPERS::NoXRF;
	ccdDetector_ = VESPERS::NoCCD;
	motor_ = VESPERS::NoMotor;
	ccdFileName_ = "";
	normalPosition_ = 888888.88;
}

VESPERSScanConfigurationDbObject::VESPERSScanConfigurationDbObject(const VESPERSScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	I0_ = original.incomingChoice();
	It_ = original.transmissionChoice();
	fluorescenceDetector_ = original.fluorescenceDetector();
	ccdDetector_ = original.ccdDetector();
	motor_ = original.motor();
	ccdFileName_ = original.ccdFileName();
	normalPosition_ = original.normalPosition();

	foreach (AMRegionOfInterest *region, original.regionsOfInterest())
		addRegionOfInterest(region->createCopy());
}

void VESPERSScanConfigurationDbObject::setIncomingChoice(VESPERS::IonChamber I0)
 {
	if (I0_ != I0){

		I0_ = I0;
		emit incomingChoiceChanged(I0_);
		emit incomingChoiceChanged(int(I0_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setTransmissionChoice(VESPERS::IonChamber It)
{
	if (It_ != It){

		It_ = It;
		emit transmissionChoiceChanged(It_);
		emit transmissionChoiceChanged(int(It_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setFluorescenceDetector(VESPERS::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		emit fluorescenceDetectorChanged(fluorescenceDetector_);
		emit fluorescenceDetectorChanged(int(fluorescenceDetector_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setMotor(VESPERS::Motors choice)
{
	if (motor_ != choice) {

		motor_ = choice;
		emit motorChanged(motor_);
		emit motorChanged(int(motor_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setCCDDetector(VESPERS::CCDDetectors ccd)
{
	if (ccdDetector_ != ccd){

		ccdDetector_ = ccd;
		emit ccdDetectorChanged(ccdDetector_);
		emit ccdDetectorChanged(int(ccdDetector_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setCCDFileName(const QString &name)
{
	ccdFileName_ = name;
	emit ccdFileNameChanged(ccdFileName_);
	setModified(true);
}

void VESPERSScanConfigurationDbObject::setNormalPosition(double newPosition)
{
	if (normalPosition_ != newPosition){

		normalPosition_ = newPosition;
		emit normalPositionChanged(newPosition);
		setModified(true);
	}
}

AMDbObjectList VESPERSScanConfigurationDbObject::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void VESPERSScanConfigurationDbObject::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

void VESPERSScanConfigurationDbObject::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void VESPERSScanConfigurationDbObject::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

void VESPERSScanConfigurationDbObject::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
		if (regionToBeUpdated->name() == region->name()){

			regionToBeUpdated->setBoundingRange(region->boundingRange());
			setModified(true);
		}
}

