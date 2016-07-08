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


#include "VESPERSSpatialLineScanConfiguration.h"

#include "ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSSpatialLineScanActionController.h"

#include <QStringBuilder>
#include <math.h>

VESPERSSpatialLineScanConfiguration::~VESPERSSpatialLineScanConfiguration(){}

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent), VESPERSScanConfiguration()
{
	setName("Line Scan");
	setUserScanName("Line Scan");
	vespersScanConfigurationDbObject_->setParent(this);
	setIncomingChoice(VESPERS::Imini);
	setFluorescenceDetector(VESPERS::SingleElement);
	setMotor(VESPERS::H);
	setCCDDetector(VESPERS::NoCCD);
	setCCDFileName("");
	setExportSpectraSources(true);
	setExportSpectraInRows(true);
	setOtherPosition(-123456789.0);
	setCloseFastShutter(true);
	setReturnToOriginalPosition(false);

	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(this, SIGNAL(startChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(stepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(endChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(vespersScanConfigurationDbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(const VESPERSSpatialLineScanConfiguration &original)
	: AMStepScanConfiguration(original), VESPERSScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());
	vespersScanConfigurationDbObject_->setParent(this);
	setOtherPosition(original.otherPosition());
	setExportSpectraSources(original.exportSpectraSources());
	setCloseFastShutter(original.closeFastShutter());
	setReturnToOriginalPosition(original.returnToOriginalPosition());
	computeTotalTime();

	connect(this, SIGNAL(startChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(stepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(endChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(vespersScanConfigurationDbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERSSpatialLineScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new VESPERSSpatialLineScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *VESPERSSpatialLineScanConfiguration::createController()
{
	AMScanActionController *controller = new VESPERSSpatialLineScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *VESPERSSpatialLineScanConfiguration::createView()
{
	return new VESPERSSpatialLineScanConfigurationView(this);
}

QString VESPERSSpatialLineScanConfiguration::technique() const
{
	return "Line";
}

QString VESPERSSpatialLineScanConfiguration::description() const
{
	return "Line Scan";
}

QString VESPERSSpatialLineScanConfiguration::detailedDescription() const
{
	if (ccdDetector() != VESPERS::NoCCD)
		return "Spatial x-ray fluorescence line scan using a CCD for x-ray diffraction.";

	return "Spatial x-ray fluorescence line scan";
}

QString VESPERSSpatialLineScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(fluorescenceHeaderString(fluorescenceDetector()));
	header.append(incomingChoiceHeaderString(incomingChoice()));
	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");
	header.append(motorHeaderString(motor()));
	header.append(ccdDetectorHeaderString(ccdDetector()));

	header.append("\n");
	header.append("Line Dimensions\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(start()).arg(end()));
	header.append(QString("Step Size:\t%1 mm\n").arg(step()));

	if (normalPosition() != 888888.88){

		header.append("\n");
		header.append(QString("Focus position:\t%1 mm\n").arg(normalPosition()));
	}

	if (hasOtherPosition())
		header.append(QString("%1 position: %2 mm\n").arg(otherMotorString(motor())).arg(otherPosition()));

	return header;
}

void VESPERSSpatialLineScanConfiguration::setStart(double position)
{
	if (start() != position){

		scanAxisAt(0)->regionAt(0)->setRegionStart(position);
		emit startChanged(position);
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setEnd(double position)
{
	if (end() != position){

		scanAxisAt(0)->regionAt(0)->setRegionEnd(position);
		emit endChanged(position);
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setStep(double position)
{
	if (step() != position){

		scanAxisAt(0)->regionAt(0)->setRegionStep(position);
		emit stepChanged(position);
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setTime(double newTime)
{
	if (time() != newTime){

		scanAxisAt(0)->regionAt(0)->setRegionTime(newTime);
		emit timeChanged(newTime);
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setOtherPosition(double position)
{
	if (otherPosition() != position){

		otherPosition_ = position;
		emit otherPositionChanged(position);
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::computeTotalTimeImplementation()
{
	double totalTime = 0;

	// Get the number of points.  Using 0 for the index because this will only have one region per scan.
	totalTime = scanAxisAt(0)->regionAt(0)->numberOfPoints();

	// Factor in the time per point.  There is an extra 6 seconds for CCD images for the Roper and Mar.
	if (ccdDetector() == VESPERS::Roper)
		totalTime *= time() + timeOffset_ + 6.0;
	else if (ccdDetector() == VESPERS::Mar)
		totalTime *= time() + timeOffset_ + 3.0;
	else
		totalTime *= time() + timeOffset_;

	totalTime_ = totalTime + 9;
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void VESPERSSpatialLineScanConfiguration::setExportSpectraSources(bool exportSpectra)
{
	if (exportSpectraSources_ == exportSpectra)
		return;

	exportSpectraSources_ = exportSpectra;
}

VESPERS::Motors VESPERSSpatialLineScanConfiguration::otherMotor(VESPERS::Motors motor) const
{
	VESPERS::Motors other = VESPERS::NoMotor;

	switch(motor){

	case VESPERS::H:
		other = VESPERS::V;
		break;

	case VESPERS::V:
		other = VESPERS::H;
		break;

	case VESPERS::X:
		other = VESPERS::Z;
		break;

	case VESPERS::Z:
		other = VESPERS::X;
		break;

	case VESPERS::AttoH:
		other = VESPERS::AttoV;
		break;

	case VESPERS::AttoV:
		other = VESPERS::AttoH;
		break;

	case VESPERS::AttoX:
		other = VESPERS::AttoZ;
		break;

	case VESPERS::AttoZ:
		other = VESPERS::AttoX;
		break;

	case VESPERS::AttoRx:
		other = VESPERS::NoMotor;
		break;

	case VESPERS::AttoRy:
		other = VESPERS::NoMotor;
		break;

	case VESPERS::AttoRz:
		other = VESPERS::NoMotor;
		break;

	case VESPERS::BigBeamX:
		other = VESPERS::BigBeamZ;
		break;

	case VESPERS::BigBeamZ:
		other = VESPERS::BigBeamX;
		break;

	case VESPERS::WireH:
		other = VESPERS::WireV;
		break;

	case VESPERS::WireV:
		other = VESPERS::WireH;
		break;

	default:
		other = VESPERS::NoMotor;
		break;
	}

	return other;
}

QString VESPERSSpatialLineScanConfiguration::otherMotorString(VESPERS::Motors motor) const
{
	QString string;

	switch(motor){

	case VESPERS::H:
		string = "Vertical motor, V";
		break;

	case VESPERS::V:
		string = "Horizontal motor, H";
		break;

	case VESPERS::X:
		string = "Vertical motor, Z";
		break;

	case VESPERS::Z:
		string = "Horizontal motor, X";
		break;

	case VESPERS::AttoH:
		string = "Horizontal Attocube motor, V";
		break;

	case VESPERS::AttoV:
		string = "Horizontal Attocube motor, H";
		break;

	case VESPERS::AttoX:
		string = "Vertical Attocube motor, Z";
		break;

	case VESPERS::AttoZ:
		string = "Horizontal Attocube motor, X";
		break;

	case VESPERS::AttoRx:
		string = "";
		break;

	case VESPERS::AttoRy:
		string = "";
		break;

	case VESPERS::AttoRz:
		string = "";
		break;

	case VESPERS::BigBeamX:
		string = "Vertical motor, Z";
		break;

	case VESPERS::BigBeamZ:
		string = "Horizontal motor, X";
		break;

	case VESPERS::WireH:
		string = "Vertical motor, V";
		break;

	case VESPERS::WireV:
		string = "Horizontal motor, H";
		break;

	case VESPERS::WireN:
		string = "";
		break;

	default:
		string = "";
		break;
	}

	return string;
}

void VESPERSSpatialLineScanConfiguration::setExportSpectraInRows(bool exportInRows)
{
	if (exportSpectraInRows_ == exportInRows)
		return;

	exportSpectraInRows_ = exportInRows;
}

