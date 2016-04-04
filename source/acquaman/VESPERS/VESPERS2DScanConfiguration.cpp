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


#include "VESPERS2DScanConfiguration.h"

#include <QStringBuilder>
#include <cmath>

#include "ui/VESPERS/VESPERS2DScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERS2DScanActionController.h"

VESPERS2DScanConfiguration::~VESPERS2DScanConfiguration(){}

VESPERS2DScanConfiguration::VESPERS2DScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent), VESPERSScanConfiguration()
{
	setName("2D Map");
	setUserScanName("2D Map");
	dbObject_->setParent(this);
	setIncomingChoice(VESPERS::Imini);
	setFluorescenceDetector(VESPERS::SingleElement);
	setMotor(VESPERS::Motors(VESPERS::H | VESPERS::V));
	setCCDDetector(VESPERS::NoCCD);
	setCCDFileName("");
	setExportAsAscii(true);
	setExportSpectraSources(true);
	setExportSpectraInRows(true);
	setCloseFastShutter(true);
	setReturnToOriginalPosition(false);

	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
	region = new AMScanAxisRegion;
	axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(dbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERS2DScanConfiguration::VESPERS2DScanConfiguration(const VESPERS2DScanConfiguration &original)
	: AMStepScanConfiguration(original), VESPERSScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());
	dbObject_->setParent(this);
	setExportAsAscii(original.exportAsAscii());
	setExportSpectraSources(original.exportSpectraSources());
	setExportSpectraInRows(original.exportSpectraInRows());
	setCloseFastShutter(original.closeFastShutter());
	setReturnToOriginalPosition(original.returnToOriginalPosition());
	computeTotalTime();
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(dbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERS2DScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new VESPERS2DScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *VESPERS2DScanConfiguration::createController()
{
	AMScanActionController *controller = new VESPERS2DScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *VESPERS2DScanConfiguration::createView()
{
	return new VESPERS2DScanConfigurationView(this);
}

QString VESPERS2DScanConfiguration::technique() const
{
	return "2D";
}

QString VESPERS2DScanConfiguration::description() const
{
	return "2D Scan";
}

QString VESPERS2DScanConfiguration::detailedDescription() const
{
	if (ccdDetector() != VESPERS::NoCCD)
		return "Spatial x-ray fluorescence 2D map using a CCD for x-ray diffraction.";

	return "Spatial x-ray fluorescence 2D map";
}

QString VESPERS2DScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(fluorescenceHeaderString(fluorescenceDetector()));
	header.append(incomingChoiceHeaderString(incomingChoice()));
	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");
	header.append(motorHeaderString(motor()));
	header.append(ccdDetectorHeaderString(ccdDetector()));

	header.append("\n");
	header.append("Map Dimensions\n");
	header.append("X Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(0)->regionAt(0)->regionStep())));
	header.append("Y Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(1)->regionAt(0)->regionStart())).arg(double(scanAxisAt(1)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(1)->regionAt(0)->regionStep())));

	if (normalPosition() != 888888.88){

		header.append("\n");
		header.append(QString("Focus position:\t%1 mm\n").arg(normalPosition()));
	}

	return header;
}

QString VESPERS2DScanConfiguration::xAxisName() const
{
	VESPERS::Motors motors = motor();

	if (motors == (VESPERS::H | VESPERS::V))
		return "Horizontal (H)";

	else if (motors == (VESPERS::X | VESPERS::Z))
		return "Horizontal (X)";

	return "Horizontal";
}

QString VESPERS2DScanConfiguration::xAxisUnits() const
{
	VESPERS::Motors motors = motor();

	if (motors == (VESPERS::H | VESPERS::V))
		return "mm";

	else if (motors == (VESPERS::X | VESPERS::Z))
		return "mm";

	return "mm";
}

QString VESPERS2DScanConfiguration::yAxisName() const
{
	VESPERS::Motors motors = motor();

	if (motors == (VESPERS::H | VESPERS::V))
		return "Vertical (V)";

	else if (motors == (VESPERS::X | VESPERS::Z))
		return "Vertical (Z)";

	return "Vertical";
}

QString VESPERS2DScanConfiguration::yAxisUnits() const
{
	VESPERS::Motors motors = motor();

	if (motors == (VESPERS::H | VESPERS::V))
		return "mm";

	else if (motors == (VESPERS::X | VESPERS::Z))
		return "mm";

	return "mm";
}

void VESPERS2DScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;

	// Get the number of points.
	time = 	scanAxisAt(0)->numberOfPoints() * scanAxisAt(1)->numberOfPoints();

	// Factor in the time per point.  There is an extra 6 seconds for CCD images for the Roper and Mar.
	if (ccdDetector() == VESPERS::Roper)
		time *= double(scanAxisAt(0)->regionAt(0)->regionTime()) + timeOffset_ + 6.0;
	else if (ccdDetector() == VESPERS::Mar)
		time *= double(scanAxisAt(0)->regionAt(0)->regionTime()) + timeOffset_ + 3.0;
	else
		time *= double(scanAxisAt(0)->regionAt(0)->regionTime()) + timeOffset_;

	totalTime_ = time + 9;
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void VESPERS2DScanConfiguration::setExportAsAscii(bool exportAsAscii)
{
	if (exportAsAscii_ == exportAsAscii)
		return;

	exportAsAscii_ = exportAsAscii;
}

void VESPERS2DScanConfiguration::setExportSpectraSources(bool exportSpectra)
{
	if (exportSpectraSources_ == exportSpectra)
		return;

	exportSpectraSources_ = exportSpectra;
}

void VESPERS2DScanConfiguration::setExportSpectraInRows(bool exportInRows)
{
	if (exportSpectraInRows_ == exportInRows)
		return;

	exportSpectraInRows_ = exportInRows;
}
