#include "AMDetectorSetDarkCurrentTimeAction.h"
#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"

AMDetectorSetDarkCurrentTimeAction::AMDetectorSetDarkCurrentTimeAction(AMDetectorSetDarkCurrentTimeActionInfo *info, AMDetector *detector, QObject *parent) :
    AMAction3(info, parent)
{
	detector_ = 0;
	setDetector(detector);
}

AMDetectorSetDarkCurrentTimeAction::AMDetectorSetDarkCurrentTimeAction(const AMDetectorSetDarkCurrentTimeAction &other) :
	AMAction3(other)
{
	detector_ = 0;
	setDetector(other.detector());
}

AMDetectorSetDarkCurrentTimeAction::~AMDetectorSetDarkCurrentTimeAction()
{

}

void AMDetectorSetDarkCurrentTimeAction::startImplementation()
{
	if (!detector_)
		setDetector(0);

	if (detector_) {
		setStarted();
		detector_->setDarkCurrentTime(detectorSetDarkCurrentTimeActionInfo()->darkCurrentTime());
		setSucceeded();

	} else {
		AMErrorMon::alert(this, AMDETECTORSETDARKCURRENTTIMEACTION_DETECTOR_NOT_FOUND, "Failed to set detector dark current dwell time--detector not found.");
		setFailed();
	}
}

bool AMDetectorSetDarkCurrentTimeAction::setDetector(AMDetector *newDetector)
{
	bool result = false;

	detector_ = newDetector;

	if (!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(detectorSetDarkCurrentTimeActionInfo()->detectorInfo()) );

	if (detector_)
		result = true;

	return result;
}
