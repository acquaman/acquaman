#include "VESPERSTimedLineScanConfiguration.h"

#include "acquaman/VESPERS/VESPERSTimedLineScanActionController.h"
#include "ui/VESPERS/VESPERSTimedLineScanConfigurationView.h"

VESPERSTimedLineScanConfiguration::VESPERSTimedLineScanConfiguration(QObject *parent)
	: VESPERSSpatialLineScanConfiguration(parent)
{
	timePerAcquisition_ = 0.0;
	iterations_ = 0;

	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(this, SIGNAL(timePerAcquisitionChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(iterationsChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERSTimedLineScanConfiguration::VESPERSTimedLineScanConfiguration(const VESPERSTimedLineScanConfiguration &original)
	: VESPERSSpatialLineScanConfiguration(original)
{
	timePerAcquisition_ = original.timePerAcquisition();
	iterations_ = original.iterations();

	connect(this, SIGNAL(timePerAcquisitionChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(iterationsChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERSTimedLineScanConfiguration::~VESPERSTimedLineScanConfiguration()
{

}

AMScanConfiguration * VESPERSTimedLineScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new VESPERSTimedLineScanConfiguration(*this);
	configuration->dissociateFromDb(true);

	return configuration;
}

AMScanController * VESPERSTimedLineScanConfiguration::createController()
{
	AMScanActionController *controller = new VESPERSTimedLineScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView * VESPERSTimedLineScanConfiguration::createView()
{
	return new VESPERSTimedLineScanConfigurationView(this);
}

QString VESPERSTimedLineScanConfiguration::technique() const
{
	return "Timed Line Scan";
}

QString VESPERSTimedLineScanConfiguration::description() const
{
	return "Timed Line Scan";
}

QString VESPERSTimedLineScanConfiguration::detailedDescription() const
{
	if (ccdDetector() != VESPERS::NoCCD)
		return "Timed Spatial x-ray fluorescence line scan using a CCD for x-ray diffraction.";

	return "Timed Spatial x-ray fluorescence line scan";
}

double VESPERSTimedLineScanConfiguration::timePerAcquisition() const
{
	return timePerAcquisition_;
}

int VESPERSTimedLineScanConfiguration::iterations() const
{
	return iterations_;
}

void VESPERSTimedLineScanConfiguration::setTimePerAcquisition(double newTotalTime)
{
	if (timePerAcquisition_ != newTotalTime){

		timePerAcquisition_ = newTotalTime;
		scanAxes_.at(1)->regionAt(0)->setRegionStart(0);
		scanAxes_.at(1)->regionAt(0)->setRegionStep(timePerAcquisition_);
		scanAxes_.at(1)->regionAt(0)->setRegionEnd(timePerAcquisition_*iterations_);
		emit timePerAcquisitionChanged(timePerAcquisition_);
		setModified(true);
	}
}

void VESPERSTimedLineScanConfiguration::setIterations(int newIteration)
{
	if (iterations_ != newIteration){

		iterations_ = newIteration;
		scanAxes_.at(1)->regionAt(0)->setRegionStart(0);
		scanAxes_.at(1)->regionAt(0)->setRegionStep(timePerAcquisition_);
		scanAxes_.at(1)->regionAt(0)->setRegionEnd(timePerAcquisition_*iterations_);
		emit iterationsChanged(iterations_);
		setModified(true);
	}
}

void VESPERSTimedLineScanConfiguration::computeTotalTimeImplementation()
{
	totalTime_ = timePerAcquisition_*iterations_;
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

QString VESPERSTimedLineScanConfiguration::headerText() const
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
	header.append(QString("Time per point: %1 s\n").arg(time()));

	header.append("\n");
	header.append("Time Dimensions\n");
	header.append(QString("Time Per Acquisition:\t%1 s and # of iterations: %2\n").arg(start()).arg(end()));

	header.append("\nEven though the time in the time column appears to be the same, the actual time would be the listed time plus the point in the line times the time.\n");
	header.append("\nExample: 5 point line scan with 1 second time per point while waiting for 20 seconds between line scans should read 0, 1, 2, 3, 4, 20, 21, 22, 23, 24, etc.\n");

	header.append("\n");
	header.append(QString("Focus position:\t%1 mm\n").arg(normalPosition()));

	if (hasOtherPosition())
		header.append(QString("%1 position: %2 mm\n").arg(otherMotorString(motor())).arg(otherPosition()));

	return header;
}
