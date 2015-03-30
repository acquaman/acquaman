#include "AMDetectorSetDarkCurrentTimeAction.h"
#include "beamline/AMBeamlineSupport.h"

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
	setDetector(other.detector_);
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
	}
}

void AMDetectorSetDarkCurrentTimeAction::setDetector(AMDetector *newDetector)
{
	detector_ = newDetector;

	if (!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(detectorSetDarkCurrentTimeActionInfo()->detectorInfo()) );
}
