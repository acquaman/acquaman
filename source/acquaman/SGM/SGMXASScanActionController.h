#ifndef SGMXASSCANACTIONCONTROLLER_H
#define SGMXASSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "SGMXASScanController.h"
#include "dataman/AMUser.h"

#define SGMXASSCANACTIONCONTROLLER_CANT_INTIALIZE 272001

class SGMXASScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	SGMXASScanActionController(SGMXASScanConfiguration *cfg, QObject *parent = 0);

	AMAction3* actionsTree();

	void setPointer(QObject *pointer);

protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();

	bool event(QEvent *e);

protected:
	AMAction3 *actionTree_;

	AMnDIndex insertionIndex_;
	double currentAxisValue_;

	QObject *pointer_;
};

#endif // SGMXASSCANACTIONCONTROLLER_H
