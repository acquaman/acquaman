#ifndef VESPERSENERGYDACQSCANCONTROLLER_H
#define VESPERSENERGYDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "acquaman/VESPERS/VESPERSEnergyScanConfiguration.h"
#include "dataman/AMXASScan.h"
#include "actions/AMBeamlineListAction.h"

#include <QTimer>

/// Some defined error codes to help with controller crashes.
#define VESPERSENERGYDACQSCANCONTROLLER_CANT_INTIALIZE 85001
#define VESPERSENERGYDACQSCANCONTROLLER_CANT_START_BL_SCANNING 85002
#define VESPERSENERGYDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 85003
#define VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 85004

class VESPERSEnergyDacqScanController : public AMDacqScanController
{
	Q_OBJECT

public:
	/// Constructor.  Passes the configuration that this controller will run.
	VESPERSEnergyDacqScanController(VESPERSEnergyScanConfiguration *cfg, QObject *parent = 0);
	/// Destructor.  Makes sure all of the memory from the actions that were created is freed.
	~VESPERSEnergyDacqScanController() { onInitializationActionFinished(); onCleanupActionFinished(); }

protected slots:
	/// Slot that handles the successful initialization of the scan.
	void onInitializationActionsSucceeded();
	/// Slot that handles the failed initialization of the scan.
	void onInitializationActionsFailed(int explanation);
	/// Slot that handles the initialization progress of the scan.
	void onInitializationActionsProgress(double elapsed, double total);

	/// Slot that catches when the cleanup actions are finished.
	void onCleanupFinished();

	/// Re-implementing to change actual dwell times for the VESPERS Beamline
	void onDwellTimeTriggerChanged(double newValue);
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

	AMnDIndex toScanIndex(QMap<int, double> aeData);

	/// Adds all the data sources that are still important but not visualized.
	void addExtraDatasources();

	/// Sets up the scan to use the Roper CCD.
	bool setupRoperScan();
	/// Sets up the scan to use the Mar CCD.
	bool setupMarScan();

	/// Returns the home directory for Acquaman.
	QString getHomeDirectory();

	/// Pointer to the configuration used by this controller.
	VESPERSEnergyScanConfiguration *config_;

	/// A counter holding the current region index being scanned.
	int currentRegionIndex_;

	/// Action that contains all of the initialization actions for the controller.
	AMBeamlineListAction *setupXASAction_;
	/// Action that contains all of the cleanup actions for the controller.
	AMBeamlineListAction *cleanupXASAction_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // VESPERSENERGYDACQSCANCONTROLLER_H