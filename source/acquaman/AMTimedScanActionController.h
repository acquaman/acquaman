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


#ifndef AMTIMEDSCANACTIONCONTROLLER_H
#define AMTIMEDSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "dataman/AMUser.h"
#include "acquaman/AMTimedRegionScanConfiguration.h"

#include <QThread>
#include <QTimer>

class AMScanActionControllerScanAssembler;
class AMListAction3;

#include "acquaman/AMScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define AMTIMEDSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 266003
#define AMTIMEDSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 266004
#define AMTIMEDSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 266005
#define AMTIMEDSCANACTIONCONTROLLER_FAILED_TO_WRITE_FILE 266006
#define AMTIMESCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR 266007

/// This class does most of the work of building a simple scan controller that uses time for the independent axis.
class AMTimedScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMTimedRegionScanConfiguration.
	AMTimedScanActionController(AMTimedRegionScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTimedScanActionController();

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController();

public slots:

signals:
	/// Notifier that new information/data should be written to file.
	void requestWriteToFile(int fileRank, const QString &textToWrite);
	/// Notifier that tells the file writer that all file writing activities are done after a scan has finished and to close all file access.
	void finishWritingToFile();

protected slots:
	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);
	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting itself.
	void flushCDFDataStoreToDisk();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);

	/// Reimplemented for EXAFS capabilities.  Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

	/// Method that writes out the header information into the scanning file.
	void writeHeaderToFile();
	/// Method that writes out the current data to the scanning file.
	void writeDataToFiles();

	/// The pointer to the region scan configuration to provide the subclass information.
	AMTimedRegionScanConfiguration *timedRegionsConfiguration_;
	/// The current index new data will be inserted into.
	AMnDIndex currentAxisValueIndex_;
	/// The current axis value.
	double currentAxisValue_;
	/// The time that is used to fill up the currentAxisValue_.
	QTime scanElapsedTime_;
	/// The assembler that takes in the region scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *scanAssembler_;
};

#endif // AMTIMEDSCANACTIONCONTROLLER_H
