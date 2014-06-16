#include "IDEASXRFScanConfiguration.h"

#include "acquaman/IDEAS/IDEASXRFScanController.h"

IDEASXRFScanConfiguration::IDEASXRFScanConfiguration(AMDetectorInfo detectorInfo, QObject *parent) :
	AMScanConfiguration(parent)
{
	xrfDetectorInfo_ = detectorInfo;
	setAutoExportEnabled(false);
	scanName_ = "XRF Scan";
	scanNumber_ = 0;
	scanNotes_ = "";
	positions_.clear();

}

IDEASXRFScanConfiguration::IDEASXRFScanConfiguration(QObject *parent)
		: AMScanConfiguration(parent)
{
		setAutoExportEnabled(false);
}

IDEASXRFScanConfiguration::~IDEASXRFScanConfiguration()
{

}

AMScanConfiguration *IDEASXRFScanConfiguration::createCopy() const
{
		return new IDEASXRFScanConfiguration(*this);
}

AMScanController *IDEASXRFScanConfiguration::createController()
{
		return new IDEASXRFScanController(this);
}

QString IDEASXRFScanConfiguration::detailedDescription() const
{
		if (!xrfDetectorInfo_.name().isEmpty())
				return QString("XRF Scan\nDetector: %1").arg(xrfDetectorInfo_.name());

		return QString();
}
