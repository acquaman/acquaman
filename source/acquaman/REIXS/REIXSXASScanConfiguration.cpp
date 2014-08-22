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


#include "REIXSXASScanConfiguration.h"
#include "beamline/REIXS/REIXSBeamline.h"

REIXSXASScanConfiguration::REIXSXASScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent)
{
	setAutoExportEnabled(false);

	scanNumber_ = 0;
	sampleId_ = -1;
	namedAutomatically_ = true;

	slitWidth_ = 50;
	applySlitWidth_ = false;
	monoGrating_ = 0;
	applyMonoGrating_ = false;
	monoMirror_ = 0;
	applyMonoMirror_ = false;
	polarization_ = 3;
	polarizationAngle_ = -90;
	applyPolarization_ = false;

	totalTime_ = 0;
	timeOffset_ = 0.4;
	minEnergy_ = 0;
	maxEnergy_ = 0;
	totalPoints_ = 0;

	AMScanAxisRegion *region = new AMScanAxisRegion;

	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
	region->setRegionStart(390);
	region->setRegionStep(0.5);
	region->setRegionEnd(430);
	region->setRegionTime(1.0);
	computeTotalTime();

	connect(axis, SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(onRegionAdded(AMScanAxisRegion*)));
	connect(axis, SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(onRegionRemoved(AMScanAxisRegion*)));
	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

REIXSXASScanConfiguration::~REIXSXASScanConfiguration() {

}

REIXSXASScanConfiguration::REIXSXASScanConfiguration(const REIXSXASScanConfiguration &other)
	: AMStepScanConfiguration(other)
{
	scanNumber_ = other.scanNumber_;
	sampleId_ = other.sampleId_;
	namedAutomatically_ = other.namedAutomatically_;

	slitWidth_ = other.slitWidth_;
	applySlitWidth_ = other.applySlitWidth_;
	monoGrating_ = other.monoGrating_;
	applyMonoGrating_ = other.applyMonoGrating_;
	monoMirror_ = other.monoMirror_;
	applyMonoMirror_ = other.applyMonoMirror_;
	polarization_ = other.polarization_;
	polarizationAngle_ = other.polarizationAngle_;
	applyPolarization_ = other.applyPolarization_;

	totalTime_ = 0;
	minEnergy_ = other.minEnergy();
	maxEnergy_ = other.maxEnergy();
	totalPoints_ = other.totalPoints();
	////////////////////////
	setAutoExportEnabled(false);

		computeTotalTime();

	connect(scanAxisAt(0), SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(onRegionAdded(AMScanAxisRegion*)));
	connect(scanAxisAt(0), SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(onRegionRemoved(AMScanAxisRegion*)));

	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	}
}

AMScanConfiguration * REIXSXASScanConfiguration::createCopy() const
{
	// using default copy constructor:
	return new REIXSXASScanConfiguration(*this);
}

#include "acquaman/REIXS/REIXSXASScanActionController.h"
AMScanController * REIXSXASScanConfiguration::createController()
{
	AMScanActionController *controller = new REIXSXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

#include "ui/REIXS/REIXSXASScanConfigurationView.h"
AMScanConfigurationView * REIXSXASScanConfiguration::createView()
{
	return new REIXSXASScanConfigurationView(this);
}

QString REIXSXASScanConfiguration::detailedDescription() const{
	return QString("XAS Scan from %1 to %2").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(scanAxisAt(0)->regionCount()-1)->regionEnd()));
}

QString REIXSXASScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(QString("XAS Scan from %1 to %2").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(scanAxisAt(0)->regionCount()-1)->regionEnd())));

	return header;
}

void REIXSXASScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;
	totalPoints_ = 0;

	// Some region stuff.
	minEnergy_ = scanAxisAt(0)->regionAt(0)->regionStart();
	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		int numberOfPoints = int((double(region->regionEnd()) - double(region->regionStart()))/double(region->regionStep()) + 1);
		totalPoints_ += numberOfPoints;
		maxEnergy_ = region->regionEnd();
		time += (double(region->regionStep()) + double(region->regionTime()))*numberOfPoints;  //The time it takes to move the mono is really close to the step size, will require tweaking.
	}

	totalTime_ = time + 5; // There is a 5 second miscellaneous startup delay.
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);

}



void REIXSXASScanConfiguration::onRegionAdded(AMScanAxisRegion *region)
{
	if (region){

		connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		computeTotalTime();
	}
}

void REIXSXASScanConfiguration::onRegionRemoved(AMScanAxisRegion *region)
{
	region->disconnect(this);
	computeTotalTime();
}

