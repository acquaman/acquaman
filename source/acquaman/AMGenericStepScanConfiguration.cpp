#include "AMGenericStepScanConfiguration.h"

#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "acquaman/AMGenericStepScanController.h"

AMGenericStepScanConfiguration::AMGenericStepScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent)
{
	setName("Generic Scan");
	setUserScanName("Generic Scan");

	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

AMGenericStepScanConfiguration::AMGenericStepScanConfiguration(const AMGenericStepScanConfiguration &original)
	: AMStepScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());

	computeTotalTime();
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

AMGenericStepScanConfiguration::~AMGenericStepScanConfiguration(){}

AMScanConfiguration *AMGenericStepScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new AMGenericStepScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *AMGenericStepScanConfiguration::createController()
{
	AMScanActionController *controller = new AMGenericStepScanController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *AMGenericStepScanConfiguration::createView()
{
	return new AMGenericStepScanConfigurationView(this);
}

QString AMGenericStepScanConfiguration::technique() const
{
	return "Generic Scan";
}

QString AMGenericStepScanConfiguration::description() const
{
	return "Generic Scan";
}

QString AMGenericStepScanConfiguration::detailedDescription() const
{
	return "Does a generic scan over one or two controls with a variety of detectors.";
}

void AMGenericStepScanConfiguration::computeTotalTime()
{
//	double time = 0;

//	// Get the number of points.
//	time = 	fabs((double(scanAxisAt(0)->regionAt(0)->regionEnd())-double(scanAxisAt(0)->regionAt(0)->regionStart()))/double(scanAxisAt(0)->regionAt(0)->regionStep())+1)
//			*fabs((double(scanAxisAt(1)->regionAt(0)->regionEnd())-double(scanAxisAt(1)->regionAt(0)->regionStart()))/double(scanAxisAt(1)->regionAt(0)->regionStep())+1);

//	totalTime_ = time;
//	setExpectedDuration(totalTime_);
//	emit totalTimeChanged(totalTime_);
}
