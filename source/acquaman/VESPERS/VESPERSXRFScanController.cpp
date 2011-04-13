#include "VESPERSXRFScanController.h"
#include "dataman/AMUser.h"
#include "dataman/AMRawDataSource.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERSXRFScanController::VESPERSXRFScanController(VESPERSXRFScanConfiguration *scanConfig, QObject *parent)
	: AMScanController(scanConfig, parent)
{
	detector_ = scanConfig->detector();
	scan_ = new AMXRFScan;
	generalScan_ = scan_;
	scan_->setName("VESPERS XRF Free run Scan");
	scan_->setFilePath("/home/Desktop");
	scan_->setFileFormat("vespersXRF");
	scan_->setRunId(AMUser::user()->currentRunId());

	for (int i = 0; i < detector_->elements(); i++){

		scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("Element %1").arg(i+1), QString("Element %1").arg(i+1), "Counts", detector_->axes()));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i));
	}

	/// \todo Add analyzed data sources.
}

void VESPERSXRFScanController::onProgressUpdate()
{
	emit progress(detector_->elapsedTime(), detector_->integrationTime());
}

void VESPERSXRFScanController::onStatusChanged()
{
	if (!detector_->status())
		onDetectorAcquisitionFinished();
}

void VESPERSXRFScanController::startImplementation()
{
	connect(detector_, SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));
	connect(detector_->elapsedTimeControl(), SIGNAL(valueChanged(double)), this, SLOT(onProgressUpdate()));
	detector_->start();

	setStarted();
}

void VESPERSXRFScanController::onDetectorAcquisitionUpdate()
{
	for (int i = 0; i < detector_->elements(); i++)
		scan_->rawData()->setValue(AMnDIndex(), i, detector_->spectraAt(i), detector_->channels());
}

void VESPERSXRFScanController::onDetectorAcquisitionFinished()
{
	disconnect(detector_, SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));
	disconnect(detector_->elapsedTimeControl(), SIGNAL(valueChanged(double)), this, SLOT(onProgressUpdate()));

	onDetectorAcquisitionUpdate();

	setFinished();
}
