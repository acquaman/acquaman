#ifndef SGMXASSCANACTIONCONTROLLER_H
#define SGMXASSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

#include <QThread>

class AMScanActionControllerScanAssembler;
class QFile;

class SGMXASScanActionControllerFileWriter : public QObject
{
Q_OBJECT
public:
	enum FileWriterError{
		AlreadyExistsError = 0,		///< The file passed in already exists
		CouldNotOpenError = 1,		///< The file could not be opened
		InvalidError = 2
	};

	SGMXASScanActionControllerFileWriter(const QString &filePath, bool hasRank2Data = false, QObject *parent = 0);

public slots:
	void writeToFile(int fileRank, const QString &textToWrite);
	void finishWriting();

signals:
	void fileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError error);
	void fileWriterIsBusy(bool isBusy);

protected slots:
	void emitError(SGMXASScanActionControllerFileWriter::FileWriterError error);
	void emitErrors();
	void emitFileWriterIsBusy();

protected:
	QString filePath_;
	bool hasRank2Data_;

	QFile *rank1File_;
	QFile *rank2File_;

	QList<SGMXASScanActionControllerFileWriter::FileWriterError> errorsList_;
};

Q_DECLARE_METATYPE(SGMXASScanActionControllerFileWriter::FileWriterError)

#define SGMXASSCANACTIONCONTROLLER_CANNOT_INTIALIZE 272001
#define SGMXASSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION 272002
#define SGMXASSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 272003
#define SGMXASSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 272004
#define SGMXASSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 272005

class SGMXASScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent = 0);

	AMAction3* actionsTree();

	virtual bool isReadyForDeletion() const;

signals:
	void requestWriteToFile(int fileRank, const QString &textToWrite);
	void finishWritingToFile();

protected slots:
	void onActionTreeGenerated(AMAction3 *actionTree);
	void onFileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError error);
	void onFileWriterIsBusy(bool isBusy);

protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();

	bool event(QEvent *e);

	void writeHeaderToFile();
	void writeDataToFiles();

protected:
	AMAction3 *actionTree_;
	AMScanActionControllerScanAssembler *newScanAssembler_;

	AMnDIndex insertionIndex_;
	double currentAxisValue_;

	QThread *fileWriterThread_;
	bool fileWriterIsBusy_;
};

#endif // SGMXASSCANACTIONCONTROLLER_H
