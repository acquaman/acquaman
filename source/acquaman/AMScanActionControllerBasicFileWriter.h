#ifndef AMSCANACTIONCONTROLLERBASICFILEWRITER_H
#define AMSCANACTIONCONTROLLERBASICFILEWRITER_H

#include <QString>
#include <QObject>

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
		InvalidError = 2			///< Catch all error.
	};

	/// Constructor.  Requires the file path for where the files should be stored.  \param filePath should have everything for the data file except the extension.  Also needs to be told whether it needs to open a separate file for spectral data.
	AMScanActionControllerBasicFileWriter(const QString &filePath, bool hasSpectraData = false, QObject *parent = 0);
	virtual ~AMScanActionControllerBasicFileWriter();

public slots:
	/// Writes the given string to a file.  You need to provide either 0 or 1 for the file rank (scaler data or spectral data).
	void writeToFile(int fileRank, const QString &textToWrite);
	/// Must be called after the scan has completed.  Does the necessary clean up of the files.
	void finishWriting();

signals:
	/// Notifier that there was a write error.  Passes the error.
	void fileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);
	/// Notifier on whether the file writer is busy or not.
	void fileWriterIsBusy(bool isBusy);

protected slots:
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

	/// A list of errors.
	QList<AMScanActionControllerBasicFileWriter::FileWriterError> errorsList_;
};

#endif // AMSCANACTIONCONTROLLERBASICFILEWRITER_H
