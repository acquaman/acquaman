#include "AMDetectorSetDarkCurrentValueAction.h"
#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"

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
	setDetector(other.detector());
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
		AMErrorMon::alert(this, AMDETECTORSETDARKCURRENTVALUEACTION_DETECTOR_NOT_FOUND, "Failed to set detector dark current value--detector not found.");
		setFailed();
	}
}

bool AMDetectorSetDarkCurrentValueAction::setDetector(AMDetector *newDetector)
{
	bool result = false;

	detector_ = newDetector;

	if (!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo( *(detectorSetDarkCurrentValueActionInfo()->detectorInfo()) );

	if (detector_)
		result = true;

	return result;
}
