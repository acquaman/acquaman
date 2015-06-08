#ifndef BIOXASJJSLITSSCANCONTROLLER_H
#define BIOXASJJSLITSSCANCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/BioXAS/BioXASJJSlitsScanConfiguration.h"

class BioXASJJSlitsScanController : public AMStepScanActionController
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASJJSlitsScanController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASJJSlitsScanController();

protected slots:
	/// Handles the scan progress updates.
	void onScanTimerUpdate();

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();
	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

protected:
	/// The scan configuration.
	BioXASJJSlitsScanController *configuration_;
	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // BIOXASJJSLITSSCANCONTROLLER_H
