#ifndef VESPERSSPATIALLINEDACQSCANCONTROLLER_H
#define VESPERSSPATIALLINEDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h"
#include "dataman/AMScan.h"
#include "actions/AMBeamlineListAction.h"

#include <QTimer>

/// Some defined error codes to help with controller crashes.
#define VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_INTIALIZE 75001
#define VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_START_BL_SCANNING 75002
#define VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 75003
#define VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 75004

/// This class builds a scan controller for doing a spatial line scan.
class VESPERSSpatialLineDacqScanController : public AMDacqScanController
{
	Q_OBJECT

public:
	/// Constructor.  \param cfg is the spatial line scan configuration that the controller will be built around.
	VESPERSSpatialLineDacqScanController(VESPERSSpatialLineScanConfiguration *cfg, QObject *parent = 0);
	/// Destructor.  Makes sure all the memory fromm the actiosn that were created are cleaned up.
	~VESPERSSpatialLineDacqScanController() { onInitializationActionFinished(); onCleanupActionFinished(); }

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

	/// Sets up the line scan based on the single element detector being used for XRF.
	bool setupSingleElementMap();
	/// Sets up the line scan based on the four element detector being used for XRF.
	bool setupFourElementMap();

	/// Pointer to the VESPERSSpatialLineScanConfiguration this scan controls.
	VESPERSSpatialLineScanConfiguration *config_;

	/// Holds the PV name.
	QString pvName_;

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

#endif // VESPERSSPATIALLINEDACQSCANCONTROLLER_H
