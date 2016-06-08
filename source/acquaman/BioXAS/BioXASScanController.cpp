#include "BioXASScanController.h"

#include "beamline/AMDetector.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASScanController::BioXASScanController()
{
}

void BioXASScanController::setGeneralScanControllerScan(AMScan * scan)
{
	generalScan_ = scan;
}

bool BioXASScanController::usingDetector(AMDetector *detector) const
{
	bool result = false;

	if (generalScan_ && detector)
		result = (generalScan_->usingDataSource(detector->name()));

	return result;
}

bool BioXASScanController::usingScaler() const
{
	bool result = false;

	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();
	if (scaler)
		result = (   usingDetector(BioXASBeamline::bioXAS()->i0Detector())
				  || usingDetector(BioXASBeamline::bioXAS()->i1Detector())
				  || usingDetector(BioXASBeamline::bioXAS()->i2Detector()) );

	return result;
}

AMDataSource* BioXASScanController::detectorDataSource(AMDetector *detector) const
{
	AMDataSource *result = 0;

	if (generalScan_ && detector)
		result = generalScan_->dataSourceAt(generalScan_->indexOfDataSource(detector->name()));

	return result;
}
