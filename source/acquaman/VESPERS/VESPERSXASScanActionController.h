#ifndef VESPERSXASSCANACTIONCONTROLLER_H
#define VESPERSXASSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

#include <QThread>

class AMScanActionControllerScanAssembler;
class AMListAction3;

#include "acquaman/AMRegionScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMRegionScanActionControllerBasicFileWriter::FileWriterError)

#define VESPERSXASSCANACTIONCONTROLLER_CANNOT_INTIALIZE 272001
#define VESPERSXASSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION 272002
#define VESPERSXASSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 272003
#define VESPERSXASSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 272004
#define VESPERSXASSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 272005

class VESPERSXASScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	VESPERSXASScanActionController(VESPERSEXAFSScanConfiguration *configuration, QObject *parent = 0);

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
	void onFileWriterError(AMRegionScanActionControllerBasicFileWriter::FileWriterError error);
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
	VESPERSEXAFSScanConfiguration *configuration_;

	AMnDIndex insertionIndex_;
	double currentAxisValue_;

	QThread *fileWriterThread_;
	bool fileWriterIsBusy_;
};

#endif // VESPERSXASSCANACTIONCONTROLLER_H
