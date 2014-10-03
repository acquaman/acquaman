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

#include <QDebug>

#include "dataman/AMScanAxisEXAFSRegion.h"
#include "acquaman/BioXASSide/BioXASSideXASScanActionController.h"
#include "ui/BioXAS/BioXASSideXASScanConfigurationView.h"

BioXASSideXASScanConfiguration::BioXASSideXASScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent)
{
	setName("Unnamed Scan");
	setUserScanName("Unnamed Scan");

	edgeEnergy_ = -1;

	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
}

BioXASSideXASScanConfiguration::BioXASSideXASScanConfiguration(const BioXASSideXASScanConfiguration &original)
	: AMStepScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

	edgeEnergy_ = original.edgeEnergy();

	AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion*>(scanAxisAt(0)->regionAt(0));
}

BioXASSideXASScanConfiguration::~BioXASSideXASScanConfiguration()
{

}

AMScanConfiguration* BioXASSideXASScanConfiguration::createCopy() const
{
	return new BioXASSideXASScanConfiguration(*this);
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

QString BioXASSideXASScanConfiguration::detailedDescription() const
{
	return "BioXAS Side XAS Scan";
}

double BioXASSideXASScanConfiguration::edgeEnergy() const
{
	return edgeEnergy_;
}

void BioXASSideXASScanConfiguration::setEdgeEnergy(double edgeEnergy){
	if(edgeEnergy_ != edgeEnergy){
		edgeEnergy_ = edgeEnergy;
		emit edgeEnergyChanged(edgeEnergy_);
		setModified(true);
	}
}

void BioXASSideXASScanConfiguration::setEdge(QString edgeName)
{
	if (edge_ != edgeName){

		edge_ = edgeName;
		emit edgeChanged(edgeName);
		setModified(true);
	}
}
