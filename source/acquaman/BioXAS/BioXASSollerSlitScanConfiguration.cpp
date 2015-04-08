#include "BioXASSollerSlitScanConfiguration.h"

#include "acquaman/BioXAS/BioXASSollerSlitScanController.h"
#include "ui/BioXAS/BioXASSollerSlitScanConfigurationView.h"

#include <math.h>

BioXASSollerSlitScanConfiguration::BioXASSollerSlitScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent)
{
	setName("Soller Slit Calibration");
	setUserScanName("Soller Slit Calibration");
	totalTime_ = 0.0;

	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
	region = new AMScanAxisRegion;
	axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

BioXASSollerSlitScanConfiguration::BioXASSollerSlitScanConfiguration(const BioXASSollerSlitScanConfiguration &original)
	: AMStepScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());
	totalTime_ = 0.0;

	computeTotalTime();
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

BioXASSollerSlitScanConfiguration::~BioXASSollerSlitScanConfiguration()
{

}

AMScanConfiguration *BioXASSollerSlitScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASSollerSlitScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *BioXASSollerSlitScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASSollerSlitScanController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *BioXASSollerSlitScanConfiguration::createView()
{
	return new BioXASSollerSlitScanConfigurationView(this);
}

QString BioXASSollerSlitScanConfiguration::detailedDescription() const
{
	return "The Soller slit calibration scan.";
}

void BioXASSollerSlitScanConfiguration::setSollerSlitX(const AMControlInfo &info)
{
	if (axisControlInfos_.count() == 0 || axisControlInfos_.count() == 1)
		axisControlInfos_.insert(0, info);

	else
		axisControlInfos_.replace(0, info);
}

void BioXASSollerSlitScanConfiguration::setSollerSlitZ(const AMControlInfo &info)
{
	if (axisControlInfos_.count() == 0 || axisControlInfos_.count() == 1)
		axisControlInfos_.insert(1, info);

	else
		axisControlInfos_.replace(1, info);
}

void BioXASSollerSlitScanConfiguration::setDetector(const AMDetectorInfo &info)
{
	if (detectorConfigurations_.isEmpty())
		detectorConfigurations_.addDetectorInfo(info);

	else
		detectorConfigurations_.replace(0, info, info.name());
}

void BioXASSollerSlitScanConfiguration::computeTotalTime()
{
	double time = 0;

	// Get the number of points.
	time = 	fabs((double(scanAxisAt(0)->regionAt(0)->regionEnd())-double(scanAxisAt(0)->regionAt(0)->regionStart()))/double(scanAxisAt(0)->regionAt(0)->regionStep())+1)
			*fabs((double(scanAxisAt(1)->regionAt(0)->regionEnd())-double(scanAxisAt(1)->regionAt(0)->regionStart()))/double(scanAxisAt(1)->regionAt(0)->regionStep())+1);

	time *= double(scanAxisAt(0)->regionAt(0)->regionTime());

	totalTime_ = time;
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}
