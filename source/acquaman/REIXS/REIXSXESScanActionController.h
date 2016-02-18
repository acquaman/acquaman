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

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class QTimer;

class AMListAction3;
class REIXSXESScanConfiguration;

#include "acquaman/REIXS/REIXSScanActionControllerMCPFileWriter.h"
Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define REIXSXESSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 267003
#define REIXSXESSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 267004
#define REIXSXESSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 267005
#define REIXSXESSCANACTIONCONTROLLER_FAILED_TO_WRITE_FILE 267006

class REIXSXESScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	virtual ~REIXSXESScanActionController();
	REIXSXESScanActionController(REIXSXESScanConfiguration* configuration, QObject *parent = 0);

signals:
	/// Notifier that new information/data should be written to file.
	void requestWriteToFile(int fileRank, const QString &textToWrite);
//	/// Notifier that tells the file writer that all file writing activities are done after a scan has finished and to close all file access.
//	void finishWritingToFile();

public slots:
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

	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);
//	/// Handles dealing with the file writer when it changes busy state.
//	void onFileWriterIsBusy(bool isBusy);

	/// Method that writes out the header information into the scanning file.
	void writeHeaderToFile();
	/// Method that writes out the current data to the scanning file.
	void writeDataToFiles();
	/// Method that handles cleanup for file writing.
	void onScanControllerFinished();

protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();

	virtual bool initializeImplementation();
	virtual void initializePositions();
	virtual bool startImplementation();
	virtual void cancelImplementation();
	/// The implementation for skipping XES scans.
	virtual void stopImplementation(const QString &command);

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation();

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation();

	/// XES Scans can be paused.
	virtual bool canPause() const {return true;}
	/// XES Scans can be skipped.
	virtual bool canSkip() const { return true; }



	AMAction3* createInitializationActions();

	REIXSXESScanConfiguration *configuration_;
	QTimer *updateTimer_;
	AMListAction3 *xesActionsInitializationList_;
	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;

	/// Holds the header string so that we don't have to recreate it everytime data is updated.
	QString headerText_;
};

#endif // REIXSXESSCANACTIONCONTROLLER_H
