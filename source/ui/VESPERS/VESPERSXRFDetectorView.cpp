#include "VESPERSXRFDetectorView.h"

#include "acquaman/AMScanController.h"

VESPERSXRFDetectorView::VESPERSXRFDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
	scanConfiguration_ = new VESPERSXRFScanConfiguration(this);
	AMDetectorInfoSet detectorInfo;
	detectorInfo.addDetectorInfo(detector->toInfo());
	scanConfiguration_->setDetectorConfigurations(detectorInfo);
}

void VESPERSXRFDetectorView::startAcquisition()
{
	scanAction_ = new AMScanAction(new AMScanActionInfo(scanConfiguration_->createCopy()));
	connect(scanAction_, SIGNAL(succeeded()), this, SLOT(onScanActionFinished()));
	connect(scanAction_, SIGNAL(cancelled()), this, SLOT(onScanActionFinished()));
	connect(scanAction_, SIGNAL(failed()), this, SLOT(onScanActionFinished()));
}

void VESPERSXRFDetectorView::cancelAcquisition()
{
	if (scanAction_)
		scanAction_->skip("Stop Now");
}

void VESPERSXRFDetectorView::onScanActionFinished()
{
	scanAction_->disconnect(this);
	scanAction_->deleteLater();
	scanAction_ = 0;
}
