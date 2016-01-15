#include "AMDetectorWaitForAcquisitionStateAction.h"

#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"

AMDetectorWaitForAcquisitionStateAction::AMDetectorWaitForAcquisitionStateAction(AMDetectorWaitForAcquisitionStateActionInfo *info, AMDetector *detector, QObject *parent)
	: AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else if(AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0; //NULL
}


AMDetectorWaitForAcquisitionStateAction::AMDetectorWaitForAcquisitionStateAction(const AMDetectorWaitForAcquisitionStateAction &other)
	: AMAction3(other)
{
	const AMDetectorWaitForAcquisitionStateActionInfo *info = qobject_cast<const AMDetectorWaitForAcquisitionStateActionInfo*>(other.info());

	//if(info)
	//	detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorInitializeInfo()->detectorInfo()));
	if(info && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorWaitForAcquisitionStateInfo()->detectorInfo()));
	else
		detector_ = 0;
}

AMDetectorWaitForAcquisitionStateAction::~AMDetectorWaitForAcquisitionStateAction()
{

}

void AMDetectorWaitForAcquisitionStateAction::startImplementation()
{
	if(!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorWaitForAcquisitionStateInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORINITIALIZEACTION_NO_VALID_DETECTOR,
						  QString("There was an error initializing the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorWaitForAcquisitionStateInfo()->name()));
		setFailed();
		return;
	}

	if(detector_->acquisitionState() != AMDetector::AcqusitionState(detectorWaitForAcquisitionStateInfo()->acquisitionState())){
		// connect to detector acquisition signal
		connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onAcquisitionStateChanged(AMDetector::AcqusitionState)));
		setStarted();
	}
	else{
		setStarted();
		setSucceeded();
	}
}

void AMDetectorWaitForAcquisitionStateAction::cancelImplementation()
{
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}

void AMDetectorWaitForAcquisitionStateAction::onAcquisitionStateChanged(AMDetector::AcqusitionState acquisitionState)
{
	if (detectorWaitForAcquisitionStateInfo()->acquisitionState() == int(acquisitionState)){

		connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onAcquisitionStateChanged(AMDetector::AcqusitionState)));
		setSucceeded();
	}
}
