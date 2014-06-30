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


#ifndef AM2DSCANACTIONCONTROLLER_H
#define AM2DSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "dataman/AMUser.h"
#include "acquaman/AM2DScanConfiguration.h"

#include <QThread>
#include <QTimer>

class AMScanActionControllerScanAssembler;
class AMListAction3;

#include "acquaman/AMScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define AM2DSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION 264002
#define AM2DSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 264003
#define AM2DSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 264004
#define AM2DSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 264005

/// This class does most of the work of setting up a simple scan controller using the 2D scan configuration.
class AM2DScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor. Takes in an AM2DScanConfiguration.
	AM2DScanActionController(AM2DScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AM2DScanActionController();

	/// Returns whether it is safe to be deleted.  This is important due to the multithreaded nature of the file writer.
	virtual bool isReadyForDeletion() const;
	/// Method that builds all the genearl aspects, sucah as measurements and raw data sources, and the file writer capabilities of the controller.
	virtual void buildScanController();

signals:
	/// Notifier that new infomration/data should be written to file.
	void requestWriteToFile(int fileRank, const QString &textToWrite);
	/// Notifier that tells the file writer that all file writing activities are done after a scan has finished.
	void finishWritingToFile();

protected slots:
	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);
	/// Handles dealing with the file writer when it changes busy state.
	void onFileWriterIsBusy(bool isBusy);
	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting the data files.
	void flushCDFDataStoreToDisk();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);

	/// Method that writes out the header information into the scanning file.
	void writeHeaderToFile();
	/// Method that writes out the current data to the scanning file.
	void writeDataToFiles();

	/// Method that fleshes out the scan's raw data store with all of the points it will need.
	void prefillScanPoints();

	/// The assembler that takes the scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *newScanAssembler_;
	/// The pointer to the 2D scan configuration to provide the subclass information.
	AM2DScanConfiguration *configuration2D_;

	/// The current index new data will be inserted into.
	AMnDIndex currentAxisValueIndex_;
	/// The name of the x-axis control.
	QString xAxisName_;
	/// The name of the y-axis control.
	QString yAxisName_;
	/// The current x-axis value.
	double currentXAxisValue_;
	/// The current y-axis value.
	double currentYAxisValue_;

	/// Pointer to the thread that handles all the file writing.
	QThread *fileWriterThread_;
	/// Flag for keeping track of whether the file writer thread is busy or not.
	bool fileWriterIsBusy_;
	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;
};

#endif // AM2DSCANACTIONCONTROLLER_H
