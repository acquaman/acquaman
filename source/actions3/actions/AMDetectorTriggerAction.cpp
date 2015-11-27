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


#include "AMDetectorTriggerAction.h"

#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"

AMDetectorTriggerAction::AMDetectorTriggerAction(AMDetectorTriggerActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	triggerSource_ = 0; //NULL
	if(detector)
		detector_ = detector;
	else if(AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0; //NULL
}

AMDetectorTriggerAction::~AMDetectorTriggerAction()
{
}

AMDetectorTriggerAction::AMDetectorTriggerAction(const AMDetectorTriggerAction &other) :
	AMAction3(other)
{
	const AMDetectorTriggerActionInfo *info = qobject_cast<const AMDetectorTriggerActionInfo*>(other.info());

	if(info && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorTriggerInfo()->detectorInfo()));
	else
		detector_ = 0;

	triggerSource_ = other.triggerSource_;
}

#include "beamline/AMDetectorTriggerSource.h"
void AMDetectorTriggerAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorTriggerInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
				  AMDETECTORTRIGGERACTION_NO_VALID_DETECTOR,
				  QString("There was an error trigger the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorTriggerInfo()->detectorInfo()->name()));
		setFailed();
		return;
	}

	if(!detector_->isReadyForAcquisition()) {
		AMErrorMon::alert(this,
				  AMDETECTORTRIGGERACTION_NOT_READY,
				  QString("There was an error triggering the detector '%1', because the detector reports it is not ready for acquisition. Please report to the Acquaman developers.").arg(detectorTriggerInfo()->detectorInfo()->name()));
		setFailed();
		return;
	}

	if( (detectorTriggerInfo()->readMode() != AMDetectorDefinitions::SingleRead) && !detector_->canContinuousAcquire()){
		AMErrorMon::alert(this,
				  AMDETECTORTRIGGERACTION_NOT_VALID_READMODE,
				  QString("There was an error triggering the detector '%1', because continuous reads are not supported for this detector. Please report this problem to the Acquaman developers.").arg(detectorTriggerInfo()->detectorInfo()->name()));
		setFailed();
		return;
	}


	if(detector_->detectorTriggerSource() && detectorTriggerInfo()->readMode() == AMDetectorDefinitions::SingleRead){
		triggerSource_ = detector_->detectorTriggerSource();
		connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onAcquisitionStarted()));
		connect(triggerSource_, SIGNAL(succeeded()), this, SLOT(onAcquisitionSucceeded()));
		connect(triggerSource_, SIGNAL(failed()), this, SLOT(onAcquisitionFailed()));

		detectorTriggerInfo()->setShortDescription(QString("Trigger %1").arg(triggerSource_->name()));
		detectorTriggerInfo()->setLongDescription(QString("Trigger %1").arg(triggerSource_->name()));
		triggerSource_->trigger(detectorTriggerInfo()->readMode());
	}
	else{
		// connect to detector initialization signals
		connect(detector_, SIGNAL(acquiring()), this, SLOT(onAcquisitionStarted()));
		connect(detector_, SIGNAL(acquisitionSucceeded()), this, SLOT(onAcquisitionSucceeded()));
		connect(detector_, SIGNAL(acquisitionFailed()), this, SLOT(onAcquisitionFailed()));

		if (detector_->isReadyForAcquisition()){
			if((detectorTriggerInfo()->readMode() == AMDetectorDefinitions::ContinuousRead) && (detectorTriggerInfo()->continousWindowSeconds() > 0))
				detector_->setContinuousDataWindow(detectorTriggerInfo()->continousWindowSeconds());
			if(!detector_->acquire(detectorTriggerInfo()->readMode())) {

				AMErrorMon::alert(this,
						  AMDETECTORTRIGGERACTION_TRIGGER_FAILED,
						  QString("There was an error triggering the detector '%1', because the detector responded that it cannot currently acquire. Please report this problem to the Acquaman developers.").arg(detectorTriggerInfo()->detectorInfo()->name()));

				setFailed();
				return;
			}

		}

		else
			connect(detector_, SIGNAL(readyForAcquisition()), this, SLOT(onDetectorReadyForAcquisition()));
	}
}

void AMDetectorTriggerAction::onAcquisitionStarted(){
	if(triggerSource_)
		disconnect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onAcquisitionStarted()));
	else
		disconnect(detector_, SIGNAL(acquiring()), this, SLOT(onAcquisitionStarted()));

	setStarted();
}

void AMDetectorTriggerAction::onAcquisitionSucceeded(){
	if(triggerSource_)
		disconnect(triggerSource_, 0, this, 0);
	disconnect(detector_, 0, this, 0);

	setSucceeded();
}

void AMDetectorTriggerAction::onAcquisitionFailed(){
	if(triggerSource_)
		disconnect(triggerSource_, 0, this, 0);
	disconnect(detector_, 0, this, 0);
	setFailed();
}

void AMDetectorTriggerAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}

void AMDetectorTriggerAction::onDetectorReadyForAcquisition()
{
	disconnect(detector_, SIGNAL(readyForAcquisition()), this, SLOT(onDetectorReadyForAcquisition()));
	if(!detector_->acquire(detectorTriggerInfo()->readMode())) {

		AMErrorMon::alert(this,
				  AMDETECTORTRIGGERACTION_TRIGGER_FAILED,
				  QString("There was an error triggering the detector '%1', because the detector responded that it cannot currently acquire. Please report this problem to the Acquaman developers.").arg(detectorTriggerInfo()->name()));

		setFailed();
		return;
	}
}
