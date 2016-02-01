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


#include "IDEASXASScanConfiguration.h"

#include "beamline/AMBeamline.h"
#include "acquaman/IDEAS/IDEASXASScanActionController.h"
#include "ui/IDEAS/IDEASXASScanConfigurationView.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "util/AMEnergyToKSpaceCalculator.h"

#include <QStringBuilder>

IDEASXASScanConfiguration::IDEASXASScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent), IDEASScanConfiguration()
{
    setName("Unnamed Scan");
	setUserScanName("Unnamed Scan");

	dbObject_->setParent(this);
	edge_ = "";
	usingTransmission_ = true;
	usingReference_ = true;

	AMScanAxisRegion *region = new AMScanAxisEXAFSRegion;
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

	computeTotalTime();
}

IDEASXASScanConfiguration::IDEASXASScanConfiguration(const IDEASXASScanConfiguration &original) :
	AMStepScanConfiguration(original), IDEASScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

	dbObject_->setParent(this);
	edge_ = original.edge();
	usingTransmission_ = original.usingTransmission();
	usingReference_ = original.usingReference();

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

IDEASXASScanConfiguration::~IDEASXASScanConfiguration()
{

}

AMScanConfiguration* IDEASXASScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new IDEASXASScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* IDEASXASScanConfiguration::createController()
{
	IDEASXASScanActionController *controller = new IDEASXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView* IDEASXASScanConfiguration::createView(){
	return new IDEASXASScanConfigurationView(this);
}

QString IDEASXASScanConfiguration::technique() const
{
	return "XAS";
}

QString IDEASXASScanConfiguration::description() const
{
	return "IDEAS XAS Scan";
}

QString IDEASXASScanConfiguration::detailedDescription() const{
	return QString("XAS Scan from %1 to %2").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(scanAxisAt(0)->regionCount()-1)->regionEnd()));
}

QString IDEASXASScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(fluorescenceHeaderString(fluorescenceDetector()));
	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");

	header.append("Scanned Edge:\t" % edge() % "\n");

	return header;
}

void IDEASXASScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;

	// Some region stuff.
	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);
		int numberOfPoints = int((double(exafsRegion->regionEnd()) - double(exafsRegion->regionStart()))/double(exafsRegion->regionStep()) + 1);

		if (exafsRegion->inKSpace()){

			if (exafsRegion->maximumTime().isValid() &&
				double(exafsRegion->maximumTime()) > double(exafsRegion->regionTime()) &&
				double(exafsRegion->regionTime()) > 0 &&
				double(exafsRegion->regionStep()) > 0 &&
				double(exafsRegion->regionEnd()) > double(exafsRegion->regionStart())){

				QVector<double> regionTimes = QVector<double>(numberOfPoints);
				AMVariableIntegrationTime calculator(exafsRegion->equation(), exafsRegion->regionTime(), exafsRegion->maximumTime(), exafsRegion->regionStart(), exafsRegion->regionStep(), exafsRegion->regionEnd(), exafsRegion->a2());
				calculator.variableTime(regionTimes.data());

				for (int i = 0; i < numberOfPoints; i++)
					time = time + regionTimes.at(i) + 0.54 + 0.25;

				totalTime_ = time + 27; // There is a 27 second miscellaneous startup delay.
				setExpectedDuration(totalTime_);
				emit totalTimeChanged(totalTime_);
				emit configurationChanged();
			}

			else {

				totalTime_ = -1; //negative value used to trigger feedback to user in IDEASXASScanConfigurationView... Hope this doesn't cause an issue elsewhere.
				setExpectedDuration(totalTime_);
				emit totalTimeChanged(totalTime_);
				emit configurationChanged();
			}
		}


		else{

			if (double(exafsRegion->regionTime()) > 0 &&
				double(exafsRegion->regionStep()) > 0 &&
				double(exafsRegion->regionEnd()) > double(exafsRegion->regionStart())){

				time += (0.52 + double(exafsRegion->regionTime()))*numberOfPoints;
				totalTime_ = time + 27; // There is a 27 second miscellaneous startup delay.
				setExpectedDuration(totalTime_);
				emit totalTimeChanged(totalTime_);
				emit configurationChanged();
			}
			else {

				totalTime_ = -1; //negative value used to trigger feedback to user in IDEASXASScanConfigurationView... Hope this doesn't cause an issue elsewhere.
				setExpectedDuration(totalTime_);
				emit totalTimeChanged(totalTime_);
				emit configurationChanged();
			}
		}
	}
}

void IDEASXASScanConfiguration::setEdge(QString edgeName)
{
	if (edge_ != edgeName){

		edge_ = edgeName;
		emit edgeChanged(edgeName);
		setModified(true);
	}
}

void IDEASXASScanConfiguration::onRegionAdded(AMScanAxisRegion *region)
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

void IDEASXASScanConfiguration::onRegionRemoved(AMScanAxisRegion *region)
{
	region->disconnect(this);
	region->deleteLater();
	computeTotalTime();
}

void IDEASXASScanConfiguration::setUsingTransmission(bool isTransScan)
{
	if(isTransScan != usingTransmission_){

		usingTransmission_ = isTransScan;
		setModified(true);
		emit configurationChanged();
	}
}

void IDEASXASScanConfiguration::setUsingReference(bool useRef)
{
	if(useRef != usingReference_){

		usingReference_ = useRef;
		setModified(true);
		emit configurationChanged();
	}
}
