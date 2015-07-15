#ifndef BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANACTIONCONTROLLER_H
#define BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANACTIONCONTROLLER_H

#include "acquaman/BioXAS/BioXASXASScanActionController.h"
#include "acquaman/BioXAS/BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration.h"

class BioXASSSRLMonochromatorEnergyCalibrationScanActionController : public BioXASXASScanActionController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyCalibrationScanActionController(BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyCalibrationScanActionController();

protected:
	/// Returns a string representation of beamline settings to note.
	virtual QString beamlineSettings();

protected:
	/// The BioXAS-specific scan configuration.
	BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *configuration_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANACTIONCONTROLLER_H
