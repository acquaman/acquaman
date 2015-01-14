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


#ifndef AMSCANACTIONCONTROLLERBASICFILEWRITER_H
#define AMSCANACTIONCONTROLLERBASICFILEWRITER_H

#include <QString>
#include <QObject>
#include <QTimer>

class QFile;

/// This class is used for basic file writing needs of a scan controller.
class AMScanActionControllerBasicFileWriter : public QObject
{
	Q_OBJECT

public:
	/// Enum for the different possible errors the file writer can have.
	enum FileWriterError{

		AlreadyExistsError = 0,		///< The file passed in already exists
		CouldNotOpenError = 1,		///< The file could not be opened
		FailedToWriteFile = 2,      ///< Failed to write to the file
		InvalidError = 3			///< Catch all error.
	};

	/// Constructor.  Requires the file path for where the files should be stored.  \param filePath should have everything for the data file except the extension.  Also needs to be told whether it needs to open a separate file for spectral data.
	AMScanActionControllerBasicFileWriter(const QString &filePath, bool hasSpectraData = false, QObject *parent = 0);
	/// Destructor.
	virtual ~AMScanActionControllerBasicFileWriter();

public slots:
	/// Writes the given string to a file.  You need to provide either 0 or 1 for the file rank (scaler data or spectral data).
	virtual void writeToFile(int fileRank, const QString &textToWrite);
	/// Must be called after the scan has completed.  Does the necessary clean up of the files.
	virtual void finishWriting();

signals:
	/// Notifier that there was a write error.  Passes the error.
	void fileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);
	/// Notifier on whether the file writer is busy or not.
	void fileWriterIsBusy(bool isBusy);

protected slots:
	/// Handles the timeout signale of dataFileSizeCheckTimer to make sure the file size is growning
	void onDataFileSizeCheckTimerTimeout();
	/// Handles all the work for emitting the given error.
	void emitError(AMScanActionControllerBasicFileWriter::FileWriterError error);
	/// Handles emitting all the errors that have been accumulated so far.
	void emitErrors();
	/// Handles emitting changes in the file writer busy state.
	void emitFileWriterIsBusy();

protected:
	/// Holds the file path.
	QString filePath_;
	/// Flag holding whether the file writer needs to worry about a second file for spectral data.
	bool hasSpectraData_;

	/// The file for the main data file.
	QFile *dataFile_;
	/// The file for the spectral data file.
	QFile *spectraFile_;

	/// The time to check whether we are writing to data file continuously
	QTimer *dataFileSizeCheckTimer_;
	/// The size of the data file we checked last time
	int dataFileSizeCheckPoint_;
	/// The size of the spectra file we checked last time
	int spectraFileSizeCheckPoint_;
	/// Flag to make sure that when we check for the rank 0 file sizes at least one write has occured in the interim
	bool writeExecutedRank0_;
	/// Flag to make sure that when we check for the rank 1 file sizes at least one write has occured in the interim
	bool writeExecutedRank1_;

	/// A list of errors.
	QList<AMScanActionControllerBasicFileWriter::FileWriterError> errorsList_;
};

#endif // AMSCANACTIONCONTROLLERBASICFILEWRITER_H
