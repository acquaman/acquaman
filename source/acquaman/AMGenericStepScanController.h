#ifndef AMGENERICSTEPSCANCONTROLLER_H
#define AMGENERICSTEPSCANCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/AMGenericStepScanConfiguration.h"

/// Builds and runs a controller based on the information given in the configuration.
class AMGenericStepScanController : public AMStepScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a scan controller using the exposed controls and detectors used inside the configuration.
	AMGenericStepScanController(AMGenericStepScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMGenericStepScanController();

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();
	/// Creates the axis order for higher dimensional scans.  Changed order to axis 1, 0.
	virtual void createAxisOrderMap();

	/// Specific scan configuration with all the VESPERS specific information inside.
	AMGenericStepScanConfiguration *configuration_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // AMGENERICSTEPSCANCONTROLLER_H
