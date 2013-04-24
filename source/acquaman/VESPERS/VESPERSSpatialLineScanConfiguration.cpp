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


#include "VESPERSSpatialLineScanConfiguration.h"

#include "acquaman/VESPERS/VESPERSSpatialLineDacqScanController.h"
#include "ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QStringBuilder>

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(QObject *parent)
	: AMRegionScanConfiguration(parent), VESPERSScanConfiguration()
{
	setName("Line Scan");
	setUserScanName("Line Scan");
	dbObject_->setParent(this);
	regions_->setDefaultControl(VESPERSBeamline::vespers()->pseudoSampleStage()->horiz());
	regions_->setDefaultTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	regions_->setSensibleRange(0, 10);
	setIncomingChoice(VESPERS::Imini);
	setFluorescenceDetector(VESPERS::SingleElement);
	setMotor(VESPERS::H);
	setCCDDetector(VESPERS::NoCCD);
	setCCDFileName("");
	setRoiInfoList(AMROIInfoList());
	setExportSpectraSources(true);
	setOtherPosition(-123456789.0);
	connect(this, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(startChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(stepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(endChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(dbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(const VESPERSSpatialLineScanConfiguration &original)
	: AMRegionScanConfiguration(original), VESPERSScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
	dbObject_->setParent(this);

	switch(int(original.motor())){

	case VESPERS::H:
		regions_->setDefaultControl(VESPERSBeamline::vespers()->pseudoSampleStage()->horiz());
		break;

	case VESPERS::X:
		regions_->setDefaultControl(VESPERSBeamline::vespers()->realSampleStage()->horiz());
		break;

	case VESPERS::V:
		regions_->setDefaultControl(VESPERSBeamline::vespers()->pseudoSampleStage()->vert());
		break;

	case VESPERS::Z:
		regions_->setDefaultControl(VESPERSBeamline::vespers()->realSampleStage()->vert());
		break;
	}

	regions_->setDefaultTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	regions_->addRegion(0, original.start(), original.step(), original.end(), original.time());
	regions_->setSensibleRange(original.regions()->sensibleStart(), original.regions()->sensibleEnd());
	regions_->setUnits(0, original.regions()->units(0));
	regions_->setTimeUnits(0, original.regions()->timeUnits(0));
	setOtherPosition(original.otherPosition());
	setExportSpectraSources(original.exportSpectraSources());
	computeTotalTime();
	connect(this, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(startChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(stepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(endChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(dbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERSSpatialLineScanConfiguration::createCopy() const
{
	return new VESPERSSpatialLineScanConfiguration(*this);
}

AMScanController *VESPERSSpatialLineScanConfiguration::createController()
{
	return new VESPERSSpatialLineDacqScanController(this);
}

AMScanConfigurationView *VESPERSSpatialLineScanConfiguration::createView()
{
	return new VESPERSSpatialLineScanConfigurationView(this);
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
	header.append(regionOfInterestHeaderString(roiList()) % "\n");
	header.append(motorHeaderString(motor()));
	header.append(ccdDetectorHeaderString(ccdDetector()));

	header.append("\n");
	header.append("Line Dimensions\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(start()).arg(end()));
	header.append(QString("Step Size:\t%1 mm\n").arg(step()));

	if (hasOtherPosition())
		header.append(QString("%1 position: %2 mm\n").arg(otherMotorString(motor())).arg(otherPosition()));

	return header;
}

void VESPERSSpatialLineScanConfiguration::setStart(double position)
{
	if (start() != position){

		regions_->setStart(0, position);
		emit startChanged(position);
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setEnd(double position)
{
	if (end() != position){

		regions_->setEnd(0, position);
		emit endChanged(position);
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setStep(double position)
{
	if (step() != position){

		regions_->setDelta(0, position);
		emit stepChanged(position);
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setTime(double newTime)
{
	if (time() != newTime){

		regions_->setTime(0, newTime);
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
	totalTime = fabs((end()-start())/step()) + 1;

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

VESPERS::Motor VESPERSSpatialLineScanConfiguration::otherMotor(VESPERS::Motor motor) const
{
	VESPERS::Motor other = VESPERS::NoMotor;

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

	default:
		other = VESPERS::NoMotor;
		break;
	}

	return other;
}

QString VESPERSSpatialLineScanConfiguration::otherMotorString(VESPERS::Motor motor) const
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

	default:
		string = "";
		break;
	}

	return string;
}
