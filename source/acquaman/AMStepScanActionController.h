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


#ifndef AMSTEPSCANACTIONCONTROLLER_H
#define AMSTEPSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/AMStepScanConfiguration.h"
#include "dataman/AMUser.h"

#include <QThread>
#include <QTimer>

class AMScanActionControllerScanAssembler;
class AMListAction3;

#include "acquaman/AMScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define AMSTEPSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR 265000
#define AMSTEPSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 265003
#define AMSTEPSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 265004
#define AMSTEPSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 265005
#define AMSTEPSCANACTIONCONTROLLER_FAILE_TO_WRITE_FILE 265006

/// This class is the base class for all step based scan controllers.  It can handle any number of dimensions as long as they are all step axes.
class AMStepScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMStepScanConfiguration.
	AMStepScanActionController(AMStepScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMStepScanActionController();

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController();

signals:
	/// Notifier that new information/data should be written to file.
	void requestWriteToFile(int fileRank, const QString &textToWrite);

protected slots:
	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);

	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting itself.
	void flushCDFDataStoreToDisk();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);
	/// Handles stopping the current scan action and setting up the necessary communications to call setFinished when appropriate.
	/*!
		Accepts the following stop commands.

		- Stop Immediately
		- Stop At End Of Line
	  */
	virtual void stopImplementation(const QString &command);
	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();
	/// Creates the axis order for higher dimensional scans.  The default order is the axis order, but if a different order is desired, then you should reimplement this in subclasses.
	/*!
		Be sure to use scan_->rawData() for access to the scan axes and not the configuration, as they haven't been properly configured by the time this has been called.
	  */
	virtual void createAxisOrderMap();

	/// Method that writes out the header information into the scanning file.
	void writeHeaderToFile();
	/// Method that writes out the current data to the scanning file.
	void writeDataToFiles();

	/// Method that prefills the scan's raw data store for 2D and higher dimensionality scans.
	virtual void prefillScanPoints();

	/// The assembler that takes in the region scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *scanAssembler_;
	/// The pointer to the region scan configuration to provide the subclass information.
	AMStepScanConfiguration *stepConfiguration_;

	/// Flag for determining whether axis value comes from setpoint or feedback.  Should be set by subclasses.  Only valid for 1D scans.
	bool useFeedback_;

	/// The current index new data will be inserted into.
	AMnDIndex currentAxisValueIndex_;
	/// The current value for all axes.
	QVector<double> currentAxisValues_;
	/// The names of the axis controls.
	QStringList axisNames_;
	/// The axis order mapping.  The mapping is <scan axis name, desired index>.  As with any list, the index should be between 0 and scanRank()-1.
	QMap<QString, int> axisOrderMap_;
	/// Flag for when we are stopping at the end of the the line.
	bool stoppingAtEndOfLine_;

	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;

	/// Axis stack counter. Keeps track of how many axes are currently running.
	int axisStackCounter_;
};

#endif // AMSTEPSCANACTIONCONTROLLER_H
