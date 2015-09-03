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
	AMStepScanConfiguration(parent), BioXASScanConfiguration()
{
	setName("XAS Scan");
	setUserScanName("XAS Scan");

	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(axis, SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(onRegionAdded(AMScanAxisRegion*)));
	connect(axis, SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(onRegionRemoved(AMScanAxisRegion*)));
	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(maximumTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(equationChanged(AMVariableIntegrationTime::Equation)), this, SLOT(computeTotalTime()));
}

BioXASSideXASScanConfiguration::BioXASSideXASScanConfiguration(const BioXASSideXASScanConfiguration &original)
	: AMStepScanConfiguration(original), BioXASScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

	computeTotalTime();

	connect(scanAxisAt(0), SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(onRegionAdded(AMScanAxisRegion*)));
	connect(scanAxisAt(0), SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(onRegionRemoved(AMScanAxisRegion*)));

	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);
		connect(exafsRegion, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(maximumTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(equationChanged(AMVariableIntegrationTime::Equation)), this, SLOT(computeTotalTime()));
	}
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

QString BioXASSideXASScanConfiguration::technique() const
{
	return "XAS Scan";
}

QString BioXASSideXASScanConfiguration::description() const
{
	return "XAS Scan";
}

QString BioXASSideXASScanConfiguration::detailedDescription() const
{
	return "XAS Scan";
}

QString BioXASSideXASScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append("Scanned Edge:\t" + edge() + "\n");

	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");
	header.append("\n");
	header.append("Regions Scanned\n");
	header.append(scanAxisAt(0)->toString("eV"));

	return header;
}

void BioXASSideXASScanConfiguration::computeTotalTimeImplementation()
{
	totalTime_ = scanAxisAt(0)->timePerAxis();
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void BioXASSideXASScanConfiguration::onRegionAdded(AMScanAxisRegion *region)
{
	AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);

	if (exafsRegion){

		connect(exafsRegion, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(maximumTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(equationChanged(AMVariableIntegrationTime::Equation)), this, SLOT(computeTotalTime()));
		computeTotalTime();
	}
}

void BioXASSideXASScanConfiguration::onRegionRemoved(AMScanAxisRegion *region)
{
	region->disconnect(this);
	region->deleteLater();
	computeTotalTime();
}
