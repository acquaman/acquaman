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

IDEASXASScanConfiguration::IDEASXASScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent)
{
	setAutoExportEnabled(false);

	setName("Unnamed Scan");
	setUserScanName("Unnamed Scan");
	isXRFScan_ = true;
	isTransScan_ = true;
	useRef_ = true;

	edge_ = "";
	energy_ = 0.0;
	useFixedTime_ = false;
	numberOfScans_ = 1;
	totalTime_ = 0;
	timeOffset_ = 0.4;
	minEnergy_ = 0;
	maxEnergy_ = 0;
	totalPoints_ = 0;
	fluorescenceDetector_ = IDEASXASScanConfiguration::Ketek;

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
	AMStepScanConfiguration(original)
{
	setAutoExportEnabled(false);

	setName(original.name());
	setUserScanName(original.userScanName());
	isXRFScan_ = original.isXRFScan();
	isTransScan_ = original.isTransScan();
	useRef_ = original.useRef();
	timeOffset_ = original.timeOffset();
	totalTime_ = original.totalTime();

	edge_ = original.edge();
	energy_ = original.energy();
	useFixedTime_ = original.useFixedTime();
	numberOfScans_ = original.numberOfScans();
	minEnergy_ = original.minEnergy();
	maxEnergy_ = original.maxEnergy();
	totalPoints_ = original.totalPoints();
	fluorescenceDetector_ = original.fluorescenceDetector();

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

AMScanConfiguration* IDEASXASScanConfiguration::createCopy() const{
	return new IDEASXASScanConfiguration(*this);
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

QString IDEASXASScanConfiguration::detailedDescription() const{
	return QString("XAS Scan from %1 to %2").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(scanAxisAt(0)->regionCount()-1)->regionEnd()));
}

QString IDEASXASScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append("Scanned Edge:\t" + edge() + "\n");

	return header;
}

void IDEASXASScanConfiguration::computeTotalTimeImplementation()
{
    double time = 0;
    totalPoints_ = 0;

    // Some region stuff.
    minEnergy_ = scanAxisAt(0)->regionAt(0)->regionStart();
    foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

	    AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);
	    int numberOfPoints = int((double(exafsRegion->regionEnd()) - double(exafsRegion->regionStart()))/double(exafsRegion->regionStep()) + 1);

	    if (numberOfPoints > 0) totalPoints_ += numberOfPoints;

	    if (exafsRegion->inKSpace()){
		if (exafsRegion->maximumTime().isValid() &&
			double(exafsRegion->maximumTime()) > double(exafsRegion->regionTime()) &&
			double(exafsRegion->regionTime()) > 0 &&
			double(exafsRegion->regionStep()) > 0 &&
			double(exafsRegion->regionEnd()) > double(exafsRegion->regionStart())){
		maxEnergy_ = AMEnergyToKSpaceCalculator::energy(energy_, exafsRegion->regionEnd());
		QVector<double> regionTimes = QVector<double>(numberOfPoints);
		AMVariableIntegrationTime calculator(exafsRegion->equation(), exafsRegion->regionTime(), exafsRegion->maximumTime(), exafsRegion->regionStart(), exafsRegion->regionStep(), exafsRegion->regionEnd(), exafsRegion->a2());
		calculator.variableTime(regionTimes.data());

		for (int i = 0; i < numberOfPoints; i++)
			time = time + regionTimes.at(i) + 0.54 + 0.25;  //0.25
		}
		else {
		    totalTime_ = -1; //negative value used to trigger feedback to user in IDEASXASScanConfigurationView... Hope this doesn't cause an issue elsewhere.
		    setExpectedDuration(totalTime_);
		    emit totalTimeChanged(totalTime_);
		    return;
		}
	    }


	    else{

		maxEnergy_ = exafsRegion->regionEnd();
		if (double(exafsRegion->regionTime()) > 0 &&
			double(exafsRegion->regionStep()) > 0 &&
			double(exafsRegion->regionEnd()) > double(exafsRegion->regionStart()))
		    time += (0.52 + double(exafsRegion->regionTime()))*numberOfPoints;
		else {
		    totalTime_ = -1; //negative value used to trigger feedback to user in IDEASXASScanConfigurationView... Hope this doesn't cause an issue elsewhere.
		    setExpectedDuration(totalTime_);
		    emit totalTimeChanged(totalTime_);
		    return;
		}
	    }
    }

    totalTime_ = time + 27; // There is a 27 second miscellaneous startup delay.
    setExpectedDuration(totalTime_);
    emit totalTimeChanged(totalTime_);

}

void IDEASXASScanConfiguration::setEdge(QString edgeName)
{
	if (edge_ != edgeName){

		edge_ = edgeName;
		emit edgeChanged(edgeName);
		setModified(true);
	}
}

void IDEASXASScanConfiguration::setEnergy(double edgeEnergy)
{
	if (energy_ != edgeEnergy){

		foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList())
			((AMScanAxisEXAFSRegion *)region)->setEdgeEnergy(edgeEnergy);

		energy_ = edgeEnergy;
		emit energyChanged(energy_);
		setModified(true);
	}
}

void IDEASXASScanConfiguration::setUseFixedTime(bool fixed)
{
	if (useFixedTime_ != fixed){

		useFixedTime_ = fixed;
		emit useFixedTimeChanged(useFixedTime_);
		computeTotalTime();
		setModified(true);
	}
}

void IDEASXASScanConfiguration::setNumberOfScans(int num)
{
	if (numberOfScans_ != num){

		numberOfScans_ = num;
		emit numberOfScansChanged(numberOfScans_);
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
	computeTotalTime();
}

void IDEASXASScanConfiguration::setIsXRFScan(bool isXRFScan)
{
	if(isXRFScan != isXRFScan_) {

		isXRFScan_ = isXRFScan;
		setModified(true);
		emit configurationChanged();
	}
}

void IDEASXASScanConfiguration::setIsTransScan(bool isTransScan)
{
	if(isTransScan != isTransScan_){

		isTransScan_ = isTransScan;
		setModified(true);
		emit configurationChanged();
	}
}

void IDEASXASScanConfiguration::setUseRef(bool useRef)
{
	if(useRef != useRef_){

		useRef_ = useRef;
		setModified(true);
		emit configurationChanged();
	}
}

void IDEASXASScanConfiguration::setFluorescenceDetector(IDEASXASScanConfiguration::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		emit fluorescenceDetectorChanged(fluorescenceDetector_);
		emit fluorescenceDetectorChanged(int(fluorescenceDetector_));
		setModified(true);
	}
}
