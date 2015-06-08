#include "BioXASXRFDetectorView.h"

#include "acquaman/BioXAS/BioXASXRFScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"

BioXASXRFDetectorView::BioXASXRFDetectorView(AMXspress3XRFDetector *detector, QWidget *parent)
	: AMXspress3XRFDetectorView(detector, parent)
{
}

void BioXASXRFDetectorView::startAcquisition()
{
	BioXASXRFScanConfiguration *configuration = new BioXASXRFScanConfiguration;
	AMDetectorInfoSet detectorSet;
	detectorSet.addDetectorInfo(detector_->toInfo());
	configuration->setDetectorConfigurations(detectorSet);
	AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));

	connect(scanAction, SIGNAL(cancelled()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(failed()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(succeeded()), scanAction, SLOT(scheduleForDeletion()));

	scanAction->start();
}
