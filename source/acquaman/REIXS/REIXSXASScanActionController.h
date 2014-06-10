#ifndef REIXSXASSCANACTIONCONTROLLER_H
#define REIXSXASSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class REIXSXASScanActionController : public AMRegionScanActionController
{
Q_OBJECT
public:
 	virtual ~REIXSXASScanActionController();
	REIXSXASScanActionController(REIXSXASScanConfiguration *cfg, QObject *parent = 0);
	//bool canSkip() const { return false; }


protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

	void onInitializationActionSucceeded();
protected:
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	QTimer *updateTimer_;
	AMListAction3 *xesActionsInitializationList_;
	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;

protected:
	REIXSXASScanConfiguration *configuration_;
};

#endif // REIXSXASSCANACTIONCONTROLLER_H
