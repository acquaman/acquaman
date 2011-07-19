#ifndef VESPERSXASDACQSCANCONTROLLER_H
#define VESPERSXASDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "acquaman/VESPERS/VESPERSXASScanConfiguration.h"
#include "dataman/AMXASScan.h"

/// Some defined error codes to help with controller crashes.
#define VESPERSXASDACQSCANCONTROLLER_CANT_INTIALIZE 77001
#define VESPERSXASDACQSCANCONTROLLER_CANT_START_BL_SCANNING 77002
#define VESPERSXASDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 77003
#define VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 77004

class VESPERSXASDacqScanController : public AMDacqScanController
{
	Q_OBJECT

public:
	/// Constructor.
	/// \param cfg is the XAS configuration that the controller will run.
	VESPERSXASDacqScanController(VESPERSXASScanConfiguration *cfg, QObject *parent = 0);

	/// Returns the scan that this controller is scanning.
	virtual AMScan *scan() { return xasScan_; }

protected slots:
	/// Slot that handles the successful initialization of the scan.
	void onInitializationActionsSucceeded();
	/// Slot that handles the failed initialization of the scan.
	void onInitializationActionsFailed(int explanation);
	/// Slot that handles the initialization progress of the scan.
	void onInitializationActionsProgress(double elapsed, double total);

protected:
	/// Specific implementation of the scan initialization.
	bool initializeImplementation();
	/// Specific implmentation of the scan start.
	bool startImplementation();

	AMnDIndex toScanIndex(QMap<int, double> aeData);

	/// Sets up the XAS scan based on no fluorescence detectors selected.
	bool setupTransmissionXAS();
	/// Sets up the XAS scan based on the single element vortex detector being selected.
	bool setupSingleElementXAS();
	/// Sets up the XAS scan based on the four element vortex detector being selected.
	bool setupFourElementXAS();

	/// Returns the home directory for Acquaman.
	QString getHomeDirectory();

	/// Pointer to the configuration used by this controller.
	VESPERSXASScanConfiguration *config_;
	/// Pointer to the scan used by this controller.
	AMXASScan *xasScan_;

};

#endif // VESPERSXASDACQSCANCONTROLLER_H
