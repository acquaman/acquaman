#ifndef SGMFASTSCANACTIONCONTROLLER_H
#define SGMFASTSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

#include <QThread>

#include "acquaman/AMRegionScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMRegionScanActionControllerBasicFileWriter::FileWriterError)

class AMScanActionControllerScanAssembler;
class AMListAction3;

#define SGMFASTSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 288003
#define SGMFASTSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 288004
#define SGMFASTSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 288005

class SGMFastScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	SGMFastScanActionController(SGMFastScanConfiguration2013 *configuration, QObject *parent = 0);

	virtual void buildScanController() {}

signals:
	void requestWriteToFile(int fileRank, const QString &textToWrite);
	void finishWritingToFile();

protected slots:
	void onInitializationActionsListSucceeded();
	void onInitializationActionsListFailed();
	void onMasterActionsListSucceeded();
	void onMasterActionsListFailed();
	void onCleanupActionsListSucceeded();
	void onCleanupActionsListFailed();

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

	virtual void buildScanControllerImplementation() {}

protected:
	SGMFastScanConfiguration2013 *configuration_;

	int encoderStartValue_;
	double spacingParam_;
	double c1Param_;
	double c2Param_;
	double sParam_;
	double thetaParam_;

	QMap< QString, QVector<double> > allDataMap_;
	AMnDIndex insertionIndex_;

	AMListAction3 *fastActionsInitializationList_;
	AMListAction3 *fastActionsMasterList_;
	bool masterListSucceeded_;
	AMListAction3 *fastActionsCleanupList_;

	QThread *fileWriterThread_;
	bool fileWriterIsBusy_;
};

#endif // SGMFASTSCANACTIONCONTROLLER_H
