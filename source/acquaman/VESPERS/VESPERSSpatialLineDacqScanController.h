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


#ifndef VESPERSSPATIALLINEDACQSCANCONTROLLER_H
#define VESPERSSPATIALLINEDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "acquaman/VESPERS/VESPERSScanController.h"
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
class VESPERSSpatialLineDacqScanController : public AMDacqScanController, public VESPERSScanController
{
	Q_OBJECT

public:
	/// Constructor.  \param cfg is the spatial line scan configuration that the controller will be built around.
	VESPERSSpatialLineDacqScanController(VESPERSSpatialLineScanConfiguration *cfg, QObject *parent = 0);
	/// Destructor.  Makes sure all the memory fromm the actiosn that were created are cleaned up.
	~VESPERSSpatialLineDacqScanController() { }

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

	/// Adds all the data sources that are still important but not visualized.
	void addExtraDatasources();

	/// Sets up the line scan with no XRF detector.
	bool setupIonChamberMap();
	/// Sets up the line scan based on the single element detector being used for XRF.
	bool setupSingleElementMap();
	/// Sets up the line scan based on the four element detector being used for XRF.
	bool setupFourElementMap();
	/// Sets up the line scan based on the single element and four element detectors being used for XRF.
	bool setupSingleAndFourElementMap();

	/// Pointer to the VESPERSSpatialLineScanConfiguration this scan controls.
	VESPERSSpatialLineScanConfiguration *config_;

	/// Holds the PV name.
	QString pvName_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // VESPERSSPATIALLINEDACQSCANCONTROLLER_H
