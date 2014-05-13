#include "AMDetectorDwellTimeAction.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

 AMDetectorDwellTimeAction::~AMDetectorDwellTimeAction(){}
AMDetectorDwellTimeAction::AMDetectorDwellTimeAction(AMDetectorDwellTimeActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	dwellTimeSource_ = 0; //NULL
	if(detector)
		detector_ = detector;
	else
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
}

AMDetectorDwellTimeAction::AMDetectorDwellTimeAction(const AMDetectorDwellTimeAction &other) :
	AMAction3(other)
{
	const AMDetectorDwellTimeActionInfo *info = qobject_cast<const AMDetectorDwellTimeActionInfo*>(other.info());

	if(info)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0;

	dwellTimeSource_ = other.dwellTimeSource_;
}

#include "beamline/AMDetectorTriggerSource.h"
void AMDetectorDwellTimeAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorDwellTimeInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORDWELLTIMEACTION_NO_VALID_DETECTOR,
						  QString("There was an error setting the dwell time for detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorDwellTimeInfo()->name()));
		setFailed();
		return;
	}

	if( detectorDwellTimeInfo()->dwellSeconds() <= 0){
		AMErrorMon::alert(this,
						  AMDETECTORDWELLTIMEACTION_NOT_VALID_DWELL,
						  QString("There was an error setting the dwell time for detector '%1', because the requested dwell time was invalid (%2). Please report this problem to the Acquaman developers.").arg(detectorDwellTimeInfo()->name()).arg(detectorDwellTimeInfo()->dwellSeconds()));
		setFailed();
		return;
	}


	if(detector_->detectorDwellTimeSource()){

		dwellTimeSource_ = detector_->detectorDwellTimeSource();
		connect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellSetStarted(double)));
		connect(dwellTimeSource_, SIGNAL(succeeded()), this, SLOT(onDwellSetSucceeded()));
		connect(dwellTimeSource_, SIGNAL(failed()), this, SLOT(onDwellSetFailed()));

		detectorDwellTimeInfo()->setShortDescription(QString("Set %1 Dwell Time to %2").arg(dwellTimeSource_->name()).arg(detectorDwellTimeInfo()->dwellSeconds()));
		detectorDwellTimeInfo()->setLongDescription(QString("Set %1 Dwell Time to %2").arg(dwellTimeSource_->name()).arg(detectorDwellTimeInfo()->dwellSeconds()));
		dwellTimeSource_->requestSetDwellTime(detectorDwellTimeInfo()->dwellSeconds());
	}

	else{
		// connect to detector initialization signals
		if(!detector_->acquisitionTimeWithinTolerance(detectorDwellTimeInfo()->dwellSeconds())){

			connect(detector_, SIGNAL(acquisitionTimeChanged(double)), this, SLOT(onDwellTimeChanged(double)));
			detector_->setAcquisitionTime(detectorDwellTimeInfo()->dwellSeconds());
			setStarted();
		}

		else{

			setStarted();
			setSucceeded();
		}
	}
}

void AMDetectorDwellTimeAction::onDwellSetStarted(double dwellSeconds){
	if(dwellTimeSource_)
		disconnect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellSetStarted(double)));

	if(dwellSeconds != detectorDwellTimeInfo()->dwellSeconds())
		setFailed();
	else
		setStarted();
}

void AMDetectorDwellTimeAction::onDwellSetSucceeded(){
	if(dwellTimeSource_)
		disconnect(dwellTimeSource_, 0, this, 0);
	disconnect(detector_, 0, this, 0);

	setSucceeded();
}

void AMDetectorDwellTimeAction::onDwellTimeChanged(double dwellSeconds){
	disconnect(detector_, 0, this, 0);

	if(dwellSeconds != detectorDwellTimeInfo()->dwellSeconds())
		setFailed();
	else
		setSucceeded();
}

void AMDetectorDwellTimeAction::onDwellSetFailed(){
	if(dwellTimeSource_)
		disconnect(dwellTimeSource_, 0, this, 0);
	disconnect(detector_, 0, this, 0);
	setFailed();
}

void AMDetectorDwellTimeAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}
