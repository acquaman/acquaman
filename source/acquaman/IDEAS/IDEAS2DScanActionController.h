#ifndef IDEAS2DSCANACTIONCONTROLLER_H
#define IDEAS2DSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/IDEAS/IDEAS2DScanConfiguration.h"

#include <QTimer>

class IDEAS2DScanActionController : public AMStepScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the IDEAS beamline.
	IDEAS2DScanActionController(IDEAS2DScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~IDEAS2DScanActionController();

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();
	/// Reimplemented to populate scan initial conditions
	void onInitializationActionsListSucceeded();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();
	/// Creates the axis order for higher dimensional scans.  The default order is the axis order, but if a different order is desired, then you should reimplement this in subclasses.
	virtual void createAxisOrderMap();

	/// Specific scan configuration with all the SGM specific information inside.
	IDEAS2DScanConfiguration *configuration_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // IDEAS2DSCANACTIONCONTROLLER_H
