#include "AMDetectorSetLastMeasurementAsDarkCurrentAction.h"
#include "beamline/AMBeamlineSupport.h"

AMDetectorSetLastMeasurementAsDarkCurrentAction::AMDetectorSetLastMeasurementAsDarkCurrentAction(AMDetectorSetLastMeasurementAsDarkCurrentActionInfo *info, AMDetector *detector, QObject *parent) :
    AMAction3(info, parent)
{
	detector_ = 0;
	setDetector(detector);
}

AMDetectorSetLastMeasurementAsDarkCurrentAction::AMDetectorSetLastMeasurementAsDarkCurrentAction(const AMDetectorSetLastMeasurementAsDarkCurrentAction &other) :
	AMAction3(other)
{
	detector_ = 0;
	setDetector(other.detector_);
}

AMDetectorSetLastMeasurementAsDarkCurrentAction::~AMDetectorSetLastMeasurementAsDarkCurrentAction()
{

}

void AMDetectorSetLastMeasurementAsDarkCurrentAction::startImplementation()
{
	if (!detector_)
		setDetector(0);

	if (detector_) {
		setStarted();
		detector_->setLastMeasurementAsDarkCurrent();
		setSucceeded();
	}
}

void AMDetectorSetLastMeasurementAsDarkCurrentAction::setDetector(AMDetector *newDetector)
{
	detector_ = newDetector;

	if (!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(detectorSetLastMeasurementAsDarkCurrentActionInfo()->detectorInfo()) );
}
