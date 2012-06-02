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


#ifndef ACQMAN_SGMXASDACQSCANCONTROLLER_H
#define ACQMAN_SGMXASDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "SGMXASScanController.h"

#define SGMXASDACQSCANCONTROLLER_CANT_INTIALIZE 27001
#define SGMXASDACQSCANCONTROLLER_CANT_START_BL_SCANNING 27002
#define SGMXASDACQSCANCONTROLLER_CANT_START_NO_TEMPLATE_SAVE_PATH 27003
#define SGMXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 27004
#define SGMXASDACQSCANCONTROLLER_CANT_START_CANT_WRITE_TEMPLATE 27005

class SGMXASDacqScanController : public AMDacqScanController, public SGMXASScanController
{
Q_OBJECT
public:
	explicit SGMXASDacqScanController(SGMXASScanConfiguration *cfg, QObject *parent = 0);

protected:
	bool initializeImplementation();
	bool startImplementation();
	void cancelImplementation();
	AMnDIndex toScanIndex(QMap<int, double> aeData);

protected slots:
	// Re-implementing to intercept finished() signal and do cleanup
	void onDacqStop();

	// Re-implementing to change actual dwell times for the SGM Beamline
	void onDwellTimeTriggerChanged(double newValue);

	void onInitializationActionsSucceeded();
	void onInitializationActionsFailed(int explanation);
	void onInitializationActionsProgress(double elapsed, double total);

	void onScanFinished();

	void onScanCancelledBeforeInitialized();
	void onScanCancelledWhileRunning();
};

#endif // ACQMAN_SGMXASDACQSCANCONTROLLER_H
