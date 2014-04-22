#include "AMDetectorTriggerAction.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

 AMDetectorTriggerAction::~AMDetectorTriggerAction(){}
AMDetectorTriggerAction::AMDetectorTriggerAction(AMDetectorTriggerActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	triggerSource_ = 0; //NULL
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

	triggerSource_ = other.triggerSource_;
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


	if(detector_->detectorTriggerSource()){
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
		detector_->acquire(detectorTriggerInfo()->readMode());
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
