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


#ifndef AMREGIONSCANACTIONCONTROLLER_H
#define AMREGIONSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "dataman/AMUser.h"
#include "acquaman/AMRegionScanConfiguration.h"

#include <QThread>

class AMScanActionControllerScanAssembler;
class AMListAction3;

#include "acquaman/AMScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define AMREGIONSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION 263002
#define AMREGIONSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 263003
#define AMREGIONSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 263004
#define AMREGIONSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 263005

/// This class does most of the work of setting up a simple scan controller for scans using the AMRegionScanConfiguration.  Handles building the raw data sources and measurements and handles the file writing.
class AMRegionScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMRegionScanConfiguration.
	AMRegionScanActionController(AMRegionScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMRegionScanActionController();

	/// Returns whether it is safe to be deleted.  This is important due to the multithreaded nature of the file writing.
	virtual bool isReadyForDeletion() const;

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController();

signals:
	/// Notifier that new information/data should be written to file.
	void requestWriteToFile(int fileRank, const QString &textToWrite);
	/// Notifier that tells the file writer that all file writing activities are done after a scan has finished and to close all file access.
	void finishWritingToFile();

protected slots:
	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);
	/// Handles dealing with the file writer when it changes busy state.
	void onFileWriterIsBusy(bool isBusy);
	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting itself.
	void flushCDFDataStoreToDisk();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);

	/// Method that writes out the header information into the scanning file.
	void writeHeaderToFile();
	/// Method that writes out the current data to the scanning file.
	void writeDataToFiles();

	/// The assembler that takes in the region scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *newScanAssembler_;
	/// The pointer to the region scan configuration to provide the subclass information.
	AMRegionScanConfiguration *regionsConfiguration_;

	/// Flag for determining whether axis value comes from setpoint or feedback.  Should be set by subclasses.
	bool useFeedback_;
	/// The current index new data will be inserted into.
	AMnDIndex currentAxisValueIndex_;
	/// The current axis value.
	double currentAxisValue_;

	/// Pointer to the thread that handles all the file writing.
	QThread *fileWriterThread_;
	/// Flag for keeping track of whether the file writer thread is busy or not.
	bool fileWriterIsBusy_;
	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;
};

#endif // AMREGIONSCANACTIONCONTROLLER_H
