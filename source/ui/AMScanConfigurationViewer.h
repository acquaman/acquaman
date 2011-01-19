#ifndef AMSCANCONFIGURATIONVIEWER_H
#define AMSCANCONFIGURATIONVIEWER_H

#include <QtGui>

#include "acquaman/AMScanConfiguration.h"
#include "acquaman/AMScanController.h"

class AMScanConfigurationViewer : public QWidget
{
	Q_OBJECT
public:
    AMScanConfigurationViewer(AMScanConfiguration *cfg, QWidget *parent = 0);

signals:
	void scanControllerReady(AMScanController *xasCtrl);
	void startScanRequested();
	void addToQueueRequested();
	void queueDirectorRequested();

protected slots:
	virtual void onStartScanClicked(){ emit startScanRequested(); }
	virtual void onAddToQueueRequested() { emit addToQueueRequested(); }
	virtual void onLockdowScanning(bool isLocked, QString reason) = 0;

protected:
    AMScanConfiguration *cfg_;
};

#endif // AMSCANCONFIGURATIONVIEWER_H
