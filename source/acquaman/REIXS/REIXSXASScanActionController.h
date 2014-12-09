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


#ifndef REIXSXASSCANACTIONCONTROLLER_H
#define REIXSXASSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class REIXSXASScanActionController : public AMStepScanActionController
{
Q_OBJECT
public:
	REIXSXASScanActionController(REIXSXASScanConfiguration *cfg, QObject *parent = 0);
	virtual ~REIXSXASScanActionController();


protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

	void onInitializationActionSucceeded();
protected:
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();
	/// Re-implemented since the MCP is unique.
	virtual void cancelImplementation();


	QTimer *updateTimer_;
	AMListAction3 *xesActionsInitializationList_;
	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;

protected:
	REIXSXASScanConfiguration *configuration_;
};

#endif // REIXSXASSCANACTIONCONTROLLER_H
