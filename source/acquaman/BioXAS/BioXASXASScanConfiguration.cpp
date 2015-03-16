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


#include "BioXASXASScanConfiguration.h"

#include "dataman/AMScanAxisEXAFSRegion.h"
#include "acquaman/BioXAS/BioXASXASScanActionController.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"

BioXASXASScanConfiguration::BioXASXASScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent)
{
	setName("Unnamed Scan");
	setUserScanName("Unnamed Scan");

	edgeEnergy_ = -1;

	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
}

BioXASXASScanConfiguration::BioXASXASScanConfiguration(const BioXASXASScanConfiguration &original)
	: AMStepScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

	edgeEnergy_ = original.edgeEnergy();
}

BioXASXASScanConfiguration::~BioXASXASScanConfiguration()
{

}

AMScanConfiguration* BioXASXASScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASXASScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* BioXASXASScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView* BioXASXASScanConfiguration::createView()
{
	return new BioXASXASScanConfigurationView(this);
}

QString BioXASXASScanConfiguration::detailedDescription() const
{
	return "BioXAS XAS Scan";
}

double BioXASXASScanConfiguration::edgeEnergy() const
{
	return edgeEnergy_;
}

void BioXASXASScanConfiguration::setEdgeEnergy(double edgeEnergy){
	if(edgeEnergy_ != edgeEnergy){
		edgeEnergy_ = edgeEnergy;
		emit edgeEnergyChanged(edgeEnergy_);
		setModified(true);
	}
}

void BioXASXASScanConfiguration::setEdge(QString edgeName)
{
	if (edge_ != edgeName){

		edge_ = edgeName;
		emit edgeChanged(edgeName);
		setModified(true);
	}
}
