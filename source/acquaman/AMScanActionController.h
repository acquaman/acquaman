#ifndef AMSCANACTIONCONTROLLER_H
#define AMSCANACTIONCONTROLLER_H

#include "AMScanController.h"

class AMScanActionController : public AMScanController
{
Q_OBJECT
public:
	AMScanActionController(AMScanConfiguration *configuration, QObject *parent = 0);

protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual bool canPause();
	virtual void pauseImplementation();
	virtual void resumeImplementation();
	virtual void cancelImplementation();
};

#endif // AMSCANACTIONCONTROLLER_H
