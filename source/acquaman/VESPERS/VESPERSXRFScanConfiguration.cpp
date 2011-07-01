#include "VESPERSXRFScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(XRFDetectorInfo detectorInfo, QObject *parent)
	: AMScanConfiguration(parent)
{
	xrfDetectorInfo_ = detectorInfo;
}

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
}

VESPERSXRFScanConfiguration::~VESPERSXRFScanConfiguration()
{

}

AMScanConfiguration *VESPERSXRFScanConfiguration::createCopy() const
{
	return new VESPERSXRFScanConfiguration(*this);
}

AMScanController *VESPERSXRFScanConfiguration::createController()
{
	return new VESPERSXRFScanController(this);
}

QString VESPERSXRFScanConfiguration::detailedDescription() const
{
	if (!xrfDetectorInfo_.name().isEmpty())
		return QString("XRF Free Run Scan\nDetector: %1\nReal time: %2 s").arg(xrfDetectorInfo_.name()).arg(xrfDetectorInfo_.integrationTime());

	return QString();
}
