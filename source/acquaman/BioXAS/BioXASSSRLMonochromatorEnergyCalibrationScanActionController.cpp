#include "BioXASSSRLMonochromatorEnergyCalibrationScanActionController.h"

BioXASSSRLMonochromatorEnergyCalibrationScanActionController::BioXASSSRLMonochromatorEnergyCalibrationScanActionController(BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *configuration, QObject *parent) :
	BioXASXASScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_->setNotes(beamlineSettings());
}

BioXASSSRLMonochromatorEnergyCalibrationScanActionController::~BioXASSSRLMonochromatorEnergyCalibrationScanActionController()
{

}

QString BioXASSSRLMonochromatorEnergyCalibrationScanActionController::beamlineSettings()
{
	// Note that this is an energy calibration XAS scan.

	QString calibrationNotes = QString("SSRL Monochromator Energy Calibration Scan");

	// Note other BioXAS settings.

	QString generalNotes = BioXASXASScanActionController::beamlineSettings();

	// Create and return complete notes.

	QString notes = calibrationNotes + "\n\n" + generalNotes;
	return notes;
}
