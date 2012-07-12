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
	regions_->setUnits(0, " mm");
	regions_->setTimeUnits(0, " s");
	I0_ = Imini;
	fluorescenceDetectorChoice_ = SingleElement;
	motorChoice_ = H;
	usingCCD_ = false;
	ccdFileName_ = "";
	roiInfoList_ = AMROIInfoList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(this, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(startChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(stepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(endChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(usingCCDChanged(bool)), this, SLOT(computeTotalTime()));
}

VESPERSSpatialLineScanConfiguration::VESPERSSpatialLineScanConfiguration(const VESPERSSpatialLineScanConfiguration &original)
	: AMRegionScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

	switch(original.motorChoice()){

	case VESPERSSpatialLineScanConfiguration::H:
		regions_->setDefaultControl(VESPERSBeamline::vespers()->pseudoSampleStage()->horiz());
		break;

	case VESPERSSpatialLineScanConfiguration::X:
		regions_->setDefaultControl(VESPERSBeamline::vespers()->realSampleStage()->horiz());
		break;

	case VESPERSSpatialLineScanConfiguration::V:
		regions_->setDefaultControl(VESPERSBeamline::vespers()->pseudoSampleStage()->vert());
		break;

	case VESPERSSpatialLineScanConfiguration::Z:
		regions_->setDefaultControl(VESPERSBeamline::vespers()->realSampleStage()->vert());
		break;
	}

	regions_->setDefaultTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	regions_->addRegion(0, original.start(), original.step(), original.end(), original.time());
	regions_->setSensibleRange(original.regions()->sensibleStart(), original.regions()->sensibleEnd());
	regions_->setUnits(0, original.regions()->units(0));
	regions_->setTimeUnits(0, original.regions()->timeUnits(0));
	I0_ = original.incomingChoice();
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	motorChoice_ = original.motorChoice();
	usingCCD_ = original.usingCCD();
	ccdFileName_ = original.ccdFileName();
	roiInfoList_ = original.roiList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(this, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(startChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(stepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(endChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(usingCCDChanged(bool)), this, SLOT(computeTotalTime()));
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
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(start()).arg(end()));
	header.append(QString("Step Size:\t%1 mm\n").arg(step()));

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
	if (usingCCD())
		totalTime *= time() + timeOffset_ + 6.0;
	else
		totalTime *= time() + timeOffset_;

	totalTime_ = totalTime;
	emit totalTimeChanged(totalTime_);
}
