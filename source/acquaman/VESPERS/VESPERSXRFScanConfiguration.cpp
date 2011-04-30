#include "VESPERSXRFScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(VESPERSBeamline::XRFDetectorChoice choice, XRFDetectorInfo info, QObject *parent)
	: AMScanConfiguration(parent)
{
	xrfDetectorInfo_ = info;
	setDetectorChoice(choice);
}

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
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
	setModified(true);
}

QString VESPERSXRFScanConfiguration::detailedDescription() const
{
	if (choice_ == VESPERSBeamline::SingleElement)
		return QString("XRF Free Run Scan\nDetector: 1-el Vortex\nReal time: %1 s").arg(xrfDetectorInfo_.integrationTime());
	else if (choice_ == VESPERSBeamline::FourElement)
		return QString("XRF Free Run Scan\nDetector: 4-el Vortex\nReal time: %1 s").arg(xrfDetectorInfo_.integrationTime());
}
