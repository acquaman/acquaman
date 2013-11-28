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


#ifndef SGMFASTDACQSCANCONTROLLER_H
#define SGMFASTDACQSCANCONTROLLER_H

//#include "acquaman/AMDacqScanController.h"
//#include "SGMFastScanController.h"

//#define SGMFASTDACQSCANCONTROLLER_CANT_INTIALIZE 27401
//#define SGMFASTDACQSCANCONTROLLER_CANT_START_BL_SCANNING 27402
//#define SGMFASTDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 27403
//#define SGMFASTDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 27404

//class SGMFastDacqScanController : public AMDacqScanController, public SGMFastScanController
//{
//	Q_OBJECT
//public:
//	/// Standard constructor, calls parent to set up the controller and sets the scan object given this configuration
//	explicit SGMFastDacqScanController(SGMFastScanConfiguration *cfg, QObject *parent = 0);

//protected slots:
//	/// Re-implementing to intercept finished() signal and do cleanup
//	void onDacqStop();
//	/// Re-implementing to incorporate initialization actions into progress
//	void onDacqSendCompletion(int completion);
//	/// Re-implementing to incorporate initialization actions into progress
//	void onDacqState(const QString &state);

//	/// Calls setInitialized() once the initialization actions have succeeded.
//	void onInitializationActionsSucceeded();
//	/// Causes a calculateProgress() to occur when each initialization action stage is completed.
//	void onInitializationActionsStageSucceeded(int stageIndex);
//	/// Calls the cleanup actions if available and sets the scan controller as cancelled.
//	void onInitializationActionsFailed(int explanation);
//	/// Assists with the timing reports for the fast scans
//	void onFastScanTimerTimeout();

//	/// Calculates the progress for the scan, including the initialization actions
//	void calculateProgress(double elapsed, double total);

//	/// Calls the cleanup actions when finished or calls AMDacqScanController::onDacqStop() if not available.
//	void onScanFinished();
//	/// Calls the cleanup actions when finished or calls AMDacqScanController::onDacqStop() if not available.
//	void onScanCancelledBeforeInitialized();
//	/// Calls the cleanup actions when finished or calls AMDacqScanController::onDacqStop() if not available.
//	void onScanCancelledWhileRunning();

//protected:
//	/// Calls SGMFastScanController's initialization functions and starts the initialization actions. Reports failure if unable to do so.
//	bool initializeImplementation();
//	/// Sets up the dacq configuration file and calls AMDacqScanController::startImplementation.
//	bool startImplementation();
//	/// Cancels either the initialization actions (if initializing) or the scan (using AMDacqScanController::cancelImplementation).
//	void cancelImplementation();

//	/// Does all of the work to parse the full scan data from the scaler buffer. There's a lot of hardcoded information in here.
//	bool event(QEvent *e);
//	/// Simple scan index returns 1D scan size
//	AMnDIndex toScanIndex(QMap<int, double> aeData);

//protected:
//	/// Tracks the progress through the scan
//	double lastProgress_;
//	/// Tracks the number of initialization actions stages that have been finished
//	int initializationStagesComplete_;

//	/// Tracks the different dacq phases
//	bool dacqRunUpStarted_;
//	bool dacqRunUpCompleted_;
//	bool dacqRunCompleted_;

//	/// Helps with reporting the timing for the fast scan
//	QTimer *fastScanTimer_;
//	int timerSeconds_;

//	/// Used to stop the motors is cancel was called after the initalization actions were done but during the fast scan phase.
//	AMBeamlineActionItem *stopMotorsAction_;
//};

#endif // SGMFASTDACQSCANCONTROLLER_H
