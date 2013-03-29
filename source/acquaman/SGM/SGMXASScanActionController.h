#ifndef SGMXASSCANACTIONCONTROLLER_H
#define SGMXASSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

#include <QThread>

class AMScanActionControllerScanAssembler;
class QFile;

#define SGMXASSCANACTIONCONTROLLER_CANT_INTIALIZE 272001

class SGMXASScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent = 0);

	AMAction3* actionsTree();

signals:
	void requestWriteToFile(int fileRank, const QString &textToWrite);

protected slots:
	void onActionTreeGenerated(AMAction3 *actionTree);

protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();

	bool event(QEvent *e);

	void writeToFiles();

protected:
	AMAction3 *actionTree_;
	AMScanActionControllerScanAssembler *newScanAssembler_;

	AMnDIndex insertionIndex_;
	double currentAxisValue_;

	QThread *fileWriterThread_;
};


class SGMXASScanActionControllerFileWriter : public QObject
{
Q_OBJECT
public:
	SGMXASScanActionControllerFileWriter(const QString &filePath, bool hasRank2Data = false, QObject *parent = 0);

public slots:
	void writeToFile(int fileRank, const QString &textToWrite);

protected:
	QString filePath_;
	bool hasRank2Data_;

	QFile *rank1File_;
	QFile *rank2File_;
};

#endif // SGMXASSCANACTIONCONTROLLER_H
