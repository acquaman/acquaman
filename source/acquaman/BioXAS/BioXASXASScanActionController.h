#ifndef BIOXASXASSCANACTIONCONTROLLER_H
#define BIOXASXASSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"

#define BIOXASXASSCANACTIONCONTROLLER_DETECTOR_NOT_FOUND 6638201
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

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Provides details for the beamline current settings.
	QString beamlineSettings();

	/// Reimplemented to provide actions that will setup the beamine for optimized operation of the XAS scan.
	virtual AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	virtual AMAction3* createCleanupActions();

	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();
	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

protected:
	/// The general BioXAS XAS scan configuration.
	BioXASXASScanConfiguration *configuration_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // BIOXASXASSCANACTIONCONTROLLER_H
