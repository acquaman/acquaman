#include "AMGenericStepScanConfiguration.h"

#include <math.h>

#include "acquaman/AMGenericStepScanController.h"
#include "beamline/AMBeamline.h"

#include "ui/acquaman/AMGenericStepScanConfigurationView.h"


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
	return "Generic Step";
}

QString AMGenericStepScanConfiguration::description() const
{
	return "Generic Scan";
}

QString AMGenericStepScanConfiguration::detailedDescription() const
{
	return "Does a generic scan over one or two controls with a variety of detectors.";
}

bool AMGenericStepScanConfiguration::hasI0() const
{
	return i0_.isValid();
}

void AMGenericStepScanConfiguration::addRegion(int scanAxisIndex, int regionIndex, AMScanAxisRegion *region)
{
	AMScanAxis *scanAxis = scanAxisAt(scanAxisIndex);

	if (scanAxis && region) {
		scanAxis->insertRegion(regionIndex, region);
		connectRegion(region);
	}

	computeTotalTime();
}

void AMGenericStepScanConfiguration::connectRegion(AMScanAxisRegion *region)
{
	if (region) {
		connect( region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()) );
		connect( region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()) );
		connect( region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()) );
		connect( region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()) );
	}
}

void AMGenericStepScanConfiguration::removeRegion(int scanAxisIndex, AMScanAxisRegion *region)
{
	AMScanAxis *scanAxis = scanAxisAt(scanAxisIndex);

	if (scanAxis && region) {
		scanAxis->removeRegion(region);
		disconnectRegion(region);
	}

	computeTotalTime();
}

void AMGenericStepScanConfiguration::disconnectRegion(AMScanAxisRegion *region)
{
	if (region) {
		disconnect( region, 0, this, 0 );
	}
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

bool AMGenericStepScanConfiguration::usingControl(const AMControlInfo &controlInfo) const
{
	return (axisControlInfos_.indexOf(controlInfo.name()) != -1);
}

bool AMGenericStepScanConfiguration::usingDetector(const QString &detectorName) const
{
	return (detectorConfigurations_.indexOf(detectorName) != -1);
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

		emit detectorsChanged();
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

			emit detectorsChanged();
		}
	}
}

void AMGenericStepScanConfiguration::addRegionOfInterest(AMRegionOfInterest *region)
{
	regionsOfInterest_.append(region);
	setModified(true);
}

void AMGenericStepScanConfiguration::removeRegionOfInterest(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeRemoved, regionsOfInterest_)
		if (regionToBeRemoved->name() == region->name()){

			regionsOfInterest_.removeOne(regionToBeRemoved);
			setModified(true);
		}
}

void AMGenericStepScanConfiguration::setRegionOfInterestBoundingRange(AMRegionOfInterest *region)
{
	foreach (AMRegionOfInterest *regionToBeUpdated, regionsOfInterest_)
		if (regionToBeUpdated->name() == region->name()){

			regionToBeUpdated->setBoundingRange(region->boundingRange());
			setModified(true);
		}
}

void AMGenericStepScanConfiguration::setI0(const AMDetectorInfo &info)
{
	bool nameInDetectorList = false;

	for (int i = 0, size = detectorConfigurations_.count(); i < size && !nameInDetectorList; i++)
		if (info.name() == detectorConfigurations_.at(i).name())
			nameInDetectorList = true;

	if ((info.isValid() && nameInDetectorList) || !info.isValid()) {
		i0_ = info;
		setModified(true);

		emit i0DetectorChanged();
	}
}

AMDbObjectList AMGenericStepScanConfiguration::dbReadRegionsOfInterest()
{
	AMDbObjectList listToBeSaved;

	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		listToBeSaved << region;

	return listToBeSaved;
}

void AMGenericStepScanConfiguration::dbLoadRegionsOfInterest(const AMDbObjectList &newRegions)
{
	regionsOfInterest_.clear();

	foreach (AMDbObject *newObject, newRegions){

		AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(newObject);

		if (region)
			regionsOfInterest_.append(region);
	}
}

QString AMGenericStepScanConfiguration::regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const
{
	QString string = "";

	if (!regions.isEmpty()){

		string.append("\nRegions Of Interest\n");

		foreach (AMRegionOfInterest *region, regions)
			string.append(QString("%1\t%2 eV\t%3 eV\n").arg(region->name()).arg(region->lowerBound()).arg(region->upperBound()));
	}

	return string;
}
