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


#include "VESPERSEnergyScanConfiguration.h"

#include "ui/VESPERS/VESPERSEnergyScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSEnergyScanActionController.h"

VESPERSEnergyScanConfiguration::~VESPERSEnergyScanConfiguration(){}

VESPERSEnergyScanConfiguration::VESPERSEnergyScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent), VESPERSScanConfiguration()
{
	setName("EnergyScan");
	setUserScanName("EnergyScan");
	dbObject_->setParent(this);

	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	setMotor(VESPERS::H | VESPERS::V);
	setCCDDetector(VESPERS::Pilatus);
	setCCDFileName("");
	goToPosition_ = false;
	position_ = QPointF(0.0, 0.0);

	connect(axis, SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(onRegionAdded(AMScanAxisRegion*)));
	connect(axis, SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(onRegionRemoved(AMScanAxisRegion*)));
	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

VESPERSEnergyScanConfiguration::VESPERSEnergyScanConfiguration(const VESPERSEnergyScanConfiguration &original)
	: AMStepScanConfiguration(original), VESPERSScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());
	dbObject_->setParent(this);

	goToPosition_ = original.goToPosition();
	position_ = original.position();
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

AMScanConfiguration *VESPERSEnergyScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new VESPERSEnergyScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *VESPERSEnergyScanConfiguration::createController()
{
	AMScanActionController *controller = new VESPERSEnergyScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *VESPERSEnergyScanConfiguration::createView()
{
	return new VESPERSEnergyScanConfigurationView(this);
}

QString VESPERSEnergyScanConfiguration::technique() const
{
	return "Energy Scan";
}

QString VESPERSEnergyScanConfiguration::description() const
{
	return "Energy Scan";
}

QString VESPERSEnergyScanConfiguration::detailedDescription() const
{
	return QString("VESPERS Energy Scan");
}

QString VESPERSEnergyScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(ccdDetectorHeaderString(ccdDetector()));
	header.append(QString("Automatically moved to a specific location (used when setting up the workflow)?\t%1").arg(goToPosition() ? "Yes\n" : "No\n\n"));

	if (goToPosition()){

		header.append(QString("Horizontal Position:\t%1 mm\n").arg(x()));
		header.append(QString("Vertical Position:\t%1 mm\n\n").arg(y()));
	}

	if (normalPosition() != 888888.88){

		header.append("\n");
		header.append(QString("Focus position:\t%1 mm\n").arg(normalPosition()));
	}

	header.append("\n");
	header.append("Regions Scanned\n");

	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		header.append(QString("Start: %1 eV\tDelta: %2 eV\tEnd: %3 eV\tTime: %4 s\n")
						  .arg(double(region->regionStart()))
						  .arg(double(region->regionStep()))
						  .arg(double(region->regionEnd()))
						  .arg(double(region->regionTime())));
	}

	return header;
}

void VESPERSEnergyScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;
	double extraOffset = 0;

	if (ccdDetector() == VESPERS::Roper)
		extraOffset = 6;

	else if (ccdDetector() == VESPERS::Mar)
		extraOffset = 3;

	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList())
		time += region->numberOfPoints()*(double(region->regionTime()) + timeOffset_ + extraOffset); // Seems to take about 0.7 seconds for extra beamline stuff to happen.

	totalTime_ = time + 9; // There is a 9 second miscellaneous startup delay.
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void VESPERSEnergyScanConfiguration::setGoToPosition(bool state)
{
	if (goToPosition_ != state){

		goToPosition_ = state;
		emit gotoPositionChanged(goToPosition_);
		setModified(true);
	}
}

void VESPERSEnergyScanConfiguration::setPosition(const QPointF &pos)
{
	setX(pos.x());
	setY(pos.y());
}

void VESPERSEnergyScanConfiguration::setX(double xPos)
{
	if (position_.x() != xPos){

		position_.setX(xPos);
		emit xPositionChanged(xPos);
		setModified(true);
	}
}

void VESPERSEnergyScanConfiguration::setY(double yPos)
{
	if (position_.y() != yPos){

		position_.setY(yPos);
		emit yPositionChanged(yPos);
		setModified(true);
	}
}

void VESPERSEnergyScanConfiguration::onRegionAdded(AMScanAxisRegion *region)
{
		connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		computeTotalTime();
}

void VESPERSEnergyScanConfiguration::onRegionRemoved(AMScanAxisRegion *region)
{
	region->disconnect(this);
	region->deleteLater();
	computeTotalTime();
}
