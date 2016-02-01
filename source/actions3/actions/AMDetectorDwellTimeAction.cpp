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


#include "AMDetectorDwellTimeAction.h"

#include <math.h>

#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"

 AMDetectorDwellTimeAction::~AMDetectorDwellTimeAction(){}
AMDetectorDwellTimeAction::AMDetectorDwellTimeAction(AMDetectorDwellTimeActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	dwellTimeSource_ = 0; //NULL
	if(detector)
		detector_ = detector;
	else if(AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(info->detectorInfo()));
		//detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0; //NULL
}

AMDetectorDwellTimeAction::AMDetectorDwellTimeAction(const AMDetectorDwellTimeAction &other) :
	AMAction3(other)
{
	const AMDetectorDwellTimeActionInfo *info = qobject_cast<const AMDetectorDwellTimeActionInfo*>(other.info());

	//if(info)
	//	detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorDwellTimeInfo()->detectorInfo()));
	if(info && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorDwellTimeInfo()->detectorInfo()));
	else
		detector_ = 0;

	dwellTimeSource_ = other.dwellTimeSource_;
}

#include "beamline/AMDetectorTriggerSource.h"
void AMDetectorDwellTimeAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	//if(!detector_)
	//	detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorDwellTimeInfo()->detectorInfo()));
	if(!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorDwellTimeInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORDWELLTIMEACTION_NO_VALID_DETECTOR,
						  QString("There was an error setting the dwell time for detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorDwellTimeInfo()->name()));
		setFailed();
		return;
	}

	if( detectorDwellTimeInfo()->dwellSeconds() <= 0){
		AMErrorMon::alert(this,
						  AMDETECTORDWELLTIMEACTION_NOT_VALID_DWELL,
						  QString("There was an error setting the dwell time for detector '%1', because the requested dwell time was invalid (%2). Please report this problem to the Acquaman developers.").arg(detectorDwellTimeInfo()->name()).arg(detectorDwellTimeInfo()->dwellSeconds()));
		setFailed();
		return;
	}


	if(detector_->detectorDwellTimeSource()){

		dwellTimeSource_ = detector_->detectorDwellTimeSource();
		connect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellSetStarted(double)));
		connect(dwellTimeSource_, SIGNAL(succeeded()), this, SLOT(onDwellSetSucceeded()));
		connect(dwellTimeSource_, SIGNAL(failed()), this, SLOT(onDwellSetFailed()));

		detectorDwellTimeInfo()->setShortDescription(QString("Set %1 Dwell Time to %2").arg(dwellTimeSource_->name()).arg(detectorDwellTimeInfo()->dwellSeconds()));
		detectorDwellTimeInfo()->setLongDescription(QString("Set %1 Dwell Time to %2").arg(dwellTimeSource_->name()).arg(detectorDwellTimeInfo()->dwellSeconds()));
		dwellTimeSource_->requestSetDwellTime(detectorDwellTimeInfo()->dwellSeconds());
	}

	else{
		// connect to detector initialization signals
		if(!detector_->acquisitionTimeWithinTolerance(detectorDwellTimeInfo()->dwellSeconds())){

			connect(detector_, SIGNAL(acquisitionTimeChanged(double)), this, SLOT(onDwellTimeChanged(double)));
			detector_->setAcquisitionTime(detectorDwellTimeInfo()->dwellSeconds());
			setStarted();
		}

		else{

			setStarted();
			setSucceeded();
		}
	}
}

void AMDetectorDwellTimeAction::onDwellSetStarted(double dwellTime){
	if(dwellTimeSource_)
		disconnect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellSetStarted(double)));

    if(fabs(dwellTime - detectorDwellTimeInfo()->dwellSeconds()) < detector_->acquisitionTimeTolerance())
		setSucceeded();
	else{

		AMErrorMon::alert(this, AMDETECTORDWELLTIMEACTION_TIME_INVALID_ON_START, "Failed to set the dwell time within tolerance.");
		setFailed();
	}
}

void AMDetectorDwellTimeAction::onDwellSetSucceeded(){
	if(dwellTimeSource_)
		disconnect(dwellTimeSource_, 0, this, 0);
	disconnect(detector_, 0, this, 0);

	setSucceeded();
}

void AMDetectorDwellTimeAction::onDwellTimeChanged(double dwellTime){
	disconnect(detector_, 0, this, 0);

	if(fabs(dwellTime - detectorDwellTimeInfo()->dwellSeconds()) < detector_->acquisitionTimeTolerance())
		setSucceeded();

	else{

		AMErrorMon::alert(this, AMDETECTORDWELLTIMEACTION_TIME_INVALID_ON_CHANGED, "Failed to set the dwell time within tolerance.");
		setFailed();
	}
}

void AMDetectorDwellTimeAction::onDwellSetFailed(){
	if(dwellTimeSource_)
		disconnect(dwellTimeSource_, 0, this, 0);
	disconnect(detector_, 0, this, 0);
	setFailed();
}

void AMDetectorDwellTimeAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}
