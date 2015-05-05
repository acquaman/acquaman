#include "AMDetectorSetDarkCurrentValidStateAction.h"
#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"

AMDetectorSetDarkCurrentValidStateAction::AMDetectorSetDarkCurrentValidStateAction(AMDetectorSetDarkCurrentValidStateActionInfo *info, AMDetector *detector, QObject *parent) :
    AMAction3(info, parent)
{
	detector_ = 0;
	setDetector(detector);
}

AMDetectorSetDarkCurrentValidStateAction::AMDetectorSetDarkCurrentValidStateAction(const AMDetectorSetDarkCurrentValidStateAction &other) :
	AMAction3(other)
{
	detector_ = 0;
	setDetector(other.detector());
}

AMDetectorSetDarkCurrentValidStateAction::~AMDetectorSetDarkCurrentValidStateAction()
{

}

void AMDetectorSetDarkCurrentValidStateAction::startImplementation()
{
	if (!detector_)
		setDetector(0);

	if (detector_) {
		setStarted();
		detector_->setDarkCurrentValidState( detectorSetDarkCurrentValidStateActionInfo()->darkCurrentState() );
		setSucceeded();

	} else {
		AMErrorMon::alert(this, AMDETECTORSETDARKCURRENTVALIDSTATEACTION_DETECTOR_NOT_FOUND, "Failed to set dark current valid state--detector not found.");
		setFailed();
	}
}

bool AMDetectorSetDarkCurrentValidStateAction::setDetector(AMDetector *newDetector)
{
	bool result = false;

	detector_ = newDetector;

	if (!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(detectorSetDarkCurrentValidStateActionInfo()->detectorInfo()) );

	if (detector_)
		result = true;

	return result;
}
