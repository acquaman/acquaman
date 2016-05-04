#include "AMGenericContinuousScanConfiguration.h"

#include "ui/acquaman/AMGenericContinuousScanConfigurationView.h"
#include "acquaman/AMGenericContinuousScanController.h"
#include "beamline/AMBeamline.h"

#include <math.h>

AMGenericContinuousScanConfiguration::AMGenericContinuousScanConfiguration(QObject *parent)
	: AMContinuousScanConfiguration(parent)
{
	setName("Generic Continuous Scan");
	setUserScanName("Generic Continuous Scan");
}

AMGenericContinuousScanConfiguration::AMGenericContinuousScanConfiguration(const AMGenericContinuousScanConfiguration &original)
	: AMContinuousScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());
	i0_ = original.i0();

	foreach (AMRegionOfInterest *region, original.regionsOfInterest())
		addRegionOfInterest(region->createCopy());

	computeTotalTime();

	for (int i = 0, size = scanAxes_.count(); i < size; i++){

		connect(scanAxisAt(i)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(i)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(i)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(scanAxisAt(i)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	}
}

AMGenericContinuousScanConfiguration::~AMGenericContinuousScanConfiguration()
{

}

AMScanConfiguration *AMGenericContinuousScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new AMGenericContinuousScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *AMGenericContinuousScanConfiguration::createController()
{
	AMScanActionController *controller = new AMGenericContinuousScanController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *AMGenericContinuousScanConfiguration::createView()
{
	return new AMGenericContinuousScanConfigurationView(this, AMBeamline::bl()->exposedControls(), AMBeamline::bl()->exposedDetectors());
}

QString AMGenericContinuousScanConfiguration::technique() const
{
	return "Generic Continuous";
}

QString AMGenericContinuousScanConfiguration::description() const
{
	return "Generic Continuous Scan";
}

QString AMGenericContinuousScanConfiguration::detailedDescription() const
{
	return "Does a generic continuous scan over a control with a variety of detectors.";
}

bool AMGenericContinuousScanConfiguration::hasI0() const
{
	return i0_.isValid();
}

void AMGenericContinuousScanConfiguration::addRegion(int scanAxisIndex, int regionIndex, AMScanAxisRegion *region)
{
	AMScanAxis *scanAxis = scanAxisAt(scanAxisIndex);

	if (scanAxis && region) {
		scanAxis->insertRegion(regionIndex, region);
		connectRegion(region);
	}

	computeTotalTime();
}

void AMGenericContinuousScanConfiguration::connectRegion(AMScanAxisRegion *region)
{
	if (region) {
		connect( region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()) );
		connect( region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()) );
		connect( region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()) );
		connect( region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	}
}

void AMGenericContinuousScanConfiguration::removeRegion(int scanAxisIndex, AMScanAxisRegion *region)
{
	AMScanAxis *scanAxis = scanAxisAt(scanAxisIndex);

	if (scanAxis && region) {
		scanAxis->removeRegion(region);
		disconnectRegion(region);
	}

	computeTotalTime();
}

void AMGenericContinuousScanConfiguration::disconnectRegion(AMScanAxisRegion *region)
{
	if (region) {
		disconnect( region, 0, this, 0 );
	}
}

void AMGenericContinuousScanConfiguration::computeTotalTime()
{
	totalTime_ = 0;

	computeTotalTimeImplementation();

	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

double AMGenericContinuousScanConfiguration::calculateRegionTotalTime(AMScanAxisRegion *region)
{
	double result = 0;

	if (region) {

		if (region->regionStart().isValid() && region->regionStep().isValid() && region->regionEnd().isValid() && region->regionTime().isValid()) {
			result = region->timePerRegion();
		}
	}

	return result;
}

double AMGenericContinuousScanConfiguration::calculateRegionsTotalTime(AMScanAxis *scanAxis)
{
	double result = 0;

	if (scanAxis)
		result = scanAxis->timePerAxis();

	return result;
}

void AMGenericContinuousScanConfiguration::computeTotalTimeImplementation()
{
	foreach (AMScanAxis *axis, scanAxes_.toList()){

		if (axis->axisType() == AMScanAxis::ContinuousMoveAxis)
			totalTime_ += double(axis->regionAt(0)->regionTime());

		else if (axis->axisType() == AMScanAxis::StepAxis)
			totalTime_ *= axis->numberOfPoints();
	}
}

void AMGenericContinuousScanConfiguration::setControl(int axisId, AMControlInfo newInfo)
{
	if (axisId == 0 && axisControlInfos_.isEmpty()){

		axisControlInfos_.append(newInfo);
		setModified(true);

		AMScanAxisRegion *region = new AMScanAxisRegion;
		AMScanAxis *axis = new AMScanAxis(AMScanAxis::ContinuousMoveAxis, region);
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

void AMGenericContinuousScanConfiguration::removeControl(int axisId)
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

void AMGenericContinuousScanConfiguration::addDetector(AMDetectorInfo newInfo)
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

void AMGenericContinuousScanConfiguration::removeDetector(AMDetectorInfo info)
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

void AMGenericContinuousScanConfiguration::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void AMGenericContinuousScanConfiguration::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

void AMGenericContinuousScanConfiguration::setI0(const AMDetectorInfo &info)
{
	bool nameInDetectorList = false;

	for (int i = 0, size = detectorConfigurations_.count(); i < size && !nameInDetectorList; i++)
		if (info.name() == detectorConfigurations_.at(i).name())
			nameInDetectorList = true;

	if ((info.isValid() && nameInDetectorList) || !info.isValid()){
		i0_ = info;
		setModified(true);

		emit i0DetectorChanged();
	}
}

AMDbObjectList AMGenericContinuousScanConfiguration::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void AMGenericContinuousScanConfiguration::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

QString AMGenericContinuousScanConfiguration::regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const
{
	QString string = "";

	if (!regions.isEmpty()){

		string.append("\nRegions Of Interest\n");

		foreach (AMRegionOfInterest *region, regions)
			string.append(QString("%1\t%2 eV\t%3 eV\n").arg(region->name()).arg(region->lowerBound()).arg(region->upperBound()));
	}

	return string;
}
