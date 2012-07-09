#include "VESPERSSpatialLineScanConfiguration.h"

#include "acquaman/VESPERS/VESPERSSpatialLineDacqScanController.h"
#include "ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h"

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(QObject *parent)
	: AMRegionScanConfiguration(parent)
{
	setName("Line Scan");
	setUserScanName("Line Scan");
	regions_->setSensibleRange(0, 10);
	regions_->setDefaultUnits(" mm");
	regions_->setDefaultTimeUnits(" s");
	I0_ = Imini;
	fluorescenceDetectorChoice_ = SingleElement;
	motorChoice_ = H;
	usingCCD_ = false;
	ccdFileName_ = "";
	roiInfoList_ = AMROIInfoList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(this, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeStepChanged(double)), this, SLOT(computeTotalTime()));
}

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(const VESPERSSpatialLineScanConfiguration &original)
	: AMRegionScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
	regions_->setSensibleRange(original.regions()->sensibleStart(), original.regions()->sensibleEnd());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());
	I0_ = original.incomingChoice();
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	motorChoice_ = original.motorChoice();
	usingCCD_ = original.usingCCD();
	ccdFileName_ = original.ccdFileName();
	roiInfoList_ = original.roiList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(this, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeStepChanged(double)), this, SLOT(computeTotalTime()));
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
	if (usingCCD_)
		return "Spatial x-ray fluorescence line scan using a CCD for x-ray diffraction.";

	return "Spatial x-ray fluorescence line scan";
}

QString VESPERSSpatialLineScanConfiguration::headerText() const
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

	switch(motorChoice()){

	case H:
		header.append("Using horizontal (H) pseudo motor.\n");
		break;

	case X:
		header.append("Using horizontal (X) real motor.\n");
		break;

	case V:
		header.append("Using vertical (V) pseudo motor.\n");
		break;

	case Z:
		header.append("Using vertical (Z) real motor.\n");
		break;
	}

	header.append("\n");
	header.append("Line Dimensions\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(regions()->start(0)).arg(regions()->end(0)));
	header.append(QString("Step Size:\t%1 mm\n").arg(regions()->delta(0)));

	if (usingCCD())
		header.append(QString("\nFilename for XRD images:\t%1\n").arg(ccdFileName()));

	return header;
}

void VESPERSSpatialLineScanConfiguration::setIncomingChoice(IonChamber I0)
 {
	if (I0_ != I0){

		I0_ = I0;
		emit incomingChoiceChanged(I0_);
		emit incomingChoiceChanged(int(I0_));
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setFluorescenceDetectorChoice(FluorescenceDetector detector)
{
	if (fluorescenceDetectorChoice_ != detector){

		fluorescenceDetectorChoice_ = detector;
		emit fluorescenceDetectorChoiceChanged(fluorescenceDetectorChoice_);
		emit fluorescenceDetectorChoiceChanged(int(fluorescenceDetectorChoice_));
		setModified(true);
	}
}

void VESPERSSpatialLineScanConfiguration::setMotorChoice(MotorChoice choice)
{
	if (motorChoice_ != choice) {

		motorChoice_ = choice;
		emit motorChoiceChanged(motorChoice_);
		emit motorChoiceChanged(int(motorChoice_));
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
	double time = 0;

	// Get the number of points.  Using 0 for the index because this will only have one region per scan.
	time = 	fabs((regions_->end(0)-regions_->start(0))/regions_->delta(0)) + 1;

	// Factor in the time per point.  There is an extra 6 seconds for CCD images.
	if (usingCCD())
		time *= regions()->time(0) + timeOffset_ + 6.0;
	else
		time *= regions()->time(0) + timeOffset_;

	totalTime_ = time;
	emit totalTimeChanged(totalTime_);
}
