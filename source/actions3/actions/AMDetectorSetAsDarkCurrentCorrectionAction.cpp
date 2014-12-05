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


#include "AMDetectorSetAsDarkCurrentCorrectionAction.h"

//#include "beamline/AMBeamline.h"
#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"



AMDetectorSetAsDarkCurrentCorrectionAction::AMDetectorSetAsDarkCurrentCorrectionAction(AMDetectorSetAsDarkCurrentCorrectionActionInfo *info, AMDetector *detector, QObject *parent) :
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

AMDetectorSetAsDarkCurrentCorrectionAction::~AMDetectorSetAsDarkCurrentCorrectionAction(){}

AMDetectorSetAsDarkCurrentCorrectionAction::AMDetectorSetAsDarkCurrentCorrectionAction(const AMDetectorSetAsDarkCurrentCorrectionAction &other) :
	AMAction3(other)
{
	const AMDetectorSetAsDarkCurrentCorrectionActionInfo *info = qobject_cast<const AMDetectorSetAsDarkCurrentCorrectionActionInfo*>(other.info());

	//if(info)
	//	detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorSetAsDarkCurrentCorrectionInfo()->detectorInfo()));
	if(info && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorSetAsDarkCurrentCorrectionInfo()->detectorInfo()));
	else
		detector_ = 0;
}

void AMDetectorSetAsDarkCurrentCorrectionAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	//if(!detector_)
	//	detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorSetAsDarkCurrentCorrectionInfo()->detectorInfo()));
	if(!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorSetAsDarkCurrentCorrectionInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
				  AMDETECTORSETASDARKCURRENTCORRECTIONACTION_NO_VALID_DETECTOR,
				  QString("There was an error setting up the dark current on %1. Please report this problem to the Acquaman developers.").arg(detectorSetAsDarkCurrentCorrectionInfo()->name()));
		setFailed();
		return;
	}

	detector_->setAsDarkCurrentMeasurementValue();
	setStarted();
	setSucceeded();
}

void AMDetectorSetAsDarkCurrentCorrectionAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}
