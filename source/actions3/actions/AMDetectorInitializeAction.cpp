#include "AMDetectorInitializeAction.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

 AMDetectorInitializeAction::~AMDetectorInitializeAction(){}
AMDetectorInitializeAction::AMDetectorInitializeAction(AMDetectorInitializeActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
}

AMDetectorInitializeAction::AMDetectorInitializeAction(const AMDetectorInitializeAction &other) :
	AMAction3(other)
{
	const AMDetectorInitializeActionInfo *info = qobject_cast<const AMDetectorInitializeActionInfo*>(other.info());

	if(info)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0;
}

void AMDetectorInitializeAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorInitializeInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORINITIALIZEACTION_NO_VALID_DETECTOR,
						  QString("There was an error initializing the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorInitializeInfo()->name()));
		setFailed();
		return;
	}

	if(detector_->initializationState() == AMDetector::InitializationRequired){
		// connect to detector initialization signals
		connect(detector_, SIGNAL(initializing()), this, SLOT(onInitializeStarted()));
		connect(detector_, SIGNAL(initialized()), this, SLOT(onInitializeFinished()));

		detector_->initialize();
	}
	else{
		setStarted();
		setSucceeded();
	}
}

void AMDetectorInitializeAction::onInitializeStarted(){
	disconnect(detector_, SIGNAL(initializing()), this, SLOT(onInitializeStarted()));

	setStarted();
}

void AMDetectorInitializeAction::onInitializeFinished(){
	disconnect(detector_, 0, this, 0);

	setSucceeded();
	/*
	if(initializationSucceeded)
		setSucceeded();
	else
		setFailed();
	*/
}

void AMDetectorInitializeAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}
