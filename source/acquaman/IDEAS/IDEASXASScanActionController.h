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


#ifndef IDEASXASSCANACTIONCONTROLLER_H
#define IDEASXASSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"

#include <QTimer>

class IDEASXASScanActionController : public AMStepScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the IDEAS beamline.
	IDEASXASScanActionController(IDEASXASScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~IDEASXASScanActionController();

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();
	/// Reimplemented to populate scan initial conditions
	void onInitializationActionsListSucceeded();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();
	/// Reimplemented for EXAFS capabilities.  Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

	/// Specific scan configuration with all the SGM specific information inside.
	IDEASXASScanConfiguration *configuration_;

	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;

};

#endif // IDEASXASSCANACTIONCONTROLLER_H
