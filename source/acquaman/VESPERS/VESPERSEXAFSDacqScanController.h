/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSEXAFSDACQSCANCONTROLLER_H
#define VESPERSEXAFSDACQSCANCONTROLLER_H

//#include "acquaman/AMDacqScanController.h"
//#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"
//#include "acquaman/VESPERS/VESPERSScanController.h"
//#include "dataman/AMXASScan.h"

//#include <QTimer>

///// Some defined error codes to help with controller crashes.
//#define VESPERSEXAFSDACQSCANCONTROLLER_CANT_INTIALIZE 78001
//#define VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_BL_SCANNING 78002
//#define VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 78003
//#define VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 78004

//class VESPERSEXAFSDacqScanController : public AMDacqScanController, public VESPERSScanController
//{
//	Q_OBJECT

//public:
//	/// Constructor.
//	/// \param cfg is the XAS configuration that the controller will run.
//	VESPERSEXAFSDacqScanController(VESPERSEXAFSScanConfiguration *cfg, QObject *parent = 0);

//protected slots:
//	/// Slot that handles the successful initialization of the scan.
//	void onInitializationActionsSucceeded();
//	/// Slot that handles the failed initialization of the scan.
//	void onInitializationActionsFailed(int explanation);
//	/// Slot that handles the initialization progress of the scan.
//	void onInitializationActionsProgress(double elapsed, double total);

//	/// Slot that catches when the cleanup actions are finished.
//	void onCleanupFinished();

//	/// Re-implementing to change actual dwell times for the VESPERS Beamline
//	void onDwellTimeTriggerChanged(double newValue);
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

//	AMnDIndex toScanIndex(QMap<int, double> aeData);

//	/// Adds all the data sources that are still important but not visualized.
//	void addExtraDatasources();

//	/// Sets up the XAS scan based on no fluorescence detectors selected.
//	bool setupTransmissionXAS();
//	/// Sets up the XAS scan based on the single element vortex detector being selected.
//	bool setupSingleElementXAS();
//	/// Sets up the XAS scan based on the four element vortex detector being selected.
//	bool setupFourElementXAS();
//	/// Sets up the XAS scan based on the single element and four element detector being selected.
//	bool setupSingleAndFourElementXAS();

//	/// Pointer to the configuration used by this controller.
//	VESPERSEXAFSScanConfiguration *config_;

//	/// A counter holding the current region index being scanned.
//	int currentRegionIndex_;

//	/// Timer used for determining the elapsed time for a scan.
//	QTimer elapsedTime_;
//	/// Number of seconds since the timer started.
//	double secondsElapsed_;
//	/// Number of seconds total for the scan to complete (estimate).
//	double secondsTotal_;
//};

#endif // VESPERSEXAFSDACQSCANCONTROLLER_H
