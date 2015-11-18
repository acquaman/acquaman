#ifndef AMGENERICCONTINUOUSSCANCONTROLLER_H
#define AMGENERICCONTINUOUSSCANCONTROLLER_H

#include "acquaman/AMContinuousScanActionController.h"
#include "acquaman/AMGenericContinuousScanConfiguration.h"

/// Builds and runs a controller based on the information given in the configuration.
class AMGenericContinuousScanController : public AMContinuousScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a scan controller using the exposed controls and detectors used inside the configuration.
	AMGenericContinuousScanController(AMGenericContinuousScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMGenericContinuousScanController();

signals:

public slots:

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	AMAction3 *createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3 *createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();

	/// Specific scan configuration with all the VESPERS specific information inside.
	AMGenericContinuousScanConfiguration *configuration_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // AMGENERICCONTINUOUSSCANCONTROLLER_H
