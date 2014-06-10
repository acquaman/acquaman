/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

AMBeamline* AMBeamline::instance_ = 0;


AMBeamline::AMBeamline(const QString& controlName)
	: AMControl(controlName, "")
{
	exposedControls_ = new AMControlSet(this);
	exposedDetectors_ = new AMDetectorSet(this);
}

AMBeamline::~AMBeamline()
{
}



void AMBeamline::releaseBl() {

	if(instance_) {
		delete instance_;
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
			detector->addRegionOfInterest(newRegion->createCopy());
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
