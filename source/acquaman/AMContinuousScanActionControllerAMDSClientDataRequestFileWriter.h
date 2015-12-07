#ifndef AMCONTINUOUSSCANACTIONCONTROLLERAMDSCLIENTDATAREQUESTFILEWRITER_H
#define AMCONTINUOUSSCANACTIONCONTROLLERAMDSCLIENTDATAREQUESTFILEWRITER_H

#include <QString>
#include <QObject>
#include <QMap>

#include "acquaman/AMScanActionControllerBasicFileWriter.h"
#include "acquaman.h"

class QFile;
class AMDSClientDataRequest;


class AMContinuousScanActionControllerAMDSClientDataRequestFileWriter : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Requires the file path for where the files should be stored.  \param filePath should have everything for the data file except the extension.
	AMContinuousScanActionControllerAMDSClientDataRequestFileWriter(const QString &filePath, QObject *parent = 0);

	/// Destructor.
	virtual ~AMContinuousScanActionControllerAMDSClientDataRequestFileWriter();

public slots:
	/// Writes the data requests to file. Takes ownership of them at this point and must delete them when it's done
	virtual void writeToFile(const AMDSClientDataRequestMap &clientDataRequestMap);
	/// Must be called after the scan has completed.  Does the necessary clean up of the files.
	virtual void finishWriting();

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

	/// The file for the main data file.
	QFile *clientDataRequestFile_;

	/// A list of errors.
	QList<AMScanActionControllerBasicFileWriter::FileWriterError> errorsList_;
};

#endif // AMCONTINUOUSSCANACTIONCONTROLLERAMDSCLIENTDATAREQUESTFILEWRITER_H
