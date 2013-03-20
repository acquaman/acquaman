#ifndef SGMXASSCANACTIONCONTROLLER_H
#define SGMXASSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "SGMXASScanController.h"

#define SGMXASSCANACTIONCONTROLLER_CANT_INTIALIZE 272001

class SGMXASScanActionController : public AMScanActionController, public SGMXASScanController
{
Q_OBJECT
public:
	SGMXASScanActionController(SGMXASScanConfiguration *cfg, QObject *parent = 0);

	AMAction3* actionsTree();

protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();

protected:
	AMAction3 *actionTree_;
};

#endif // SGMXASSCANACTIONCONTROLLER_H
