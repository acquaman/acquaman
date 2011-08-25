/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "REIXSXESScanController.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "util/AMErrorMonitor.h"
#include "dataman/AMUser.h"

#include "dataman/REIXS/REIXSXESCalibration.h"
#include "beamline/AMBeamlineControlMoveAction.h"
#include "util/AMSettings.h"

REIXSXESScanController::REIXSXESScanController(REIXSXESScanConfiguration* configuration, QObject *parent) :
	AMScanController(configuration, parent)
{

	initialMoveAction_ = new AMBeamlineControlMoveAction(REIXSBeamline::bl()->spectrometer(), this);

	/////////////////////////

	// create our new scan object
	generalScan_ = new AMXESScan();
	generalScan_->setName("REIXS XES Scan");

	generalScan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".img");
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
bool REIXSXESScanController::initializeImplementation() {



	// configure and clear the MCP detector
	if( !REIXSBeamline::bl()->mcpDetector()->setFromInfo(*(pCfg()->mcpDetectorInfo())) ) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 3, "Could not connect to and configure the MCP detector before starting XES scan."));
		return false;
	}



	// Do we actually need to move into position?
	if(pCfg()->shouldStartFromCurrentPosition()) {
		onInitialSetupMoveSucceeded();
		return true;
	}

	else {

		if(!REIXSBeamline::bl()->spectrometer()->loadSpectrometerCalibration(AMDatabase::userdb(), pCfg()->spectrometerCalibrationId())) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 10, QString("Could not load the spectrometer calibration (%1) that was specified in this scan configuration.").arg(pCfg()->spectrometerCalibrationId())));
			return false;
		}

		if(!REIXSBeamline::bl()->spectrometer()->specifyGrating(pCfg()->gratingNumber())) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 11, "There was no grating like the one specified in this scan configuration."));
			return false;
		}

		REIXSBeamline::bl()->spectrometer()->specifyDetectorTiltOffset(pCfg()->detectorTiltOffset());
		REIXSBeamline::bl()->spectrometer()->specifyFocusOffset(pCfg()->defocusDistanceMm());

		//if(initialMoveAction_->isRunning())
		//	initialMoveAction_->cancel();

		if(!initialMoveAction_->setSetpoint(pCfg()->centerEV())) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 12, "Could not start moving the spectrometer into position."));
			return false;
		}

		connect(initialMoveAction_, SIGNAL(succeeded()), this, SLOT(onInitialSetupMoveSucceeded()));
		connect(initialMoveAction_, SIGNAL(failed(int)), this, SLOT(onInitialSetupMoveFailed()));
		initialMoveAction_->start();

		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Information, 0, "Moving spectrometer into position before starting the scan..."));
		return true;
	}
}


void REIXSXESScanController::onInitialSetupMoveFailed() {
	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 13, "Could not move the spectrometer into position before starting a scan."));
	disconnect(initialMoveAction_, 0, this, 0);
	setFailed();
}

void REIXSXESScanController::onInitialSetupMoveSucceeded() {
	// remember the positions of the spectrometer
	*(pScan()->scanInitialConditions()) = REIXSBeamline::bl()->spectrometerPositionSet()->toInfoList();

	disconnect(initialMoveAction_, 0, this, 0);

	// tell the controller API we're now ready to go.
	setInitialized();
}

bool REIXSXESScanController::startImplementation() {

	REIXSBeamline::bl()->mcpDetector()->clearImage();

	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	connect(&scanProgressTimer_, SIGNAL(timeout()), this, SLOT(onScanProgressCheck()));

	startTime_.start();
	scanProgressTimer_.start(1000);

	setStarted();
	return true;
}

/// Cancel scan if currently running or paused
void REIXSXESScanController::cancelImplementation() {

	ScanState currentState = state();

	if(currentState == Running || currentState == Paused) {
		scanProgressTimer_.stop();
		disconnect(&scanProgressTimer_, SIGNAL(timeout()), this, SLOT(onScanProgressCheck()));
		disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	}
	setCancelled();
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
	if(!exporter.saveToFile(AMUserSettings::userDataFolder + "/" + pScan()->filePath()))
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 38, "Error saving the currently-running XES scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to the Acquaman developers."));
}

void REIXSXESScanController::onScanFinished() {

	scanProgressTimer_.stop();
	disconnect(&scanProgressTimer_, SIGNAL(timeout()), this, SLOT(onScanProgressCheck()));
	disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	saveRawData();
	setFinished();
}

/// to disconnect: scan progress check, onNewImageValues... Where else?
