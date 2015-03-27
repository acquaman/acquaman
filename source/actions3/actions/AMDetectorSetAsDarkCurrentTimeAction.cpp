#include "AMDetectorSetAsDarkCurrentTimeAction.h"
#include "beamline/AMBeamlineSupport.h"

AMDetectorSetAsDarkCurrentTimeAction::AMDetectorSetAsDarkCurrentTimeAction(AMDetectorSetAsDarkCurrentTimeActionInfo *info, AMDetector *detector, QObject *parent) :
    AMAction3(info, parent)
{
	detector_ = 0;

	if (detector)
		detector_ = detector;
	else if (AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(info->detectorInfo()) );
}

AMDetectorSetAsDarkCurrentTimeAction::AMDetectorSetAsDarkCurrentTimeAction(const AMDetectorSetAsDarkCurrentTimeAction &other) :
	AMAction3(other)
{

}

AMDetectorSetAsDarkCurrentTimeAction::~AMDetectorSetAsDarkCurrentTimeAction()
{

}

void AMDetectorSetAsDarkCurrentTimeAction::startImplementation()
{
	if (!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(detectorSetAsDarkCurrentTimeActionInfo()->detectorInfo()) );

	if (detector_) {
		setStarted();
		detector_->setAsDarkCurrentMeasurementTime(detectorSetAsDarkCurrentTimeActionInfo()->darkCurrentTime());
		setSucceeded();

	}
}
