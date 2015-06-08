#include "BioXASJJSlitsScanConfiguration.h"

BioXASJJSlitsScanConfiguration::BioXASJJSlitsScanConfiguration(QObject *parent) :
    AMStepScanConfiguration(parent)
{
	setName("JJ Slits Calibration");
	setUserScanName("JJ Slits Calibration");
	totalTime_ = 0.0;

	AMScanAxisRegion *region = new AMScanAxisRegion();
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	region = new AMScanAxisRegion();
	axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect( scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()) );

	connect( scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(1)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()) );

	computeTotalTime();
}

BioXASJJSlitsScanConfiguration::BioXASJJSlitsScanConfiguration(const BioXASJJSlitsScanConfiguration &original)
{
	setName(original.name());
	setUserScanName(original.name());
	totalTime_ = 0.0;

	connect( scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()) );

	connect( scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	connect( scanAxisAt(1)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()) );

	computeTotalTime();
}

BioXASJJSlitsScanConfiguration::~BioXASJJSlitsScanConfiguration()
{

}

AMScanConfiguration* BioXASJJSlitsScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASJJSlitsScanConfiguration(*this);
	configuration->dissociateFromDb(true);

	return configuration;
}

AMScanConfigurationView* BioXASJJSlitsScanConfiguration::createView()
{
	return 0;
}

QString BioXASJJSlitsScanConfiguration::detailedDescription() const
{
	return "The JJ slits calibration scan";
}

void BioXASJJSlitsScanConfiguration::setJJSlitGap(const AMControlInfo &info)
{
	if (axisControlInfos_.count() == 0 || axisControlInfos_.count() == 1) {
		axisControlInfos_.insert(0, info);
	} else {
		axisControlInfos_.replace(0, info);
	}
}

void BioXASJJSlitsScanConfiguration::setJJSlitCenter(const AMControlInfo &info)
{
	if (axisControlInfos_.count() == 0 || axisControlInfos_.count() == 1) {
		axisControlInfos_.insert(1, info);
	} else {
		axisControlInfos_.replace(1, info);
	}
}

void BioXASJJSlitsScanConfiguration::setDetector(const AMDetectorInfo &info)
{
	if (detectorConfigurations_.isEmpty())
		detectorConfigurations_.addDetectorInfo(info);
	else
		detectorConfigurations_.replace(0, info, info.name());
}

void BioXASJJSlitsScanConfiguration::computeTotalTime()
{
	int gapPoints = fabs((double(scanAxisAt(0)->regionAt(0)->regionEnd()) - double(scanAxisAt(0)->regionAt(0)->regionStart()) / double(scanAxisAt(0)->regionAt(0)->regionStep()) + 1));
	double gapTime = gapPoints * double(scanAxisAt(0)->regionAt(0)->regionTime());

	int centerPoints = fabs((double(scanAxisAt(1)->regionAt(0)->regionEnd()) - double(scanAxisAt(1)->regionAt(0)->regionStart()) / double(scanAxisAt(1)->regionAt(0)->regionStep()) + 1));
	double centerTime = centerPoints * double(scanAxisAt(1)->regionAt(0)->regionTime());

	totalTime_ = gapTime + centerTime;

	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}
