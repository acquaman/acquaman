#ifndef REIXSXASSCANACTIONCONTROLLER_H
#define REIXSXASSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

#include <QThread>

class AMScanActionControllerScanAssembler;
class AMListAction3;

#include "acquaman/SGM/SGMXASScanActionControllerFileWriter.h"
Q_DECLARE_METATYPE(SGMXASScanActionControllerFileWriter::FileWriterError)

#define REIXSXASSCANACTIONCONTROLLER_CANNOT_INTIALIZE 279001
#define REIXSXASSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION 279002
#define REIXSXASSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 279003
#define REIXSXASSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 279004
#define REIXSXASSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 279005

class REIXSXASScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	REIXSXASScanActionController(REIXSXASScanConfiguration *cfg, QObject *parent = 0);

	AMAction3* actionsTree();

	virtual bool isReadyForDeletion() const;

signals:
	void requestWriteToFile(int fileRank, const QString &textToWrite);
	void finishWritingToFile();

protected slots:
	void onInitializationActionsListSucceeded();
	void onInitializationActionsListFailed();
	void onActionsTreeSucceeded();
	void onActionsTreeFailed();
	void onCleanupActionsListSucceeded();
	void onCleanupActionsListFailed();

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

	AMAction3* createInitializationActions();
	AMAction3* createCleanupActions();

protected:
	AMAction3 *actionTree_;
	bool actionTreeSucceeded_;
	AMListAction3 *xasActionsInitializationList_;
	AMListAction3 *xasActionsCleanupList_;
	AMScanActionControllerScanAssembler *newScanAssembler_;
	REIXSXASScanConfiguration *configuration_;

	AMnDIndex insertionIndex_;
	double currentAxisValue_;

	QThread *fileWriterThread_;
	bool fileWriterIsBusy_;
};

#endif // REIXSXASSCANACTIONCONTROLLER_H
