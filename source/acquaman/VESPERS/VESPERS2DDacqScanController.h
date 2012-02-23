#ifndef VESPERS2DDACQSCANCONTROLLER_H
#define VESPERS2DDACQSCANCONTROLLER_H

#include "acquaman/AM2DDacqScanController.h"
#include "acquaman/VESPERS/VESPERS2DScanConfiguration.h"
#include "dataman/AM2DScan.h"
#include "actions/AMBeamlineListAction.h"

#include <QTimer>

/// Some defined error codes to help with controller crashes.
#define VESPERS2DDACQSCANCONTROLLER_CANT_INTIALIZE 79001
#define VESPERS2DDACQSCANCONTROLLER_CANT_START_BL_SCANNING 79002
#define VESPERS2DDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 79003
#define VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 79004

class VESPERS2DDacqScanController : public AM2DDacqScanController
{
	Q_OBJECT

public:
	/// Constructor.  \param cfg is the 2D scan configuration that the controller will run.
	VESPERS2DDacqScanController(VESPERS2DScanConfiguration *cfg, QObject *parent = 0);
	/// Destructor.  Makes sure all the memory from the actions that were created is freed.
	~VESPERS2DDacqScanController() { onInitializationActionFinished(); onCleanupActionFinished(); }

	/// Returns the number of values in the first independent axis.
	virtual int xAxisCount() const { return xAxisCount_; }
	/// Returns the number of values in the second independent axis.
	virtual int yAxisCount() const { return yAxisCount_; }

	/// Returns the PV name that will be used for the x-axis.
	virtual QString xAxisPVName() const { return xAxisPVName_; }
	/// Returns the PV name that will be used for the y-axis.
	virtual QString yAxisPVName() const { return yAxisPVName_; }

protected slots:
	/// Slot that handles the successful initialization of the scan.
	void onInitializationActionsSucceeded();
	/// Slot that handles the failed initialization of the scan.
	void onInitializationActionsFailed(int explanation);
	/// Slot that handles the initialization progress of the scan.
	void onInitializationActionsProgress(double elapsed, double total);

	/// Slot that catches when the cleanup actions are finished.
	void onCleanupFinished();

	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

	/// Re-implementing to castrate the function.  I don't want any dacq progress updates because they are much less accurate.
	virtual void onDacqSendCompletion(int completion) { Q_UNUSED(completion) }

protected:
	/// Specific implementation of the scan initialization.
	bool initializeImplementation();
	/// Specific implmentation of the scan start.
	bool startImplementation();
	/// Specific implementation of the scan cancel.
	void cancelImplementation() { AMDacqScanController::cancelImplementation(); cleanup(); }

	/// Re-implementing to intercept finished() signal and do cleanup
	void onDacqStop() { cleanup(); }
	/// Method that cleans up the beamline after a scan is finished.  Makes a list of clean up actions and executes them.
	void cleanup();

	/// Helper method that removes and deletes all of the actions from initialization action for proper memory management.
	void onInitializationActionFinished();
	/// Helper method that removes and deletes all of the actions from the cleanup action for proper memory management.
	void onCleanupActionFinished();

	/// Adds all the data sources that are still important but not visualized.
	void addExtraDatasources();

	/// Returns the home directory for Acquaman.
	QString getHomeDirectory();

	/// Pointer to the VESPERS2DScanConfiguration this scan controls.
	VESPERS2DScanConfiguration *config_;

	/// Holds the x-axis PV name.
	QString xAxisPVName_;
	/// Holds the y-axis PV name.
	QString yAxisPVName_;
	/// Holds the x-axis count.
	int xAxisCount_;
	/// Holds the y-axis count.
	int yAxisCount_;

	/// Action that contains all of the initialization actions for the controller.
	AMBeamlineListAction *initializationActions_;
	/// Action that contains all of the cleanup actions for the controller.
	AMBeamlineListAction *cleanupActions_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // VESPERS2DDACQSCANCONTROLLER_H
