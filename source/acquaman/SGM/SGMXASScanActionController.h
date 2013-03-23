#ifndef SGMXASSCANACTIONCONTROLLER_H
#define SGMXASSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class AMScanActionControllerScanAssembler;
class QFile;

#define SGMXASSCANACTIONCONTROLLER_CANT_INTIALIZE 272001

class SGMXASScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent = 0);

	AMAction3* actionsTree();

	void setPointer(QObject *pointer);

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

	QObject *pointer_;

	QFile *rank1File_;
	QFile *rank2File_;
	QTextStream rank1Stream_;
	QTextStream rank2Stream_;
};

#endif // SGMXASSCANACTIONCONTROLLER_H
