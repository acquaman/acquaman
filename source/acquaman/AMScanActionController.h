#ifndef AMSCANACTIONCONTROLLER_H
#define AMSCANACTIONCONTROLLER_H

#include "AMScanController.h"

class AMScanActionController : public AMScanController
{
Q_OBJECT
public:
	AMScanActionController(AMScanConfiguration *configuration, QObject *parent = 0);

public slots:
	virtual void skip(const QString &command);

protected slots:
	void onStateChanged(int oldState, int newState);

protected:
	virtual bool startImplementation();
	virtual bool canPause() const;
	virtual void pauseImplementation();
	virtual void resumeImplementation();
	virtual void cancelImplementation();

	bool event(QEvent *e);
};

#endif // AMSCANACTIONCONTROLLER_H
