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

bool BioXASScanController::usingI0Detector() const
{
	return usingDetector(BioXASBeamline::bioXAS()->i0Detector());
}

bool BioXASScanController::usingI1Detector()  const
{
	return usingDetector(BioXASBeamline::bioXAS()->i1Detector());
}

bool BioXASScanController::usingI2Detector() const
{
	return usingDetector(BioXASBeamline::bioXAS()->i2Detector());
}

bool BioXASScanController::usingGeDetector(BioXAS32ElementGeDetector *detector) const
{
	return usingDetector(detector);
}

bool BioXASScanController::usingScaler() const
{
	bool result = false;

	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();
	if (scaler)
		result = ( usingI0Detector() || usingI1Detector() || usingI2Detector() );

	return result;
}

AMDataSource* BioXASScanController::detectorDataSource(AMDetector *detector) const
{
	AMDataSource *result = 0;

	if (generalScan_ && detector)
		result = generalScan_->dataSourceAt(generalScan_->indexOfDataSource(detector->name()));

	return result;
}

AMDataSource* BioXASScanController::i0DetectorDataSource() const
{
	return detectorDataSource(BioXASBeamline::bioXAS()->i0Detector());
}

AMDataSource* BioXASScanController::i1DetectorDataSource() const
{
	return detectorDataSource(BioXASBeamline::bioXAS()->i1Detector());
}

AMDataSource* BioXASScanController::i2DetectorDataSource() const
{
	return detectorDataSource(BioXASBeamline::bioXAS()->i2Detector());
}

AMDataSource* BioXASScanController::diodeDetectorDataSource() const
{
	return detectorDataSource(BioXASBeamline::bioXAS()->diodeDetector());
}

AMDataSource* BioXASScanController::pipsDetectorDataSource() const
{
	return detectorDataSource(BioXASBeamline::bioXAS()->pipsDetector());
}

AMDataSource* BioXASScanController::lytleDetectorDataSource() const
{
	return detectorDataSource(BioXASBeamline::bioXAS()->lytleDetector());
}

AMDataSource* BioXASScanController::scalerDwellTimeDetectorDataSource() const
{
	return detectorDataSource(BioXASBeamline::bioXAS()->scalerDwellTimeDetector());
}

AMDataSource* BioXASScanController::geDetectorDataSource(BioXAS32ElementGeDetector *detector) const
{
	return detectorDataSource(detector);
}
