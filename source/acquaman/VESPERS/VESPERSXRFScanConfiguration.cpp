#include "VESPERSXRFScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"
#include "util/VESPERS/XRFPeriodicTable.h"
#include "util/AMPeriodicTable.h"

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(XRFDetector detector, QObject *parent)
	: AMScanConfiguration(parent)
{
	detector_ = detector;
	xrfDetectorInfo_ = detector_->toXRFInfo();

	XRFPeriodicTable *xrfTable = new XRFPeriodicTable(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().second.toDouble(), detector_->maximumEnergy(), this);

	connect(xrfTable, SIGNAL(addedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(addRegionOfInterest(XRFElement*,QString)));
	connect(xrfTable, SIGNAL(removedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(removeRegionOfInterest(XRFElement*,QString)));
}

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
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
