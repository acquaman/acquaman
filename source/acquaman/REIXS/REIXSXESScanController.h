//WARNING																								WARNING
//WARNING  This file has been depreciated, it has been replace with REIXSXESScanActionController.cpp	WARNING
//WARNING																								WARNING

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


#ifndef REIXSXESSCANCONTROLLER_H
#define REIXSXESSCANCONTROLLER_H

#include "acquaman/AMScanController.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "dataman/AMXESScan.h"
#include <QTimer>
#include <QDateTime>


/// This is the implementation of AMScanController that executes a REIXS XES Scan, based on the parameters in a given REIXSXESScanConfiguration.
class REIXSXESScanController : public AMScanController
{
	Q_OBJECT
public:
 	virtual ~REIXSXESScanController();
	explicit REIXSXESScanController(REIXSXESScanConfiguration* configuration, QObject *parent = 0);

signals:

protected:
	/// Start scan running if not currently running or paused
	virtual bool startImplementation();
	/// Cancel scan if currently running or paused
	virtual void cancelImplementation();
	/// Called before starting to satisfy any prerequisites (ie: setting up the beamline, setting up files, etc.)
	virtual bool initializeImplementation();


	/// Fills the scan meta-data (scan name, number, sampleId) either automatically, or from the pre-set configuration values.
	void initializeScanMetaData();


protected slots:
	/// Called when the move to the starting position succeeds
	void onInitialSetupMoveSucceeded();
	/// Called if the move to the starting position fails.
	void onInitialSetupMoveFailed();

	/// Called when the data values of the detector image change (during a scan)
	void onNewImageValues();

	/// Called at every interval of the scanProgressTimer_ to report on the scan progress and check if we've reached the timeout duration.
	void onScanProgressCheck();
	/// Called to clean up when the scan is finished.
	void onScanFinished();

	/// Called to save the current raw data to disk
	void saveRawData();

protected:
	/// Time that scan was started
	QDateTime startTime_;
	/// 1 second timer to check on progress of scan
	QTimer scanProgressTimer_;


private:
	/// Our scan configuration:
	REIXSXESScanConfiguration* config_;

};

#endif // REIXSXESSCANCONTROLLER_H
