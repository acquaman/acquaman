#include "AMDetectorCleanupAction.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

 AMDetectorCleanupAction::~AMDetectorCleanupAction(){}
AMDetectorCleanupAction::AMDetectorCleanupAction(AMDetectorCleanupActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
}

AMDetectorCleanupAction::AMDetectorCleanupAction(const AMDetectorCleanupAction &other) :
	AMAction3(other)
{
	const AMDetectorCleanupActionInfo *info = qobject_cast<const AMDetectorCleanupActionInfo*>(other.info());

	if(info)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0;
}

void AMDetectorCleanupAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorCleanupInfo()->detectorInfo()));

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
