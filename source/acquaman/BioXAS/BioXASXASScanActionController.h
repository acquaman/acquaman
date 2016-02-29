#ifndef BIOXASXASSCANACTIONCONTROLLER_H
#define BIOXASXASSCANACTIONCONTROLLER_H

#include "acquaman/AMGenericStepScanController.h"

#include "application/AMAppControllerSupport.h"
#include "application/BioXAS/BioXAS.h"

#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

#define BIOXASXASSCANACTIONCONTROLLER_XRF_DETECTOR_NOT_FOUND 6638201
#define BIOXASXASSCANACTIONCONTROLLER_SCALER_NOT_FOUND 6638202

class BioXASXASScanConfiguration;

class BioXASXASScanActionController : public AMGenericStepScanController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanActionController();

protected:
	/// Provides details for the beamline current settings.
	virtual QString scanNotes();

	/// Returns true if the beamline has a scaler and the scaler will be used in the scan.
	bool usingScaler() const;

	/// Returns true if the beamline has an I0 detector and the I0 detector will be used in the scan.
	bool usingI0Detector() const;
	/// Returns true if the beamline has an I1 detector and the I1 detector will be used in the scan.
	bool usingI1Detector() const;
	/// Returns true if the beamline has an I2 detector and the I2 detector will be used in the scan.
	bool usingI2Detector() const;

	/// Returns true if the beamline has a Zebra and the Zebra will be used in the scan.
	bool usingZebra() const;

	/// Returns true if the beamline has a Ge detector and the Ge detector will be used in the scan.
	bool usingGeDetector() const;

	/// Returns actions that will initialize one of the BioXAS XAS beamlines for an XAS scan.
	virtual AMAction3* createInitializationActions();
	/// Returns actions that will make final adjustments to the beamline once the XAS scan has finished.
	virtual AMAction3* createCleanupActions();

	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

protected:
	/// The BioXAS XAS scan configuration.
	BioXASXASScanConfiguration *bioXASConfiguration_;
};

#endif // BIOXASXASSCANACTIONCONTROLLER_H
