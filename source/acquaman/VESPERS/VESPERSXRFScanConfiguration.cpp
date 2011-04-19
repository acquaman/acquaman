#include "VESPERSXRFScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(XRFDetector *detector, QObject *parent)
	: AMScanConfiguration(parent)
{
	detector_ = detector;

	switch(detector->elements()){

	case 1: choice_ = VESPERSBeamline::SingleElement;
		break;
	case 4: choice_ = VESPERSBeamline::FourElement;
		break;
	}

	xrfDetectorInfo_ = detector_->toXRFInfo();
}

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	detector_ = 0;
	setDetectorChoice(VESPERSBeamline::SingleElement);
}

AMScanConfiguration *VESPERSXRFScanConfiguration::createCopy() const
{
	return new VESPERSXRFScanConfiguration(*this);
}

AMScanController *VESPERSXRFScanConfiguration::createController()
{
	return new VESPERSXRFScanController(this);
}

void VESPERSXRFScanConfiguration::setDetectorChoice(VESPERSBeamline::XRFDetectorChoice choice)
{
	choice_ = choice;

	switch(choice_){
	case VESPERSBeamline::SingleElement:
		detector_ = VESPERSBeamline::vespers()->vortexXRF1E();
		break;
	case VESPERSBeamline::FourElement:
		detector_ = VESPERSBeamline::vespers()->vortexXRF4E();
		break;
	}

	xrfDetectorInfo_ = detector_->toXRFInfo();
	setModified(true);
}
