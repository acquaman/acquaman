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


#include "VESPERSUserConfiguration.h"

VESPERSUserConfiguration::~VESPERSUserConfiguration(){}

VESPERSUserConfiguration::VESPERSUserConfiguration(QObject *parent)
	: CLSUserConfiguration(parent)
{
	setName("User Configuration");
	I0_ = VESPERS::Imini;
	It_ = VESPERS::Ipost;
	fluorescenceDetector_ = VESPERS::SingleElement;
	ccdDetector_ = VESPERS::NoCCD;
	motor_ = VESPERS::Motors(VESPERS::H | VESPERS::V);
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

void VESPERSUserConfiguration::setFluorescenceDetector(VESPERS::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		setModified(true);
	}
}

void VESPERSUserConfiguration::setMotor(VESPERS::Motors choice)
{
	if (motor_ != choice) {

		motor_ = choice;
		setModified(true);
	}
}

void VESPERSUserConfiguration::setCCDDetector(VESPERS::CCDDetectors ccd)
{
	if (ccdDetector_ != ccd){

		ccdDetector_ = ccd;
		setModified(true);
	}
}

