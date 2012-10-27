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

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(QObject *parent)
	: AMRegionScanConfiguration(parent)
{
	setName("Line Scan");
	setUserScanName("Line Scan");
	regions_->setDefaultControl(VESPERSBeamline::vespers()->pseudoSampleStage()->horiz());
	regions_->setDefaultTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	regions_->setSensibleRange(0, 10);
	I0_ = VESPERS::Imini;
	fluorescenceDetector_ = VESPERS::SingleElement;
	motor_ = VESPERS::H;
	ccdDetector_ = VESPERS::NoCCD;
	ccdFileName_ = "";
	roiInfoList_ = AMROIInfoList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	setExportSpectraSources(true);
	connect(this, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(startChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(stepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(endChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(const VESPERSSpatialLineScanConfiguration &original)
	: AMRegionScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

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
	I0_ = original.incomingChoice();
	fluorescenceDetector_ = original.fluorescenceDetector();
	motor_ = original.motor();
	ccdDetector_ = original.ccdDetector();
	ccdFileName_ = original.ccdFileName();
	roiInfoList_ = original.roiList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	setExportSpectraSources(original.exportSpectraSources());
	computeTotalTime();
	connect(this, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(startChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(stepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(endChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
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
	if (ccdDetector_ == VESPERS::Roper || ccdDetector_ == VESPERS::Mar)
		return "Spatial x-ray fluorescence line scan using a CCD for x-ray diffraction.";

	return "Spatial x-ray fluorescence line scan";
}

QString VESPERSSpatialLineScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	switch((int)fluorescenceDetector()){

	case VESPERS::NoXRF:
		header.append("Fluorescence Detector:\tNone\n");
		break;
	case VESPERS::SingleElement:
		header.append("Fluorescence Detector:\tSingle Element Vortex Detector\n");
		break;
	case VESPERS::FourElement:
		header.append("Fluorescence Detector:\tFour Element Vortex Detector\n");
		break;
	case VESPERS::SingleElement | VESPERS::FourElement:
		header.append("Fluorescence Detector:\tSingle Element Vortex Detector and Four Element Vortex Detector\n");
		break;
	}

	switch(incomingChoice()){

	case VESPERS::Isplit:
		header.append("I0:\tIsplit - The split ion chamber.\n");
		break;
	case VESPERS::Iprekb:
		header.append("I0:\tIprekb - The ion chamber before the KB mirror box.\n");
		break;
	case VESPERS::Imini:
		header.append("I0:\tImini - The small ion chamber immediately after the KB mirror box.\n");
		break;
	case VESPERS::Ipost:
		header.append("I0:\tIpost - The ion chamber at the end of the beamline.\n");
		break;
	}

	header.append("\nRegions of Interest\n");

	for (int i = 0; i < roiInfoList_.count(); i++)
		header.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");

	header.append("\n");

	switch(int(motor())){

	case VESPERS::H:
		header.append("Using horizontal (H) pseudo motor.\n");
		break;

	case VESPERS::X:
		header.append("Using horizontal (X) real motor.\n");
		break;

	case VESPERS::V:
		header.append("Using vertical (V) pseudo motor.\n");
		break;

	case VESPERS::Z:
		header.append("Using vertical (Z) real motor.\n");
		break;
	}

	header.append("\n");
	header.append("Line Dimensions\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(start()).arg(end()));
	header.append(QString("Step Size:\t%1 mm\n").arg(step()));

	if (ccdDetector_ == VESPERS::Roper || ccdDetector_ == VESPERS::Mar)
		header.append(QString("\nFilename for XRD images:\t%1\n").arg(ccdFileName()));

	return header;
}

void VESPERSSpatialLineScanConfiguration::setIncomingChoice(VESPERS::IonChamber I0)
 {
	if (I0_ != I0){

		I0_ = I0;
		emit incomingChoiceChanged(I0_);
		emit incomingChoiceChanged(int(I0_));
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setFluorescenceDetector(VESPERS::FluorescenceDetector detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		emit fluorescenceDetectorChanged(fluorescenceDetector_);
		emit fluorescenceDetectorChanged(int(fluorescenceDetector_));
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setMotor(VESPERS::Motor choice)
{
	if (motor_ != choice) {

		motor_ = choice;
		emit motorChanged(motor_);
		emit motorChanged(int(motor_));
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setCCDDetector(VESPERS::CCDDetector ccd)
{
	if (ccdDetector_ != ccd){

		ccdDetector_ = ccd;
		emit ccdDetectorChanged(ccdDetector_);
		emit ccdDetectorChanged(int(ccdDetector_));
		setModified(true);
	}
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


QString VESPERSSpatialLineScanConfiguration::readRoiList() const
{
	QString prettyRois = "Regions of Interest\n";

	for (int i = 0; i < roiInfoList_.count(); i++)
		prettyRois.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");

	return prettyRois;
}

void VESPERSSpatialLineScanConfiguration::computeTotalTime()
{
	double totalTime = 0;

	// Get the number of points.  Using 0 for the index because this will only have one region per scan.
	totalTime = fabs((end()-start())/step()) + 1;

	// Factor in the time per point.  There is an extra 6 seconds for CCD images.
	if (ccdDetector_ == VESPERS::Roper)
		totalTime *= time() + timeOffset_ + 6.0;
	else if (ccdDetector_ == VESPERS::Mar)
		totalTime *= time() + timeOffset_ + 3.0;
	else
		totalTime *= time() + timeOffset_;

	totalTime_ = totalTime;
	emit totalTimeChanged(totalTime_);
}

void VESPERSSpatialLineScanConfiguration::setExportSpectraSources(bool exportSpectra)
{
	if (exportSpectraSources_ == exportSpectra)
		return;

	exportSpectraSources_ = exportSpectra;
}
