/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef VESPERS2DSCANACTIONCONTROLLER_H
#define VESPERS2DSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/VESPERS/VESPERS2DScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSScanController.h"

/// VESPERS specific 2D scan action controller.
class VESPERS2DScanActionController : public AMStepScanActionController, public VESPERSScanController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the VESPERS beamline.
	VESPERS2DScanActionController(VESPERS2DScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERS2DScanActionController(){}

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Reimplementned to provide actions that will setup the beamline for optimized operation of the 2D scan.
	AMAction3 *createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3 *createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();
	/// Creates the axis order for higher dimensional scans.  Changed order to axis 1, 0.
	virtual void createAxisOrderMap();

	/// Specific scan configuration with all the VESPERS specific information inside.
	VESPERS2DScanConfiguration *configuration_;

	/// Timer used for determining the elasped time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // VESPERS2DSCANACTIONCONTROLLER_H
