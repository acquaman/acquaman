#ifndef BIOXASXASSCANACTIONCONTROLLER_H
#define BIOXASXASSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"

#include "application/AMAppControllerSupport.h"
#include "application/BioXAS/BioXAS.h"

#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

#define BIOXASXASSCANACTIONCONTROLLER_VORTEX_DETECTOR_NOT_FOUND 6638201
#define BIOXASXASSCANACTIONCONTROLLER_SCALER_NOT_FOUND 6638202

class BioXASXASScanConfiguration;

class BioXASXASScanActionController : public AMStepScanActionController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanActionController();

public slots:
	/// Sets the axis control infos for the scan configuration.
	void setConfigurationAxisControlInfos(const AMControlInfoList &infos);
	/// Sets the detector configurations for the scan configuration.
	void setConfigurationDetectorConfigurations(const AMDetectorInfoSet &infos);

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Provides details for the beamline current settings.
	virtual QString beamlineSettings();

	/// Returns actions that will initialize one of the BioXAS XAS beamlines for an XAS scan.
	virtual AMAction3* createInitializationActions();
	/// Returns actions that will make final adjustments to the beamline once the XAS scan has finished.
	virtual AMAction3* createCleanupActions();


	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

protected:
	/// The scan configuration.
	BioXASXASScanConfiguration *configuration_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // BIOXASXASSCANACTIONCONTROLLER_H
