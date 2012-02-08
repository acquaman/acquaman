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
#include "actions2/actions/AMInternalControlMoveAction.h"
#include "util/AMSettings.h"

#include "analysis/AM2DSummingAB.h"

REIXSXESScanController::REIXSXESScanController(REIXSXESScanConfiguration* configuration, QObject *parent) :
	AMScanController(configuration, parent)
{
	config_ = configuration;
	initialMoveAction_ = new AMInternalControlMoveAction(REIXSBeamline::bl()->spectrometer(), 0, this);

	/////////////////////////

	// create our new scan object
	scan_ = new AMXESScan();
	scan_->setName("REIXS XES Scan");

	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".img");
	scan_->setFileFormat("reixsXESRaw");
	scan_->setRunId(AMUser::user()->currentRunId());

	// set the scan configuration within the scan:
	scan_->setScanConfiguration(config_);
	///////////////////////////

	// resolve any potential differences between the configured detector info, and the size of the actual detector. (Cumbersome, I know... The detector size seems configurable but isn't; you have to use the actual size of the real detector.)

	/// \todo Check at this point that we have the actual size for the real detector reporting already.
	// AMMeasurementInfo realDetector = *(REIXSBeamline::bl()->mcpDetector()); // REIXSXESMCPDetector has cast operator to AMDetectorInfo, which has cast operator to AMMeasurementInfo.
	AMMeasurementInfo configuredDetector = *(config_->mcpDetectorInfo());
	configuredDetector.axes[0].size = REIXSBeamline::bl()->mcpDetector()->image()->size(0);
	configuredDetector.axes[1].size = REIXSBeamline::bl()->mcpDetector()->image()->size(1);
	configuredDetector.name = "xesImage";	// necessary for file loader/saver.

	scan_->rawData()->addMeasurement(configuredDetector);
	AMRawDataSource* imageDataSource = new AMRawDataSource(scan_->rawData(), 0);
	scan_->addRawDataSource(imageDataSource);

	AM2DSummingAB* xesSpectrum = new AM2DSummingAB("Spectrum");
	xesSpectrum->setInputDataSources(QList<AMDataSource*>() << imageDataSource);
	xesSpectrum->setSumAxis(1);
	xesSpectrum->setSumRangeMax(imageDataSource->size(1)-1);
	scan_->addAnalyzedDataSource(xesSpectrum);
}



/// Called before starting to satisfy any prerequisites (ie: setting up the beamline, setting up files, etc.)
bool REIXSXESScanController::initializeImplementation() {

	// Is the detector connected?
	if(!REIXSBeamline::bl()->mcpDetector()->canRead() || !REIXSBeamline::bl()->mcpDetector()->canConfigure()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 17, "Could not connect to the MCP detector before starting an XES scan. Please report this problem to the beamline staff."));
		return false;
	}


	// configure and clear the MCP detector.
	/// \todo We should really configure the detector even if we're not supposed to clear it, but right now setting the orientation clears the accumulated counts.  We'll be removing that orientation setting soon, since we no longer use it. Therefore, we should be OK to skip this if we're not supposed to clear.
	if(!config_->doNotClearExistingCounts()) {
		if( !REIXSBeamline::bl()->mcpDetector()->setFromInfo(*(config_->mcpDetectorInfo())) ) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 3, "Could not connect to and configure the MCP detector before starting an XES scan. Please report this problem to the beamline staff."));
			return false;
		}
	}



	// Do we actually need to move into position?
	if(config_->shouldStartFromCurrentPosition()) {
		onInitialSetupMoveSucceeded();
		return true;
	}

	else {

		if(!REIXSBeamline::bl()->spectrometer()->loadSpectrometerCalibration(AMDatabase::database("user"), config_->spectrometerCalibrationId())) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 10, QString("Could not load the spectrometer calibration (%1) that was specified in this scan configuration.").arg(config_->spectrometerCalibrationId())));
			return false;
		}

		if(!REIXSBeamline::bl()->spectrometer()->specifyGrating(config_->gratingNumber())) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 11, "There was no grating like the one specified in this scan configuration."));
			return false;
		}

		REIXSBeamline::bl()->spectrometer()->specifyDetectorTiltOffset(config_->detectorTiltOffset());
		REIXSBeamline::bl()->spectrometer()->specifyFocusOffset(config_->defocusDistanceMm());

		//if(initialMoveAction_->isRunning())
		//	initialMoveAction_->cancel();

		if(!initialMoveAction_->setSetpoint(config_->centerEV())) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 12, "Could not start moving the spectrometer into position."));
			return false;
		}

		connect(initialMoveAction_, SIGNAL(succeeded()), this, SLOT(onInitialSetupMoveSucceeded()));
		connect(initialMoveAction_, SIGNAL(failed()), this, SLOT(onInitialSetupMoveFailed()));
		initialMoveAction_->start();

		// AMErrorMon::report(AMErrorReport(this, AMErrorReport::Information, 0, "Moving spectrometer into position before starting the scan..."));
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
	*(scan_->scanInitialConditions()) = REIXSBeamline::bl()->spectrometerPositionSet()->toInfoList();

	disconnect(initialMoveAction_, 0, this, 0);

	// tell the controller API we're now ready to go.
	setInitialized();
}

bool REIXSXESScanController::startImplementation() {

	if(!config_->doNotClearExistingCounts()) {
		REIXSBeamline::bl()->mcpDetector()->clearImage();
	}

	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	connect(&scanProgressTimer_, SIGNAL(timeout()), this, SLOT(onScanProgressCheck()));

	startTime_ = QDateTime::currentDateTime();
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

	if(!scan_->rawData()->setValue(AMnDIndex(), 0, imageData.constData(), imageData.size()))
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 37, "Error setting the new values from the MCP Detector. The size of the image didn't match what it should be.  This is probably a problem with the network connection to the detector, or a bug in the detector driver."));

}

#include "dataman/REIXS/REIXSXESRawFileLoader.h"

void REIXSXESScanController::onScanProgressCheck() {

	int secondsElapsed = startTime_.secsTo(QDateTime::currentDateTime());

	if(secondsElapsed > config_->maximumDurationSeconds()) {
		onScanFinished();
		return;
	}

	int totalCounts = int(REIXSBeamline::bl()->mcpDetector()->totalCounts());

	// Check if total counts is reached.
	// problem: What if this occurs before the detector is done clearing itself? could be spurious?  To solve this problem simply, just make sure we collect at least 5 seconds regardless of the total counts.
	if(secondsElapsed > 5 && totalCounts > config_->maximumTotalCounts()) {
		onScanFinished();
		return;
	}

	double timeProgress = double(secondsElapsed) / config_->maximumDurationSeconds();
	double countsProgress = double(totalCounts) / config_->maximumTotalCounts();

	// emit based on whichever looks more promising to happen first...
	if(countsProgress > timeProgress) {	// more likely to hit max counts first
		emit progress(totalCounts, config_->maximumTotalCounts());
		double timeLeft = (config_->maximumTotalCounts()-totalCounts) * (double(secondsElapsed)/totalCounts);
		emit timeRemaining(timeLeft);
	}
	else {
		emit progress(secondsElapsed, config_->maximumDurationSeconds());
		emit timeRemaining(config_->maximumDurationSeconds() - secondsElapsed);
	}


	// every 5 seconds, save the raw data to disk.
	/// \todo Make this a define adjustable
	if(secondsElapsed % 5 == 0) {
		saveRawData();
	}
}


void REIXSXESScanController::saveRawData() {
	REIXSXESRawFileLoader exporter(qobject_cast<AMXESScan*>(scan_));
	if(!exporter.saveToFile(AMUserSettings::userDataFolder + "/" + scan_->filePath()))
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
