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
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "acquaman/VESPERS/VESPERSXASScanActionController.h"
#include "dataman/AMScanAxisEXAFSRegion.h"

VESPERSEXAFSScanConfiguration::~VESPERSEXAFSScanConfiguration(){}

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent), VESPERSScanConfiguration()
{
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

	AMScanAxisRegion *region = new AMScanAxisEXAFSRegion(8800.0, 5.0, 8970.0, 1.0);
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	region = new AMScanAxisEXAFSRegion(8970, 0.5, 9040, 1);
	axis->appendRegion(region);
	region = new AMScanAxisEXAFSRegion(3.24, 0.01, 10, 1, true, 9000, 10);
	axis->appendRegion(region);
	appendScanAxis(axis);

	goToPosition_ = false;
	position_ = QPointF(0.0, 0.0);
	setExportSpectraSources(true);
	setExportSpectraInRows(true);
}

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(const VESPERSEXAFSScanConfiguration &original)
	: AMStepScanConfiguration(original), VESPERSScanConfiguration(original)
{
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
	return QString("VESPERS XAS Scan");
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

	if (normalPosition() != 888888.88){

		header.append("\n");
		header.append(QString("Focus position:\t%1 mm\n").arg(normalPosition()));
	}

	header.append(regionOfInterestHeaderString(roiList()));
	header.append("\n");
	header.append("Regions Scanned\n");

//	for (int i = 0; i < regionCount(); i++){

//		if (exafsRegions()->type(i) == AMEXAFSRegion::kSpace && useFixedTime())
//			header.append(QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tTime: %4 s\n")
//						  .arg(exafsRegions()->startByType(i, AMEXAFSRegion::Energy))
//						  .arg(exafsRegions()->delta(i))
//						  .arg(exafsRegions()->endByType(i, AMEXAFSRegion::kSpace))
//						  .arg(regions_->time(i)));

//		else if (exafsRegions()->type(i) == AMEXAFSRegion::kSpace && !useFixedTime())
//			header.append(QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tMaximum time (used with variable integration time): %4 s\n")
//						  .arg(exafsRegions()->startByType(i, AMEXAFSRegion::Energy))
//						  .arg(exafsRegions()->delta(i))
//						  .arg(exafsRegions()->endByType(i, AMEXAFSRegion::kSpace))
//						  .arg(exafsRegions()->time(i)));

//		else
//			header.append(QString("Start: %1 eV\tDelta: %2 eV\tEnd: %3 eV\tTime: %4 s\n")
//						  .arg(regionStart(i))
//						  .arg(regionDelta(i))
//						  .arg(regionEnd(i))
//						  .arg(regionTime(i)));
//	}

	return header;
}

void VESPERSEXAFSScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;

	// Some region stuff.

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

//		exafsRegions()->setDefaultEdgeEnergy(edgeEnergy);
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
