#include "AMDetectorAcquisitionAction.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMDetectorAcquisitionAction::AMDetectorAcquisitionAction(AMDetectorAcquisitionActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
}

AMDetectorAcquisitionAction::AMDetectorAcquisitionAction(const AMDetectorAcquisitionAction &other) :
	AMAction3(other)
{
	const AMDetectorAcquisitionActionInfo *info = qobject_cast<const AMDetectorAcquisitionActionInfo*>(other.info());

	if(info)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0;
}

void AMDetectorAcquisitionAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorAcquisitionInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORACQUISITIONACTION_NO_VALID_DETECTOR,
						  QString("There was an error acquiring with the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorAcquisitionInfo()->name()));
		setFailed();
		return;
	}

	if( (detectorAcquisitionInfo()->readMode() != AMDetectorDefinitions::SingleRead) && !detector_->canContinuousAcquire()){
		AMErrorMon::alert(this,
						  AMDETECTORACQUISITIONACTION_NOT_VALID_READMODE,
						  QString("There was an error acquiring with the detector '%1', because continuous reads are not supported for this detector. Please report this problem to the Acquaman developers.").arg(detectorAcquisitionInfo()->name()));
		setFailed();
		return;
	}

	// connect to detector initialization signals
	connect(detector_, SIGNAL(acquiring()), this, SLOT(onAcquisitionStarted()));
	connect(detector_, SIGNAL(acquisitionSucceeded()), this, SLOT(onAcquisitionSucceeded()));
	connect(detector_, SIGNAL(acquisitionFailed()), this, SLOT(onAcquisitionFailed()));

	detector_->acquire(detectorAcquisitionInfo()->readMode());
}

void AMDetectorAcquisitionAction::onAcquisitionStarted(){
	disconnect(detector_, SIGNAL(acquiring()), this, SLOT(onAcquisitionStarted()));

	setStarted();
	/*
	if(acquisitionStartSuccessful)
		setStarted();
	else{
		AMErrorMon::alert(this,
						  AMDETECTORACQUISITIONACTION_ACQUISITION_START_FAILED,
						  QString("There was an error acquiring with the detector '%1', because the acquisition failed to start. Please report this problem to the Acquaman developers.").arg(detectorAcquisitionInfo()->name()));
		setFailed();
		return;
	}
	*/
}

void AMDetectorAcquisitionAction::onAcquisitionSucceeded(){
	disconnect(detector_, 0, this, 0);
	setSucceeded();
}

void AMDetectorAcquisitionAction::onAcquisitionFailed(){
	disconnect(detector_, 0, this, 0);
	setFailed();
}

void AMDetectorAcquisitionAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}
