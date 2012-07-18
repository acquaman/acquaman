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


#include "VESPERS2DScanConfiguration.h"

#include "acquaman/VESPERS/VESPERS2DDacqScanController.h"
#include "ui/VESPERS/VESPERS2DScanConfigurationView.h"

VESPERS2DScanConfiguration::VESPERS2DScanConfiguration(QObject *parent)
	: AM2DScanConfiguration(parent)
{
	setName("2D Map");
	setUserScanName("2D Map");
	I0_ = Imini;
	fluorescenceDetectorChoice_ = SingleElement;
	motorsChoice_ = HAndV;
	usingCCD_ = false;
	ccdFileName_ = "";
	roiInfoList_ = AMROIInfoList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(this, SIGNAL(xStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(usingCCDChanged(bool)), this, SLOT(computeTotalTime()));
}

VESPERS2DScanConfiguration::VESPERS2DScanConfiguration(const VESPERS2DScanConfiguration &original)
	: AM2DScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
	I0_ = original.incomingChoice();
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	motorsChoice_ = original.motorsChoice();
	usingCCD_ = original.usingCCD();
	ccdFileName_ = original.ccdFileName();
	roiInfoList_ = original.roiList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(this, SIGNAL(xStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(usingCCDChanged(bool)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERS2DScanConfiguration::createCopy() const
{
	return new VESPERS2DScanConfiguration(*this);
}

AMScanController *VESPERS2DScanConfiguration::createController()
{
	return new VESPERS2DDacqScanController(this);
}

AMScanConfigurationView *VESPERS2DScanConfiguration::createView()
{
	return new VESPERS2DScanConfigurationView(this);
}

QString VESPERS2DScanConfiguration::detailedDescription() const
{
	if (usingCCD_)
		return "Spatial x-ray fluorescence 2D map using a CCD for x-ray diffraction.";

	return "Spatial x-ray fluorescence 2D map";
}

QString VESPERS2DScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

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

	header.append("\nRegions of Interest\n");

	for (int i = 0; i < roiInfoList_.count(); i++)
		header.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");

	header.append("\n");

	switch(motorsChoice()){

	case HAndV:
		header.append("Using pseudo motors: H and V.\n");
		break;

	case XAndZ:
		header.append("Using real motors: X and Z.\n");
		break;
	}

	header.append("\n");
	header.append("Map Dimensions\n");
	header.append("X Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(xStart()).arg(xEnd()));
	header.append(QString("Step Size:\t%1 mm\n").arg(xStep()));
	header.append("Y Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(yStart()).arg(yEnd()));
	header.append(QString("Step Size:\t%1 mm\n").arg(yStep()));

	if (usingCCD())
		header.append(QString("\nFilename for XRD images:\t%1\n").arg(ccdFileName()));

	return header;
}

void VESPERS2DScanConfiguration::setIncomingChoice(IonChamber I0)
 {
	if (I0_ != I0){

		I0_ = I0;
		emit incomingChoiceChanged(I0_);
		emit incomingChoiceChanged(int(I0_));
		setModified(true);
	}
}

void VESPERS2DScanConfiguration::setFluorescenceDetectorChoice(FluorescenceDetector detector)
{
	if (fluorescenceDetectorChoice_ != detector){

		fluorescenceDetectorChoice_ = detector;
		emit fluorescenceDetectorChoiceChanged(fluorescenceDetectorChoice_);
		emit fluorescenceDetectorChoiceChanged(int(fluorescenceDetectorChoice_));
		setModified(true);
	}
}

void VESPERS2DScanConfiguration::setMotorsChoice(MotorsChoice choice)
{
	if (motorsChoice_ != choice) {

		motorsChoice_ = choice;
		emit motorsChoiceChanged(motorsChoice_);
		emit motorsChoiceChanged(int(motorsChoice_));
		setModified(true);
	}
}

QString VESPERS2DScanConfiguration::readRoiList() const
{
	QString prettyRois = "Regions of Interest\n";

	for (int i = 0; i < roiInfoList_.count(); i++)
		prettyRois.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");

	return prettyRois;
}

QString VESPERS2DScanConfiguration::xAxisName() const
{
	switch(motorsChoice_){

	case HAndV:
		return "Horizontal (H)";

	case XAndZ:
		return "Horizontal (X)";
	}

	return "Horizontal";
}

QString VESPERS2DScanConfiguration::xAxisUnits() const
{
	switch(motorsChoice_){

	case HAndV:
		return "mm";

	case XAndZ:
		return "mm";
	}

	return "mm";
}

QString VESPERS2DScanConfiguration::yAxisName() const
{
	switch(motorsChoice_){

	case HAndV:
		return "Vertical (V)";

	case XAndZ:
		return "Vertical (Z)";
	}

	return "Vertical";
}

QString VESPERS2DScanConfiguration::yAxisUnits() const
{
	switch(motorsChoice_){

	case HAndV:
		return "mm";

	case XAndZ:
		return "mm";
	}

	return "mm";
}

void VESPERS2DScanConfiguration::computeTotalTime()
{
	double time = 0;

	// Get the number of points.
	time = 	fabs((xEnd()-xStart())/xStep()+1)*fabs((yEnd()-yStart())/yStep()+1);

	// Factor in the time per point.  There is an extra 6 seconds for CCD images.
	if (usingCCD())
		time *= timeStep() + timeOffset_ + 6.0;
	else
		time *= timeStep() + timeOffset_;

	totalTime_ = time;
	emit totalTimeChanged(totalTime_);
}
