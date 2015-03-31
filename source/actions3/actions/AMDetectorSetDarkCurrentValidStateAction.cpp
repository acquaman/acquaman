#include "AMDetectorSetDarkCurrentValidStateAction.h"
#include "beamline/AMBeamlineSupport.h"

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
		setFailed();
	}
}

void AMDetectorSetDarkCurrentValidStateAction::setDetector(AMDetector *newDetector)
{
	detector_ = newDetector;

	if (!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(detectorSetDarkCurrentValidStateActionInfo()->detectorInfo()) );
}
