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


#ifndef VESPERS2DDACQSCANCONTROLLER_H
#define VESPERS2DDACQSCANCONTROLLER_H

#include "acquaman/AM2DDacqScanController.h"
#include "acquaman/VESPERS/VESPERSScanController.h"
#include "acquaman/VESPERS/VESPERS2DScanConfiguration.h"
#include "dataman/AM2DScan.h"
#include "actions/AMBeamlineListAction.h"
#include "application/VESPERS/VESPERS.h"

#include <QTimer>

/// Some defined error codes to help with controller crashes.
#define VESPERS2DDACQSCANCONTROLLER_CANT_INTIALIZE 79001
#define VESPERS2DDACQSCANCONTROLLER_CANT_START_BL_SCANNING 79002
#define VESPERS2DDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 79003
#define VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 79004

/// This class builds a scan controller for doing a 2D map.
class VESPERS2DDacqScanController : public AM2DDacqScanController, public VESPERSScanController
{
	Q_OBJECT

public:
	/// Constructor.  \param cfg is the 2D scan configuration that the controller will run.
	VESPERS2DDacqScanController(VESPERS2DScanConfiguration *cfg, QObject *parent = 0);
	/// Destructor.  Makes sure all the memory from the actions that were created is freed.
	~VESPERS2DDacqScanController() { onInitializationActionFinished(); onCleanupActionFinished(); }

	/// Returns the PV name that will be used for the x-axis.
	virtual QString xAxisPVName() const { return xAxisPVName_; }
	/// Returns the PV name that will be used for the y-axis.
	virtual QString yAxisPVName() const { return yAxisPVName_; }

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

	/// Sets up the 2D scan based on the single element detector being used for XRF.
	bool setupSingleElementMap();
	/// Sets up the 2D scan based on the four element detector being used for XRF.
	bool setupFourElementMap();
	/// Sets up the 2D scan based on the single element and four element detectors being used for XRF.
	bool setupSingleAndFourElementMap();

	/// Pointer to the VESPERS2DScanConfiguration this scan controls.
	VESPERS2DScanConfiguration *config_;

	/// Holds the x-axis PV name.
	QString xAxisPVName_;
	/// Holds the y-axis PV name.
	QString yAxisPVName_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // VESPERS2DDACQSCANCONTROLLER_H
