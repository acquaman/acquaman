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


#include "AMDetectorCleanupAction.h"

//#include "beamline/AMBeamline.h"
#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"

 AMDetectorCleanupAction::~AMDetectorCleanupAction(){}
AMDetectorCleanupAction::AMDetectorCleanupAction(AMDetectorCleanupActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else if(AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(info->detectorInfo()));
		//detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0; //NULL
}

AMDetectorCleanupAction::AMDetectorCleanupAction(const AMDetectorCleanupAction &other) :
	AMAction3(other)
{
	const AMDetectorCleanupActionInfo *info = qobject_cast<const AMDetectorCleanupActionInfo*>(other.info());

	//if(info)
	//	detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorCleanupInfo()->detectorInfo()));
	if(info && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorCleanupInfo()->detectorInfo()));
	else
		detector_ = 0;
}

void AMDetectorCleanupAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	//if(!detector_)
	//	detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorCleanupInfo()->detectorInfo()));
	if(!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorCleanupInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORCLEANUPACTION_NO_VALID_DETECTOR,
						  QString("There was an error cleaning up the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorCleanupInfo()->name()));
		setFailed();
		return;
	}

	if(detector_->cleanupState() == AMDetector::CleanupRequired){
		// connect to detector cleanup signals
		connect(detector_, SIGNAL(cleaningUp()), this, SLOT(onCleanupStarted()));
		connect(detector_, SIGNAL(cleanedUp()), this, SLOT(onCleanupFinished()));

		detector_->cleanup();
	}
	else{
		setStarted();
		setSucceeded();
	}
}

void AMDetectorCleanupAction::onCleanupStarted(){
	disconnect(detector_, SIGNAL(cleaningUp()), this, SLOT(onCleanupStarted()));

	setStarted();
}

void AMDetectorCleanupAction::onCleanupFinished(){
	disconnect(detector_, 0, this, 0);

	setSucceeded();
	/*
	if(cleanupSucceeded)
		setSucceeded();
	else
		setFailed();
	*/
}

void AMDetectorCleanupAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}
