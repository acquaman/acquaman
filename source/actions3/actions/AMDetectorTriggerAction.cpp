#include "AMDetectorTriggerAction.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMDetectorTriggerAction::AMDetectorTriggerAction(AMDetectorTriggerActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
}

AMDetectorTriggerAction::AMDetectorTriggerAction(const AMDetectorTriggerAction &other) :
	AMAction3(other)
{
	const AMDetectorTriggerActionInfo *info = qobject_cast<const AMDetectorTriggerActionInfo*>(other.info());

	if(info)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0;
}

#include "beamline/AMDetectorTriggerSource.h"
void AMDetectorTriggerAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorTriggerInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORTRIGGERACTION_NO_VALID_DETECTOR,
						  QString("There was an error trigger the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorTriggerInfo()->name()));
		setFailed();
		return;
	}

	if( (detectorTriggerInfo()->readMode() != AMDetectorDefinitions::SingleRead) && !detector_->canContinuousAcquire()){
		AMErrorMon::alert(this,
						  AMDETECTORTRIGGERACTION_NOT_VALID_READMODE,
						  QString("There was an error triggering the detector '%1', because continuous reads are not supported for this detector. Please report this problem to the Acquaman developers.").arg(detectorTriggerInfo()->name()));
		setFailed();
		return;
	}

	// connect to detector initialization signals
	connect(detector_, SIGNAL(acquiring()), this, SLOT(onAcquisitionStarted()));
	connect(detector_, SIGNAL(acquisitionSucceeded()), this, SLOT(onAcquisitionSucceeded()));
	connect(detector_, SIGNAL(acquisitionFailed()), this, SLOT(onAcquisitionFailed()));

	if(detector_->detectorTriggerSource()){
		detectorTriggerInfo()->setShortDescription(QString("Trigger %1").arg(detector_->detectorTriggerSource()->name()));
		detectorTriggerInfo()->setLongDescription(QString("Trigger %1").arg(detector_->detectorTriggerSource()->name()));
		detector_->detectorTriggerSource()->trigger(detectorTriggerInfo()->readMode());
	}
	else
		detector_->acquire(detectorTriggerInfo()->readMode());
}

void AMDetectorTriggerAction::onAcquisitionStarted(){
	disconnect(detector_, SIGNAL(acquiring()), this, SLOT(onAcquisitionStarted()));

	setStarted();
}

void AMDetectorTriggerAction::onAcquisitionSucceeded(){
	disconnect(detector_, 0, this, 0);

	setSucceeded();
}

void AMDetectorTriggerAction::onAcquisitionFailed(){
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
