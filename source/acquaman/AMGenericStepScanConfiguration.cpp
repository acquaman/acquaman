#include "AMGenericStepScanConfiguration.h"

#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "acquaman/AMGenericStepScanController.h"
#include "beamline/AMBeamline.h"

#include <math.h>

AMGenericStepScanConfiguration::AMGenericStepScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent)
{
	setName("Generic Scan");
	setUserScanName("Generic Scan");
}

AMGenericStepScanConfiguration::AMGenericStepScanConfiguration(const AMGenericStepScanConfiguration &original)
	: AMStepScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());

	computeTotalTime();

	for (int i = 0, size = scanAxes_.count(); i < size; i++){

		connect(scanAxisAt(i)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(i)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(i)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(i)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	}
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
	return new AMGenericStepScanConfigurationView(this, AMBeamline::bl()->exposedControls(), AMBeamline::bl()->exposedDetectors());
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
	totalTime_ = 0;

	foreach (AMScanAxis *axis, scanAxes_.toList())
		totalTime_ += calculateRegionsTotalTime(axis);

	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

double AMGenericStepScanConfiguration::calculateRegionTotalTime(AMScanAxisRegion *region)
{
	double result = 0;

	if (region) {

		if (region->regionStart().isValid() && region->regionStep().isValid() && region->regionEnd().isValid() && region->regionTime().isValid()) {
			double size = fabs( double(region->regionStart()) - double(region->regionEnd()) );
			double points = int(size / double(region->regionStep()));

			if ((size - (points + 0.01) * double(region->regionStep())) < 0)
				points += 1;
			else
				points += 2;

			result = points * double(region->regionTime());
		}
	}

	return result;
}

double AMGenericStepScanConfiguration::calculateRegionsTotalTime(AMScanAxis *scanAxis)
{
	double result = 0;

	if (scanAxis)
		foreach (AMScanAxisRegion *region, scanAxis->regions().toList())
			result += calculateRegionTotalTime(region);

	return result;
}

void AMGenericStepScanConfiguration::setControl(int axisId, AMControlInfo newInfo)
{
	if (axisId == 0 && axisControlInfos_.isEmpty()){

		axisControlInfos_.append(newInfo);
		setModified(true);

		AMScanAxisRegion *region = new AMScanAxisRegion;
		AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
		appendScanAxis(axis);

		connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	}

	else if (axisId == 0){

		axisControlInfos_.replace(0, newInfo);
		setModified(true);
	}

	else if (axisId == 1 && axisControlInfos_.count() == 1){

		axisControlInfos_.append(newInfo);
		setModified(true);

		AMScanAxisRegion *region = new AMScanAxisRegion;
		AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
		appendScanAxis(axis);

		connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	}

	else if (axisId == 1){

		axisControlInfos_.replace(1, newInfo);
		setModified(true);
	}

	computeTotalTime();
}

void AMGenericStepScanConfiguration::removeControl(int axisId)
{
	if (axisId < axisControlInfos_.count() && axisId >= 0){

		AMScanAxis *axis = removeScanAxis(axisId);
		axis->regionAt(0)->disconnect();
		axis->regionAt(0)->deleteLater();
		axis->deleteLater();
		axisControlInfos_.remove(axisId);
		setModified(true);
	}
}

void AMGenericStepScanConfiguration::addDetector(AMDetectorInfo newInfo)
{
	bool containsDetector = false;

	for (int i = 0, size = detectorConfigurations_.count(); i < size; i++){

		if (newInfo.name() == detectorConfigurations_.at(i).name())
			containsDetector = true;
	}

	if (!containsDetector){

		detectorConfigurations_.append(newInfo, newInfo.name());
		setModified(true);
	}
}

void AMGenericStepScanConfiguration::removeDetector(AMDetectorInfo info)
{
	bool detectorRemoved = false;

	for (int i = 0, size = detectorConfigurations_.count(); i < size && !detectorRemoved; i++){

		if (info.name() == detectorConfigurations_.at(i).name()){

			detectorRemoved = true;
			detectorConfigurations_.remove(i);
			setModified(true);
		}
	}
}
