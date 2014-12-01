#include "VESPERSXRFDetailedDetectorView.h"

#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"

VESPERSXRFDetailedDetectorView::VESPERSXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
}

void VESPERSXRFDetailedDetectorView::startAcquisition()
{
	VESPERSXRFScanConfiguration *configuration = new VESPERSXRFScanConfiguration;
	AMDetectorInfoSet detectorSet;
	detectorSet.addDetectorInfo(detector_->toInfo());
	configuration->setDetectorConfigurations(detectorSet);
	AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));

	connect(scanAction, SIGNAL(cancelled()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(failed()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(succeeded()), scanAction, SLOT(scheduleForDeletion()));

	scanAction->start();
}
