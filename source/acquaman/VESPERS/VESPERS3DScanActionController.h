#ifndef VESPERS3DSCANACTIONCONTROLLER_H
#define VESPERS3DSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/VESPERS/VESPERS3DScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSScanController.h"

/// VESPERS specific 3D scan action controller.
class VESPERS3DScanActionController : public AMStepScanActionController, public VESPERSScanController
{
	Q_OBJECT

public:
	/// Constructor.  Builds
	VESPERS3DScanActionController(VESPERS3DScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERS3DScanActionController(){}

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the 3D scan.
	AMAction3 *createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3 *createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();
	/// Creates the axis order for higher dimensional scans.  Changed order to 3, 1, 2.
	virtual void createAxisOrderMap();

	/// Specific scan configuration with all the VESPERS specific inside.
	VESPERS3DScanConfiguration *configuration_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of secondss total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // VESPERS3DSCANACTIONCONTROLLER_H
