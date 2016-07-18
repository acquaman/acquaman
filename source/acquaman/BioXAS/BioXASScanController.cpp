#include "BioXASScanController.h"

#include "beamline/AMDetector.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASScanController::BioXASScanController()
{
	scanInstance_ = 0;
}

void BioXASScanController::setScan(AMScan * scan)
{
	scanInstance_ = scan;
}

bool BioXASScanController::usingDetector(AMDetector *detector) const
{
	bool result = false;

	if (scanInstance_ && detector)
		result = (scanInstance_->usingDataSource(detector->name()));

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

	if (scanInstance_ && detector)
		result = scanInstance_->dataSourceAt(scanInstance_->indexOfDataSource(detector->name()));

	return result;
}
