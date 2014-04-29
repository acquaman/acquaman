#ifndef VESPERS2DSCANACTIONCONTROLLER_H
#define VESPERS2DSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/VESPERS/VESPERS2DScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSScanController.h"

/// VESPERS specific 2D scan action controller.
class VESPERS2DScanActionController : public AMStepScanActionController, public VESPERSScanController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the VESPERS beamline.
	VESPERS2DScanActionController(VESPERS2DScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERS2DScanActionController(){}

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Reimplementned to provide actions that will setup the beamline for optimized operation of the 2D scan.
	AMAction3 *createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3 *createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();
	/// Creates the axis order for higher dimensional scans.  The default order is the axis order, but if a different order is desired, then you should reimplement this in subclasses.
	virtual void createAxisOrderMap();

	/// Specific scan configuration with all the VESPERS specific information inside.
	VESPERS2DScanConfiguration *configuration_;

	/// Timer used for determinging the elasped time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // VESPERS2DSCANACTIONCONTROLLER_H
