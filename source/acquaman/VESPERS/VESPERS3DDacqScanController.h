#ifndef VESPERS3DDACQSCANCONTROLLER_H
#define VESPERS3DDACQSCANCONTROLLER_H

//#include "acquaman/AM3DDacqScanController.h"
//#include "acquaman/VESPERS/VESPERSScanController.h"
//#include "acquaman/VESPERS/VESPERS3DScanConfiguration.h"
//#include "dataman/AM3DScan.h"
//#include "application/VESPERS/VESPERS.h"

//#include <QTimer>

///// Some defined error codes to help with controller crashes.
//#define VESPERS3DDACQSCANCONTROLLER_CANT_INTIALIZE 79101
//#define VESPERS3DDACQSCANCONTROLLER_CANT_START_BL_SCANNING 79102
//#define VESPERS3DDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 79103
//#define VESPERS3DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 79104
//#define VESPERS3DDACQSCANCONTROLLER_NO_CCD_SELECTED 79105

///// This class builds a scan controller for doing a 3D map.
//class VESPERS3DDacqScanController : public AM3DDacqScanController, public VESPERSScanController
//{
//	Q_OBJECT

//public:
//	/// Constructor.  \param config is the 3D scan configuration that thee controller will run.
//	VESPERS3DDacqScanController(VESPERS3DScanConfiguration *config, QObject *parent = 0);
//	/// Destructor.  Makes sure all the memory from the actions that were created is freed.
//	~VESPERS3DDacqScanController() {  }

//	/// Returns the PV name that will be used for the x-axis.
//	virtual QString xAxisPVName() const { return xAxisPVName_; }
//	/// Returns the PV name that will be used for the y-axis.
//	virtual QString yAxisPVName() const { return yAxisPVName_; }
//	/// Returns the PV name that will be used for the z-axis.
//	virtual QString zAxisPVName() const { return zAxisPVName_; }

//protected slots:
//	/// Slot that handles the successful initialization of the scan.
//	void onInitializationActionsSucceeded();
//	/// Slot that handles the failed initialization of the scan.
//	void onInitializationActionsFailed(int explanation);
//	/// Slot that handles the initialization progress of the scan.
//	void onInitializationActionsProgress(double elapsed, double total);

//	/// Slot that catches when the cleanup actions are finished.
//	void onCleanupFinished();

//	/// Helper slot that handles the progress update.
//	void onScanTimerUpdate();

//	/// Re-implementing to castrate the function.  I don't want any dacq progress updates because they are much less accurate.
//	virtual void onDacqSendCompletion(int completion) { Q_UNUSED(completion) }

//protected:
//	/// Specific implementation of the scan initialization.
//	bool initializeImplementation();
//	/// Specific implmentation of the scan start.
//	bool startImplementation();
//	/// Specific implementation of the scan cancel.
//	void cancelImplementation() { AMDacqScanController::cancelImplementation(); cleanup(); }

//	/// Re-implementing to intercept finished() signal and do cleanup
//	void onDacqStop() { cleanup(); }
//	/// Method that cleans up the beamline after a scan is finished.  Makes a list of clean up actions and executes them.
//	void cleanup();

//	/// Adds all the data sources that are still important but not visualized.
//	void addExtraDatasources();

//	/// Sets up the 3D scan based on the single element detector being used for XRF.
//	bool setupSingleElementMap();
//	/// Sets up the 3D scan based on the four element detector being used for XRF.
//	bool setupFourElementMap();
//	/// Sets up the 3D scan based on the single element and four element detectors being used for XRF.
//	bool setupSingleAndFourElementMap();

//	/// Pointer to the VESPERS3DScanConfiguration this scan controls.
//	VESPERS3DScanConfiguration *config_;

//	/// Holds the x-axis PV name.
//	QString xAxisPVName_;
//	/// Holds the y-axis PV name.
//	QString yAxisPVName_;
//	/// Holds the z-axis PV name.
//	QString zAxisPVName_;

//	/// Timer used for determining the elapsed time for a scan.
//	QTimer elapsedTime_;
//	/// Number of seconds since the timer started.
//	double secondsElapsed_;
//	/// Number of seconds total for the scan to complete (estimate).
//	double secondsTotal_;
//};

#endif // VESPERS3DDACQSCANCONTROLLER_H
