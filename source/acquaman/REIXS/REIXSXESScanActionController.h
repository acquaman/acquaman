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


#ifndef REIXSXESSCANACTIONCONTROLLER_H
#define REIXSXESSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class QTimer;

class AMListAction3;
class REIXSXESScanConfiguration;

class REIXSXESScanActionController : public AMScanActionController
{
Q_OBJECT
public:
 	virtual ~REIXSXESScanActionController();
	REIXSXESScanActionController(REIXSXESScanConfiguration* configuration, QObject *parent = 0);


public slots:
	virtual void skip(const QString &command);

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController();

protected slots:
	void onDetectorAcquisitionSucceeded();
	void onDetectorAcquisitionFailed();

	/// Called when the data values of the detector image change (during a scan)
	void onNewImageValues();
	/// Called to save the current raw data to disk
	void saveRawData();

	/// Fills the scan meta-data (scan name, number, sampleId) either automatically, or from the pre-set configuration values.
	void initializeScanMetaData();
	void onInitializationActionsListSucceeded();
	void onInitializationActionsListFailed();


	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();



protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();

	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();


	AMAction3* createInitializationActions();


protected:
	REIXSXESScanConfiguration *configuration_;
	QTimer *updateTimer_;
	AMListAction3 *xesActionsInitializationList_;
	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;



};

#endif // REIXSXESSCANACTIONCONTROLLER_H
