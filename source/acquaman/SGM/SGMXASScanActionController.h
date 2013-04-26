#ifndef SGMXASSCANACTIONCONTROLLER_H
#define SGMXASSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

#include <QThread>

class AMScanActionControllerScanAssembler;

#include "acquaman/SGM/SGMXASScanActionControllerFileWriter.h"
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
	void onScanFinished();

protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();

	bool event(QEvent *e);

	void writeHeaderToFile();
	void writeDataToFiles();

	AMAction3* createInitializationActions();
	AMAction3* createCleanupActions();

protected:
	AMAction3 *actionTree_;
	AMScanActionControllerScanAssembler *newScanAssembler_;
	SGMXASScanConfiguration2013 *configuration_;

	AMnDIndex insertionIndex_;
	double currentAxisValue_;

	QThread *fileWriterThread_;
	bool fileWriterIsBusy_;
};

#endif // SGMXASSCANACTIONCONTROLLER_H
