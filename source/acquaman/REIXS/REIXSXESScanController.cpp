#include "REIXSXESScanController.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "util/AMErrorMonitor.h"
#include "dataman/AMUser.h"

#include "dataman/REIXS/REIXSXESCalibration.h"
#include "beamline/AMBeamlineControlSetMoveAction.h"
#include "util/AMSettings.h"

REIXSXESScanController::REIXSXESScanController(REIXSXESScanConfiguration* configuration, QObject *parent) :
	AMScanController(configuration, parent)
{

	initialMoveAction_ = new AMBeamlineControlSetMoveAction(REIXSBeamline::bl()->spectrometerPositionSet(), this);

	/// \todo could this all be done at the abstract level, once we go to a single scan class?  ie: within AMScanController::initialize() ?

	/// \todo Would it be okay for controllers to create their scans in initialize()? Saves memory because every scan action in the queue doesn't need to create its scan object (and associated memory) until actually running.  Currently doesn't work because AMBeamlineScanAction calls AMScanControllerSupervisor::setCurrentScanController() before calling AMScanController::initialize().

	/////////////////////////

	// create our new scan object
	generalScan_ = new AMXESScan();
	generalScan_->setName("REIXS XES Scan");
	/// \todo These never get set: generalScan_->setFilePath(generalCfg_->filePath() + generalCfg_->fileName());
	// instead:
	generalScan_->setFilePath(AMUserSettings::defaultFilePath(QDateTime::currentDateTime()));
	generalScan_->setFileFormat("reixsXESRaw");
	generalScan_->setRunId(AMUser::user()->currentRunId());

	// set the scan configuration within the scan:
	pScan()->setScanConfiguration(generalCfg_);
	///////////////////////////

	// resolve any potential differences between the configured detector info, and the size of the actual detector. (Cumbersome, I know... The detector size seems configurable but isn't; you have to use the actual size of the real detector.)

	/// \todo Check at this point that we have the actual size for the real detector reporting already.
	// AMMeasurementInfo realDetector = *(REIXSBeamline::bl()->mcpDetector()); // REIXSXESMCPDetector has cast operator to AMDetectorInfo, which has cast operator to AMMeasurementInfo.
	AMMeasurementInfo configuredDetector = *(pCfg()->mcpDetectorInfo());
	configuredDetector.axes[0].size = REIXSBeamline::bl()->mcpDetector()->image()->size(0);
	configuredDetector.axes[1].size = REIXSBeamline::bl()->mcpDetector()->image()->size(1);
	configuredDetector.name = "xesImage";	// necessary for file loader/saver.

	pScan()->rawData()->addMeasurement(configuredDetector);
	pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), 0));

}



/// Called before starting to satisfy any prerequisites (ie: setting up the beamline, setting up files, etc.)
void REIXSXESScanController::initialize() {



	// configure and clear the MCP detector
	if( !REIXSBeamline::bl()->mcpDetector()->setFromInfo(*(pCfg()->mcpDetectorInfo())) ) {
		/// \todo How to notify init failed?
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 3, "Could not connect to and configure the MCP detector before starting XES scan."));
		emit cancelled();
		return;
	}



	// Do we actually need to move into position?
	//if(pCfg()->shouldStartFromCurrentPosition()) {
	if(true) {
		onInitialSetupMoveSucceeded();
		return;
	}

	else {
		// compute the values to move to.
		REIXSXESCalibration calibration;
		calibration.loadFromDb(AMDatabase::userdb(), pCfg()->spectrometerCalibrationId());
		AMControlInfoList moveValues = calibration.computeSpectrometerPosition(pCfg());

		// move into position. connect signal from move done to onInitialSetupMoveSucceeded(), and move failed to onInitialSetupMoveFailed().

		if(initialMoveAction_->isRunning())
			initialMoveAction_->cancel();
		if(!initialMoveAction_->setSetpoint(&moveValues)) {
			/// \todo How to notify init failed?
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 3, "Could not start moving the spectrometer into position; maybe the move positions were out of range?"));
			emit cancelled();
			return;
		}

		connect(initialMoveAction_, SIGNAL(succeeded()), this, SLOT(onInitialSetupMoveSucceeded()));
		connect(initialMoveAction_, SIGNAL(failed(int)), this, SLOT(onInitialSetupMoveFailed()));
		initialMoveAction_->start();

		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Moving spectrometer into position before starting the scan..."));

	}

}


void REIXSXESScanController::onInitialSetupMoveFailed() {
	/// \todo How to notify init failed?
	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 3, "Could not move the spectrometer into position before starting a scan."));
	disconnect(initialMoveAction_, 0, this, 0);
	emit cancelled();
}

void REIXSXESScanController::onInitialSetupMoveSucceeded() {
	// remember the positions of the spectrometer
	*(pScan()->scanInitialConditions()) = REIXSBeamline::bl()->spectrometerPositionSet()->toInfoList();
	// emit initialized... Tells everyone we're ready to go.
	initialized_ = true;
	disconnect(initialMoveAction_, 0, this, 0);
	emit initialized();
}

void REIXSXESScanController::start() {

	/// \todo Is this check required? Will anything ever try to call this
	if(!initialized_) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -1, "Attempted start on uninitialized controller."));
		/// \todo how to notify init failed?
		emit cancelled();
	}

	REIXSBeamline::bl()->mcpDetector()->clearImage();

	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	connect(&scanProgressTimer_, SIGNAL(timeout()), this, SLOT(onScanProgressCheck()));

	startTime_.start();
	scanProgressTimer_.start(1000);
	running_ = true;
	emit started();
}

/// Cancel scan if currently running or paused
void REIXSXESScanController::cancel() {

	if(running_) {
		scanProgressTimer_.stop();
		disconnect(&scanProgressTimer_, SIGNAL(timeout()), this, SLOT(onScanProgressCheck()));
		disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	}
	running_ = false;
	emit cancelled();
}

void REIXSXESScanController::onNewImageValues() {

	QVector<int> imageData = REIXSBeamline::bl()->mcpDetector()->imageData();

	if(!pScan()->rawData()->setValue(AMnDIndex(), 0, imageData.constData(), imageData.size()))
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 37, "Error setting the new values from the MCP Detector. The size of the image didn't match what it should be.  This is probably a problem with the network connection to the detector, or a bug in the detector driver."));

}

#include "dataman/REIXS/REIXSXESRawFileLoader.h"

void REIXSXESScanController::onScanProgressCheck() {

	/// \todo This will not work for scans longer than 24 hours.
	double secondsElapsed = startTime_.elapsed()/1000;

	if(secondsElapsed > pCfg()->maximumDurationSeconds()) {
		onScanFinished();
		return;
	}

	/// \bug What if this occurs before the detector is done clearing itself? could be spurious?
	int totalCounts = REIXSBeamline::bl()->mcpDetector()->totalCounts();

	if(totalCounts > pCfg()->maximumTotalCounts()) {
		onScanFinished();
		return;
	}

	double timeProgress = secondsElapsed / pCfg()->maximumDurationSeconds();
	double countsProgress = totalCounts / pCfg()->maximumTotalCounts();

	// emit based on whichever looks more promising to happen first...
	if(countsProgress > timeProgress) {	// more likely to hit max counts first
		emit progress(totalCounts, pCfg()->maximumTotalCounts());
		double timeLeft = (pCfg()->maximumTotalCounts()-totalCounts) * (secondsElapsed/totalCounts);
		emit timeRemaining(timeLeft);
	}
	else {
		emit progress(secondsElapsed, pCfg()->maximumDurationSeconds());
		emit timeRemaining(pCfg()->maximumDurationSeconds() - secondsElapsed);
	}


	// every 5 seconds, save the raw data to disk.
	/// \todo Make this a define adjustable
	if((int)secondsElapsed % 5 == 0) {
		saveRawData();
	}
}


void REIXSXESScanController::saveRawData() {
	REIXSXESRawFileLoader exporter(pScan());
	if(!exporter.saveToFile(pScan()->filePath()))
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 38, "Error saving the currently-running XES scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to the Acquaman developers."));
}

void REIXSXESScanController::onScanFinished() {

	scanProgressTimer_.stop();
	disconnect(&scanProgressTimer_, SIGNAL(timeout()), this, SLOT(onScanProgressCheck()));
	disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	running_ = false;
	saveRawData();
	emit finished();

}

/// to disconnect: scan progress check, onNewImageValues... Where else?
