/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "acquaman/VESPERS/VESPERSEnergyDacqScanController.h"
#include "ui/VESPERS/VESPERSEnergyScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "acquaman/VESPERS/VESPERSEnergyScanActionController.h"

VESPERSEnergyScanConfiguration::VESPERSEnergyScanConfiguration(QObject *parent)
	: AMXASScanConfiguration(parent), VESPERSScanConfiguration()
{
	setName("EnergyScan");
	setUserScanName("EnergyScan");
	dbObject_->setParent(this);
	regions_->setSensibleRange(10000, 20000);
	xasRegions()->setEnergyControl(VESPERSBeamline::vespers()->energy());
	regions_->setDefaultTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	regions_->setDefaultUnits(" eV");
	regions_->setDefaultTimeUnits(" s");

	setMotor(VESPERS::H | VESPERS::V);
	setCCDDetector(VESPERS::Pilatus);
	setCCDFileName("");
	goToPosition_ = false;
	position_ = QPointF(0.0, 0.0);
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
}

VESPERSEnergyScanConfiguration::VESPERSEnergyScanConfiguration(const VESPERSEnergyScanConfiguration &original)
	: AMXASScanConfiguration(original), VESPERSScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
	dbObject_->setParent(this);
	regions_->setSensibleRange(original.regions()->sensibleStart(), original.regions()->sensibleEnd());
	xasRegions()->setEnergyControl(VESPERSBeamline::vespers()->energy());
	regions_->setDefaultTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());

	for (int i = 0; i < original.regionCount(); i++)
		regions_->addRegion(i, original.regionStart(i), original.regionDelta(i), original.regionEnd(i), original.regionTime(i));

	goToPosition_ = original.goToPosition();
	position_ = original.position();
	computeTotalTime();
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERSEnergyScanConfiguration::createCopy() const
{
	return new VESPERSEnergyScanConfiguration(*this);
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

	header.append("\n");
	header.append("Regions Scanned\n");

	for (int i = 0; i < regionCount(); i++){

		header.append(QString("Start: %1 eV\tDelta: %2 eV\tEnd: %3 eV\tTime: %4 s\n")
						  .arg(regionStart(i))
						  .arg(regionDelta(i))
						  .arg(regionEnd(i))
						  .arg(regionTime(i)));
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

	for (int i = 0; i < regions_->count(); i++)
		time += ((regions_->end(i) - regions_->start(i))/regions_->delta(i))*(regions_->time(i) + timeOffset_ + extraOffset); // Seems to take about 0.7 seconds for extra beamline stuff to happen.

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
