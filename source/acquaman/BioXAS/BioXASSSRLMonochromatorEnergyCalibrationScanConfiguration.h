#ifndef BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANCONFIGURATION_H
#define BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANCONFIGURATION_H

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

class BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration : public BioXASXASScanConfiguration
{
    Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS SSRL Monochromator Energy Calibration XAS Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration(const BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration();

	/// Returns a description of this scan configuration.
	virtual QString description() const { return "SSRL Mono Energy Calibration XAS Scan"; }
	/// Returns a detailed description of this scan configuration.
	virtual QString detailedDescription() const { return "BioXAS SSRL Monochromator Energy Calibration XAS Scan"; }

	/// Returns a pointer to a newly-created copy of this scan configuration.
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.
	virtual AMScanController* createController();
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANCONFIGURATION_H
