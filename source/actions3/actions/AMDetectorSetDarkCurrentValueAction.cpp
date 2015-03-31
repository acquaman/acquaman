#include "AMDetectorSetDarkCurrentValueAction.h"
#include "beamline/AMBeamlineSupport.h"

AMDetectorSetDarkCurrentValueAction::AMDetectorSetDarkCurrentValueAction(AMDetectorSetDarkCurrentValueActionInfo *info, AMDetector *detector, QObject *parent) :
    AMAction3(info, parent)
{
	detector_ = 0;
	setDetector(detector);
}

AMDetectorSetDarkCurrentValueAction::AMDetectorSetDarkCurrentValueAction(const AMDetectorSetDarkCurrentValueAction &other) :
	AMAction3(other)
{
	detector_ = 0;
	setDetector(other.detector_);
}

AMDetectorSetDarkCurrentValueAction::~AMDetectorSetDarkCurrentValueAction()
{

}

void AMDetectorSetDarkCurrentValueAction::startImplementation()
{
	if (!detector_)
		setDetector(0);

	if (detector_) {
		setStarted();
		detector_->setDarkCurrentValue(detectorSetDarkCurrentValueActionInfo()->darkCurrentValue());
		setSucceeded();

	} else {
		setFailed();
	}
}

void AMDetectorSetDarkCurrentValueAction::setDetector(AMDetector *newDetector)
{
	detector_ = newDetector;

	if (!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(detectorSetDarkCurrentValueActionInfo()->detectorInfo()) );
}
