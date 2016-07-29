/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMBeamline.h"

#include "util/AMErrorMonitor.h"

#include "dataman/AMSamplePlate.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include "acquaman/AMStepScanConfiguration.h"

AMBeamline* AMBeamline::instance_ = 0;

void AMBeamline::releaseBl() {

	if(instance_) {
		instance_->deleteLater();
		instance_ = 0;
	}

}

AMBeamline * AMBeamline::bl()
{
	 if(!instance_) {
		 AMErrorMon::alert(0, AMBEAMLINE_BEAMLINE_NOT_CREATED_YET, "No beamline created yet.  You need to call YourBeamline::bl() before calling AMBeamline::bl().");
	 }

	return instance_;
}

AMBeamline::AMBeamline(const QString& controlName)
	: AMControl(controlName, ""), AMBeamlineControlAPI(), AMBeamlineControlSetAPI(), AMBeamlineDetectorAPI(), AMBeamlineDetectorSetAPI(), AMBeamlineSynchronizedDwellTimeAPI()
{
	exposedControls_ = new AMControlSet(this);
	exposedDetectors_ = new AMDetectorSet(this);
	exposedScientificDetectors_ = new AMDetectorSet(this);
	samplePlate_ = 0; //NULL
	samplePlateBrowser_ = 0; //NULL
	currentSamples_ = QList<AMSample*>();
}

AMBeamline::~AMBeamline()
{
}

void AMBeamline::initializeBeamline()
{
	// initialize beamline support
	AMBeamlineSupport::setBeamlineControlAPI(AMBeamline::AMBeamline::bl());
	AMBeamlineSupport::setBeamlineControlSetAPI(AMBeamline::bl());
	AMBeamlineSupport::setBeamineDetectorAPI(AMBeamline::bl());
	AMBeamlineSupport::setBeamlineDetectorSetAPI(AMBeamline::bl());
	AMBeamlineSupport::setBeamlineSynchronizedDwellTimeAPI(AMBeamline::bl());
}

AMSamplePlate* AMBeamline::samplePlate(){
	return samplePlate_;
}

AMSamplePlateBrowser* AMBeamline::samplePlateBrowser(){
	if(!samplePlateBrowser_){
		samplePlateBrowser_ = new AMSamplePlateBrowser(AMDatabase::database("user"), this);
		connect(this, SIGNAL(samplePlateChanged(AMSamplePlate*)), samplePlateBrowser_, SLOT(setCurrentSamplePlate(AMSamplePlate*)));
	}
	return samplePlateBrowser_;
}

void AMBeamline::setSamplePlate(AMSamplePlate *samplePlate){
	emit samplePlateAboutToChange(samplePlate_);
	samplePlate_ = samplePlate;

	if(samplePlate_)
		samplePlate_->storeToDb(AMDatabase::database("user"));
	emit samplePlateChanged(samplePlate_);
}

QList<AMSample*> AMBeamline::currentSamples() const{
	return currentSamples_;
}

AMSample* AMBeamline::currentSample() const{
	if(currentSamples_.count() == 1)
		return currentSamples_.at(0);
	return 0; //NULL
}

void AMBeamline::setCurrentSamples(QList<AMSample*> sample){
	bool sameSampleList = true;

	if(sample.count() != currentSamples_.count())
		sameSampleList = false;
	else
		for(int x = 0, size = sample.count(); x < size; x++)
			if(sample.at(x) != currentSamples_.at(x))
				sameSampleList = false;

	if(!sameSampleList){
		currentSamples_ = sample;
		emit currentSampleChanged(currentSamples_);
	}
}

bool AMBeamline::detectorAvailable(const AMDetectorInfo &detectorInfo){
	if(exposedDetectors()->detectorNamed(detectorInfo.name()) && exposedDetectors()->detectorNamed(detectorInfo.name())->isConnected())
		return true;
	return false;
}

AMDetectorGroup* AMBeamline::exposedDetectorGroupByName(const QString &name) const{
	for(int x = 0; x < exposedDetectorGroups_.count(); x++)
		if(exposedDetectorGroups_.at(x)->name() == name)
			return exposedDetectorGroups_.at(x);

	return 0; //NULL
}

bool AMBeamline::addExposedDetectorGroup(AMDetectorGroup *detectorGroup){
	for(int x = 0; x < exposedDetectorGroups_.count(); x++)
		if(exposedDetectorGroups_.at(x)->name() == detectorGroup->name())
			return false;

	exposedDetectorGroups_.append(detectorGroup);
	return true;
}

void AMBeamline::addSynchronizedXRFDetector(AMXRFDetector *detector)
{
	synchronizedXRFDetectors_.append(detector);
	connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
	connect(detector, SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));
}

AMAction3* AMBeamline::createTurnOffBeamActions()
{
    return 0;
}

void AMBeamline::onRegionOfInterestAdded(AMRegionOfInterest *newRegion)
{
	foreach (AMXRFDetector *detector, synchronizedXRFDetectors_){

		bool regionNeedsToBeAdded = true;

		foreach (AMRegionOfInterest *region, detector->regionsOfInterest())
			if (newRegion->name() == region->name())
				regionNeedsToBeAdded = false;

		if (regionNeedsToBeAdded)
			detector->addRegionOfInterest(newRegion);
	}
}

void AMBeamline::onRegionOfInterestRemoved(AMRegionOfInterest *removedRegion)
{
	foreach (AMXRFDetector *detector, synchronizedXRFDetectors_){

		AMRegionOfInterest *regionToBeRemoved = 0;

		foreach (AMRegionOfInterest *region, detector->regionsOfInterest())
			if (removedRegion->name() == region->name())
				regionToBeRemoved = region;

		if (regionToBeRemoved)
			detector->removeRegionOfInterest(regionToBeRemoved);
	}
}

void AMBeamline::onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region)
{
	foreach (AMXRFDetector *detector, synchronizedXRFDetectors_)
		foreach (AMRegionOfInterest *tempRegion, detector->regionsOfInterest())
			if (region->name() == tempRegion->name() && tempRegion->boundingRange() != region->boundingRange())
				tempRegion->setBoundingRange(region->boundingRange());
}

AMAction3* AMBeamline::createInitializeScanAxisControlsAction(AMStepScanConfiguration *configuration)
{
	AMAction3* result = 0;

	if (configuration) {

		AMListAction3 *initializationAction = new AMListAction3(new AMListActionInfo3("Initialize step scan axis controls", "Initialize step scan axis controls"), AMListAction3::Parallel);

		// add the move actions to move the axis controls to the start point
		for (int i=0, size=configuration->scanAxes().count(); i < size; i++) {
			AMScanAxis *scanAxis = configuration->scanAxisAt(i);
			AMControl *control = AMBeamline::bl()->exposedControlByInfo(configuration->axisControlInfoAt(i));
			if (scanAxis && control) {
				AMAction3 *moveAxisControlAction = AMActionSupport::buildControlMoveAction(control, scanAxis->regionAt(0)->regionStart());
				initializationAction->addSubAction(moveAxisControlAction);
			}
		}

		result = initializationAction;
	}

	return result;
}

AMAction3* AMBeamline::createScanInitializationAction(AMScanConfiguration *configuration)
{
	AMAction3* result = 0;

	AMStepScanConfiguration *stepScanConfiguration = qobject_cast<AMStepScanConfiguration*>(configuration);

	if (stepScanConfiguration)
		result = createInitializeScanAxisControlsAction(stepScanConfiguration);

	return result;
}
