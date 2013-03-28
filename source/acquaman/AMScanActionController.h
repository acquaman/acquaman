#ifndef AMSCANACTIONCONTROLLER_H
#define AMSCANACTIONCONTROLLER_H

#include "AMScanController.h"

class AMScanActionController : public AMScanController
{
Q_OBJECT
public:
	AMScanActionController(AMScanConfiguration *configuration, QObject *parent = 0);

protected slots:
	void onStateChanged(int oldState, int newState);

protected:
	virtual bool startImplementation();
	virtual bool canPause();
	virtual void pauseImplementation();
	virtual void resumeImplementation();
	virtual void cancelImplementation();

	bool event(QEvent *e);
};

#endif // AMSCANACTIONCONTROLLER_H
