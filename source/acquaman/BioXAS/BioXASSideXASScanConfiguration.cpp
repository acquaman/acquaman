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


#include "BioXASSideXASScanConfiguration.h"

#include "dataman/AMScanAxisEXAFSRegion.h"
#include "acquaman/BioXAS/BioXASSideXASScanActionController.h"
#include "ui/BioXAS/BioXASSideXASScanConfigurationView.h"
#include "util/AMEnergyToKSpaceCalculator.h"

#include <QStringBuilder>

BioXASSideXASScanConfiguration::BioXASSideXASScanConfiguration(QObject *parent) :
	BioXASXASScanConfiguration(parent)
{

}

BioXASSideXASScanConfiguration::BioXASSideXASScanConfiguration(const BioXASSideXASScanConfiguration &original)
	: BioXASXASScanConfiguration(original)
{

}

BioXASSideXASScanConfiguration::~BioXASSideXASScanConfiguration()
{

}

AMScanConfiguration* BioXASSideXASScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASSideXASScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* BioXASSideXASScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASSideXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView* BioXASSideXASScanConfiguration::createView()
{
	return new BioXASSideXASScanConfigurationView(this);
}
