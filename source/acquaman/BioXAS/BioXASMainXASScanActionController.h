#ifndef BIOXASMAINXASSCANACTIONCONTROLLER_H
#define BIOXASMAINXASSCANACTIONCONTROLLER_H

#include <QObject>

#include "acquaman/AMStepScanActionController.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"

#define BIOXASMAINXASSCANACTIONCONTROLLER_DETECTOR_NOT_FOUND 6638201
#define BIOXASMAINXASSCANACTIONCONTROLLER_SCALER_NOT_FOUND 6638202

class BioXASMainXASScanConfiguration;

class BioXASMainXASScanActionController : public AMStepScanActionController
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainXASScanActionController(BioXASMainXASScanConfiguration *configuration, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainXASScanActionController();

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Provides details for the beamline current settings.
	QString beamlineSettings();

	/// Reimplemented to provide actions that will setup the beamine for optimized operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();
	/// Reimplemented for EXAFS capabilities.  Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

protected:
	/// The BioXAS-specific scan configuration.
    BioXASMainXASScanConfiguration *configuration_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;

};

#endif // BIOXASMAINXASSCANACTIONCONTROLLER_H
