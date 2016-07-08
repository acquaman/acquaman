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


#include "VESPERSTimeScanConfiguration.h"

#include "acquaman/VESPERS/VESPERSTimeScanActionController.h"
#include "ui/VESPERS/VESPERSTimeScanConfigurationView.h"

VESPERSTimeScanConfiguration::VESPERSTimeScanConfiguration(QObject *parent)
	: AMTimedRegionScanConfiguration(parent), VESPERSScanConfiguration()
{
	setName("Timed Scan");
	setUserScanName("Timed Scan");
	vespersScanConfigurationDbObject_->setParent(this);
	setIncomingChoice(VESPERS::Imini);
	setFluorescenceDetector(VESPERS::SingleElement);
	setCCDDetector(VESPERS::NoCCD);
	setCCDFileName("");
	setExportSpectraSources(true);
	setExportSpectraInRows(true);
	setTime(1);
	setTimePerAcquisition(10);
	setIterations(1);
	computeTotalTime();

	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timePerAcquisitionChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(iterationsChanged(int)), this, SLOT(computeTotalTime()));
	connect(vespersScanConfigurationDbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERSTimeScanConfiguration::VESPERSTimeScanConfiguration(const VESPERSTimeScanConfiguration &original)
	: AMTimedRegionScanConfiguration(original), VESPERSScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());
	vespersScanConfigurationDbObject_->setParent(this);
	setExportSpectraSources(original.exportSpectraSources());
	computeTotalTime();

	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timePerAcquisitionChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(iterationsChanged(int)), this, SLOT(computeTotalTime()));
	connect(vespersScanConfigurationDbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERSTimeScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new VESPERSTimeScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *VESPERSTimeScanConfiguration::createController()
{
	AMScanActionController *controller = new VESPERSTimeScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *VESPERSTimeScanConfiguration::createView()
{
	return new VESPERSTimeScanConfigurationView(this);
}

QString VESPERSTimeScanConfiguration::technique() const
{
	return "Timed";
}

QString VESPERSTimeScanConfiguration::description() const
{
	return "Timed Scan";
}

QString VESPERSTimeScanConfiguration::detailedDescription() const
{
	if (ccdDetector() != VESPERS::NoCCD)
		return "Timed x-ray fluorescence scan using a CCD for x-ray diffraction.";

	return "Timed x-ray fluorescence scan";
}

QString VESPERSTimeScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(fluorescenceHeaderString(fluorescenceDetector()));
	header.append(incomingChoiceHeaderString(incomingChoice()));
	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");
	header.append(ccdDetectorHeaderString(ccdDetector()));

	header.append("\n");

	header.append(QString("Acquired for %1 seconds every %2 seconds %3 times.\n").arg(time()).arg(timePerAcquisition()).arg(iterations()));
	return header;
}

void VESPERSTimeScanConfiguration::computeTotalTimeImplementation()
{
	double totalTime = 0;

	// Factor in the time per point.  There is an extra 6 seconds for CCD images for the Roper and Mar.
	if (ccdDetector() == VESPERS::Roper)
		totalTime += timePerAcquisition() + timeOffset_ + 6.0;
	else if (ccdDetector() == VESPERS::Mar)
		totalTime += timePerAcquisition() + timeOffset_ + 3.0;
	else
		totalTime += timePerAcquisition() + timeOffset_;

	totalTime_ = iterations_*totalTime;
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void VESPERSTimeScanConfiguration::setExportSpectraSources(bool exportSpectra)
{
	if (exportSpectraSources_ == exportSpectra)
		return;

	exportSpectraSources_ = exportSpectra;
}


void VESPERSTimeScanConfiguration::setExportSpectraInRows(bool exportInRows)
{
	if (exportSpectraInRows_ == exportInRows)
		return;

	exportSpectraInRows_ = exportInRows;
}
