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
#include "acquaman/VESPERS/VESPERSXASScanActionController.h"

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(QObject *parent)
	: AMEXAFSScanConfiguration(parent), VESPERSScanConfiguration()
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
	dbObject_->setParent(this);
	setMotor(VESPERS::H | VESPERS::V);
	setFluorescenceDetector(VESPERS::NoXRF);
	setTransmissionChoice(VESPERS::Ipost);
	setIncomingChoice(VESPERS::Imini);
	edge_ = "";
	energy_ = 0.0;
	useFixedTime_ = false;
	numberOfScans_ = 1;

	goToPosition_ = false;
	position_ = QPointF(0.0, 0.0);
	setExportSpectraSources(true);
	setExportSpectraInRows(true);
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onEXAFSRegionsChanged()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a0Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a1Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a2Changed(double)), this, SLOT(computeTotalTime()));
}

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(const VESPERSEXAFSScanConfiguration &original)
	: AMEXAFSScanConfiguration(original), VESPERSScanConfiguration(original)
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
	dbObject_->setParent(this);
	edge_ = original.edge();
	energy_ = original.energy();
	useFixedTime_ = original.useFixedTime();
	numberOfScans_ = original.numberOfScans();

	goToPosition_ = original.goToPosition();
	position_ = original.position();
	setExportSpectraSources(original.exportSpectraSources());
	setExportSpectraInRows(original.exportSpectraInRows());
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
	AMScanActionController *controller = new VESPERSXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *VESPERSEXAFSScanConfiguration::createView()
{
	return new VESPERSEXAFSScanConfigurationView(this);
}

QString VESPERSEXAFSScanConfiguration::detailedDescription() const
{
	return exafsRegions()->hasKSpace() ? QString("VESPERS EXAFS Scan") : QString("VESPERS XANES Scan");
}

QString VESPERSEXAFSScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append("Scanned Edge:\t" + edge() + "\n");
	header.append(fluorescenceHeaderString(fluorescenceDetector()));
	header.append(incomingChoiceHeaderString(incomingChoice()));
	header.append(transmissionChoiceHeaderString(transmissionChoice()));

	header.append(QString("Automatically moved to a specific location (used when setting up the workflow)?\t%1").arg(goToPosition() ? "Yes\n" : "No\n\n"));

	if (goToPosition()){

		header.append(QString("Horizontal Position:\t%1 mm\n").arg(x()));
		header.append(QString("Vertical Position:\t%1 mm\n\n").arg(y()));
	}

	header.append(regionOfInterestHeaderString(roiList()));
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

void VESPERSEXAFSScanConfiguration::computeTotalTimeImplementation()
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
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
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

void VESPERSEXAFSScanConfiguration::setPosition(const QPointF &pos)
{
	setX(pos.x());
	setY(pos.y());
}

void VESPERSEXAFSScanConfiguration::setX(double xPos)
{
	if (position_.x() != xPos){

		position_.setX(xPos);
		emit xPositionChanged(xPos);
		setModified(true);
	}
}

void VESPERSEXAFSScanConfiguration::setY(double yPos)
{
	if (position_.y() != yPos){

		position_.setY(yPos);
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

void VESPERSEXAFSScanConfiguration::setExportSpectraSources(bool exportSpectra)
{
	if (exportSpectraSources_ == exportSpectra)
		return;

	exportSpectraSources_ = exportSpectra;
}

void VESPERSEXAFSScanConfiguration::setExportSpectraInRows(bool exportInRows)
{
	if (exportSpectraInRows_ == exportInRows)
		return;

	exportSpectraInRows_ = exportInRows;
}
