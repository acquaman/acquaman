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
