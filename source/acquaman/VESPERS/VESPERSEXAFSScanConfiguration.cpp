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


#include "VESPERSEXAFSScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSEXAFSDacqScanController.h"
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(QObject *parent)
	: AMEXAFSScanConfiguration(parent)
{
	regions_->setSensibleRange(-30, 40);
	exafsRegions()->setDefaultEdgeEnergy(0); // Everything for XAS related scans on VESPERS is done using relative energy to the edge on a PV level.
	exafsRegions()->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	exafsRegions()->setTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	exafsRegions()->setKControl(VESPERSBeamline::vespers()->kControl());
	exafsRegions()->setDefaultIsRelative(true);
	regions_->setDefaultUnits(" eV");
	regions_->setDefaultTimeUnits(" s");
	setName("XAS Scan");
	setUserScanName("XAS Scan");
	fluorescenceDetectorChoice_ = None;
	It_ = Ipost;
	I0_ = Imini;
	edge_ = "";
	energy_ = 0.0;
	useFixedTime_ = false;
	numberOfScans_ = 1;

	roiInfoList_ = AMROIInfoList();

	goToPosition_ = false;
	position_ = qMakePair(0.0, 0.0);
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onEXAFSRegionsChanged()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a0Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a1Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a2Changed(double)), this, SLOT(computeTotalTime()));
}

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(const VESPERSEXAFSScanConfiguration &original)
	: AMEXAFSScanConfiguration(original)
{
	regions_->setSensibleStart(original.regions()->sensibleStart());
	regions_->setSensibleEnd(original.regions()->sensibleEnd());
	exafsRegions()->setDefaultEdgeEnergy(original.exafsRegions()->defaultEdgeEnergy());
	exafsRegions()->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	exafsRegions()->setTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	exafsRegions()->setKControl(VESPERSBeamline::vespers()->kControl());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());
	exafsRegions()->setDefaultIsRelative(original.exafsRegions()->defaultIsRelative());

	for (int i = 0; i < original.regionCount(); i++){

		// Because I store the values in energy space, I need to ask for them explicitly with no converstion.  Otherwise, the k-space values will be converted twice.
		regions_->addRegion(i, original.regionStartByType(i, AMEXAFSRegion::Energy), original.regionDelta(i), original.regionEndByType(i, AMEXAFSRegion::Energy), original.regionTime(i));
		exafsRegions()->setType(i, original.regionType(i));
		exafsRegions()->setEdgeEnergy(i, original.regionEdgeEnergy(i));
	}

	setName(original.name());
	setUserScanName(original.userScanName());
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	It_ = original.transmissionChoice();
	I0_ = original.incomingChoice();
	edge_ = original.edge();
	energy_ = original.energy();
	useFixedTime_ = original.useFixedTime();
	numberOfScans_ = original.numberOfScans();

	goToPosition_ = original.goToPosition();
	position_ = original.position();

	roiInfoList_ = original.roiList();

	timeOffset_ = 0.7;
	totalTime_ = 0;
	computeTotalTime();
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onEXAFSRegionsChanged()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a0Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a1Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a2Changed(double)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERSEXAFSScanConfiguration::createCopy() const
{
	return new VESPERSEXAFSScanConfiguration(*this);
}

AMScanController *VESPERSEXAFSScanConfiguration::createController()
{
	return new VESPERSEXAFSDacqScanController(this);
}

AMScanConfigurationView *VESPERSEXAFSScanConfiguration::createView()
{
	return new VESPERSEXAFSScanConfigurationView(this);
}

QString VESPERSEXAFSScanConfiguration::detailedDescription() const
{
	return exafsRegions()->hasKSpace() ? QString("VESPERS EXAFS Scan") : QString("VESPERS XANES Scan");
}

QString VESPERSEXAFSScanConfiguration::readRoiList() const
{
	QString prettyRois = "Regions of Interest\n";

	for (int i = 0; i < roiInfoList_.count(); i++)
		prettyRois.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");

	return prettyRois;
}

QString VESPERSEXAFSScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append("Scanned Edge:\t" + edge() + "\n");

	switch(fluorescenceDetectorChoice()){

	case None:
		header.append("Fluorescence Detector:\tNone\n");
		break;
	case SingleElement:
		header.append("Fluorescence Detector:\tSingle Element Vortex Detector\n");
		break;
	case FourElement:
		header.append("Fluorescence Detector:\tFour Element Vortex Detector\n");
		break;
	}

	switch(incomingChoice()){

	case Isplit:
		header.append("I0:\tIsplit - The split ion chamber.\n");
		break;
	case Iprekb:
		header.append("I0:\tIprekb - The ion chamber before the KB mirror box.\n");
		break;
	case Imini:
		header.append("I0:\tImini - The small ion chamber immediately after the KB mirror box.\n");
		break;
	case Ipost:
		header.append("I0:\tIpost - The ion chamber at the end of the beamline.\n");
		break;
	}

	switch(transmissionChoice()){

	case Isplit:
		header.append("It:\tIsplit - The split ion chamber.\n");
		break;
	case Iprekb:
		header.append("It:\tIprekb - The ion chamber before the KB mirror box.\n");
		break;
	case Imini:
		header.append("It:\tImini - The small ion chamber immediately after the KB mirror box.\n");
		break;
	case Ipost:
		header.append("It:\tIpost - The ion chamber at the end of the beamline.\n");
		break;
	}

	header.append(QString("Automatically moved to a specific location (used when setting up the workflow)?\t%1").arg(goToPosition() ? "Yes\n" : "No\n\n"));

	if (goToPosition()){

		header.append(QString("Horizontal Position:\t%1 mm\n").arg(x()));
		header.append(QString("Vertical Position:\t%1 mm\n\n").arg(y()));
	}

	if (fluorescenceDetectorChoice() != None){

		header.append("Regions of Interest\n");

		for (int i = 0; i < roiInfoList_.count(); i++)
			header.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");
	}

	header.append("\n");
	header.append("Regions Scanned\n");

	for (int i = 0; i < regionCount(); i++){

		if (exafsRegions()->type(i) == AMEXAFSRegion::kSpace && useFixedTime())
			header.append(QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tTime: %4 s\n")
						  .arg(exafsRegions()->startByType(i, AMEXAFSRegion::Energy))
						  .arg(exafsRegions()->delta(i))
						  .arg(exafsRegions()->endByType(i, AMEXAFSRegion::kSpace))
						  .arg(regions_->time(i)));

		else if (exafsRegions()->type(i) == AMEXAFSRegion::kSpace && !useFixedTime())
			header.append(QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tMaximum time (used with variable integration time): %4 s\n")
						  .arg(exafsRegions()->startByType(i, AMEXAFSRegion::Energy))
						  .arg(exafsRegions()->delta(i))
						  .arg(exafsRegions()->endByType(i, AMEXAFSRegion::kSpace))
						  .arg(exafsRegions()->time(i)));

		else
			header.append(QString("Start: %1 eV\tDelta: %2 eV\tEnd: %3 eV\tTime: %4 s\n")
						  .arg(regionStart(i))
						  .arg(regionDelta(i))
						  .arg(regionEnd(i))
						  .arg(regionTime(i)));
	}

	return header;
}

void VESPERSEXAFSScanConfiguration::onEXAFSRegionsChanged()
{
	if (exafsRegions()->hasKSpace()){

		CLSVariableIntegrationTime *timeApp = VESPERSBeamline::vespers()->variableIntegrationTime();
		bool needToCompute = false;
		int lastVal = exafsRegions()->count()-1;

		if (exafsRegions()->count() > 1 && (exafsRegions()->time(lastVal-1) != timeApp->defautTime())){

			timeApp->setDefaultTime(exafsRegions()->time(lastVal-1));
			needToCompute = true;
		}

		if (exafsRegions()->startByType(lastVal, AMEXAFSRegion::kSpace) != timeApp->lowValue()){

			timeApp->setLowValue(exafsRegions()->startByType(lastVal, AMEXAFSRegion::kSpace));
			needToCompute = true;
		}

		if (exafsRegions()->endByType(lastVal, AMEXAFSRegion::kSpace) != timeApp->highValue()){

			timeApp->setHighValue(exafsRegions()->endByType(lastVal, AMEXAFSRegion::kSpace));
			needToCompute = true;
		}

		if (exafsRegions()->time(lastVal) != timeApp->maximumTime()){

			timeApp->setMaximumTime(exafsRegions()->time(lastVal));
			needToCompute = true;
		}

		if (needToCompute)
			timeApp->compute();
	}
}

void VESPERSEXAFSScanConfiguration::computeTotalTime()
{
	double time = 0;

	if (exafsRegions()->hasKSpace() && !useFixedTime_){

		for (int i = 0; i < regions_->count(); i++){

			if (exafsRegions()->type(i) == AMEXAFSRegion::kSpace)
				time += VESPERSBeamline::vespers()->variableIntegrationTime()->totalTime(regions_->delta(i)) + ((regions_->end(i) - regions_->start(i))/regions_->delta(i))*timeOffset_;

			else
				time += ((regions_->end(i) - regions_->start(i))/regions_->delta(i))*(regions_->time(i) + timeOffset_); // Seems to take about 0.7 seconds for extra beamline stuff to happen.
		}
	}

	else{

		for (int i = 0; i < regions_->count(); i++)
			time += ((regions_->end(i) - regions_->start(i))/regions_->delta(i))*(regions_->time(i) + timeOffset_); // Seems to take about 0.7 seconds for extra beamline stuff to happen.
	}

	totalTime_ = time + 9; // There is a 9 second miscellaneous startup delay.
	emit totalTimeChanged(totalTime_);
}

void VESPERSEXAFSScanConfiguration::setFluorescenceDetectorChoice(FluorescenceDetector detector)
{
	if (fluorescenceDetectorChoice_ != detector){

		fluorescenceDetectorChoice_ = detector;
		emit fluorescenceDetectorChoiceChanged(fluorescenceDetectorChoice_);
		emit fluorescenceDetectorChoiceChanged(int(fluorescenceDetectorChoice_));
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setTransmissionChoice(IonChamber It)
{
	if (It_ != It){

		It_ = It;
		emit transmissionChoiceChanged(It_);
		emit transmissionChoiceChanged(int(It_));
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setIncomingChoice(IonChamber I0)
{
	if (I0_ != I0){

		I0_ = I0;
		emit incomingChoiceChanged(I0_);
		emit incomingChoiceChanged(int(I0_));
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setEdge(QString edgeName)
{
	if (edge_ != edgeName){

		edge_ = edgeName;
		emit edgeChanged(edgeName);
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setEnergy(double edgeEnergy)
{
	if (energy_ != edgeEnergy){

		exafsRegions()->setDefaultEdgeEnergy(edgeEnergy);
		energy_ = edgeEnergy;
		emit energyChanged(energy_);
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setGoToPosition(bool state)
{
	if (goToPosition_ != state){

		goToPosition_ = state;
		emit gotoPositionChanged(goToPosition_);
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setPosition(QPair<double, double> pos)
{
	setX(pos.first);
	setY(pos.second);
}

void VESPERSEXAFSScanConfiguration::setX(double xPos)
{
	if (position_.first != xPos){

		position_.first = xPos;
		emit xPositionChanged(xPos);
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setY(double yPos)
{
	if (position_.second != yPos){

		position_.second = yPos;
		emit yPositionChanged(yPos);
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setUseFixedTime(bool fixed)
{
	if (useFixedTime_ != fixed){

		useFixedTime_ = fixed;
		emit useFixedTimeChanged(useFixedTime_);
		computeTotalTime();
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setNumberOfScans(int num)
{
	if (numberOfScans_ != num){

		numberOfScans_ = num;
		emit numberOfScansChanged(numberOfScans_);
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setTimeOffset(double offset)
{
	if (timeOffset_ != offset){

		timeOffset_ = offset;
		computeTotalTime();
	}
}

void VESPERSEXAFSScanConfiguration::setRoiInfoList(const AMROIInfoList &list)
{
	roiInfoList_ = list;
	setModified(true);
}

