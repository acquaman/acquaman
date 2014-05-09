#include "AMDetectorSetAsDarkCurrentCorrectionAction.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"



AMDetectorSetAsDarkCurrentCorrectionAction::AMDetectorSetAsDarkCurrentCorrectionAction(AMDetectorSetAsDarkCurrentCorrectionActionInfo *info, AMDetector *detector, QObject *parent) :
    AMAction3(info, parent)
{
    if(detector)
        detector_ = detector;
    else
        detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
}

AMDetectorSetAsDarkCurrentCorrectionAction::~AMDetectorSetAsDarkCurrentCorrectionAction(){}

AMDetectorSetAsDarkCurrentCorrectionAction::AMDetectorSetAsDarkCurrentCorrectionAction(const AMDetectorSetAsDarkCurrentCorrectionAction &other) :
    AMAction3(other)
{
    const AMDetectorSetAsDarkCurrentCorrectionActionInfo *info = qobject_cast<const AMDetectorSetAsDarkCurrentCorrectionActionInfo*>(other.info());

    if(info)
        detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
    else
        detector_ = 0;
}

void AMDetectorSetAsDarkCurrentCorrectionAction::startImplementation(){
    // If you still don't have a detector, check the exposed detectors one last time.
    if(!detector_)
        detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorSetAsDarkCurrentCorrectionInfo()->detectorInfo()));

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
