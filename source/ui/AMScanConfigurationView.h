#ifndef AMSCANCONFIGURATIONVIEW_H
#define AMSCANCONFIGURATIONVIEW_H

#include <QWidget>
#include "ui/SGMXASScanConfigurationViewer.h"
#include "ui/SGMXASScanConfigurationWizard.h"
#include "ui/SGMFastScanConfigurationViewer.h"

class AMScanConfigurationQueueDirector;
class AMScanConfigurationScanDirector;

class AMScanConfigurationHolder : public QWidget
{
	Q_OBJECT
public:
	AMScanConfigurationHolder(QWidget *parent = 0);
	~AMScanConfigurationHolder();

public slots:
	virtual void onFreeToScan(bool queueEmpty, bool queueNotRunning);
	virtual void onLockdownScanning(bool isLocked, QString reason);
	virtual void onAddedToQueue(AMScanConfiguration *cfg);

	/// This slot will be used to notify this widget when it becomes the current (active) widget
	virtual void onBecameCurrentWidget() = 0;

signals:
	void addToQueueRequested(AMScanConfiguration *cfg, bool startNow = false);
	void cancelAddToQueueRequest();
	void goToQueueRequested();
	void lockdownScanning(bool isLocked, QString reason);
	void newScanConfigurationView();

protected slots:
	virtual void createScanConfiguration() = 0;
	virtual void destroyScanConfigurationViewer() = 0;

	virtual void onStartScanRequested();
	virtual void onAddToQueueRequested();

	virtual void goToQueue();
	virtual void goToNewScan();

protected:
	AMScanConfiguration *cfg_;
	bool requestedStart_;
	bool canStartImmediately_;

	AMScanConfigurationQueueDirector *director;
	AMScanConfigurationScanDirector *sDirector;
};

class AMXASScanConfigurationHolder : public AMScanConfigurationHolder
{
	Q_OBJECT
public:
	AMXASScanConfigurationHolder(QWidget *parent = 0);
	~AMXASScanConfigurationHolder();

public slots:
	/// This slot will be used to notify this widget when it becomes the current (active) widget
	void onBecameCurrentWidget();

protected slots:
	void createScanConfiguration();
	void destroyScanConfigurationViewer();

private:
	SGMXASScanConfiguration* cfg() { return qobject_cast<SGMXASScanConfiguration*>(cfg_);}

protected:
	AMDetectorInfoSet *cfgDetectorInfoSet_;

	SGMXASScanConfigurationViewer *sxscViewer;
	SGMXASScanConfigurationWizard *sxscWizard;
	QVBoxLayout *vl_;
};

class AMScanConfigurationQueueDirector : public QWidget
{
	Q_OBJECT
public:
	AMScanConfigurationQueueDirector(QWidget *parent = 0);

public slots:
	void showDirector();

signals:
	void goToQueue();
	void goToNewScan();

protected slots:
	void onAlwaysQueueCheck(bool checked);
	void onAlwaysNewScanCheck(bool checked);

protected:
	bool alwaysGoToQueue_;
	bool alwaysGoToNewScan_;

	QVBoxLayout *vl_;
	QLabel *message_;
	QFormLayout *choices_;
	QPushButton *goToQueueButton_;
	QPushButton *goToNewScanButton_;
	QCheckBox *goToQueueCheck_;
	QCheckBox *goToNewScanCheck_;
};

class AMScanConfigurationScanDirector : public QWidget
{
Q_OBJECT
public:
	AMScanConfigurationScanDirector(QWidget *parent = 0);

public slots:
	void showDirector(QString reason);

signals:
	void cancel();
	void startNow();
	void addToQueue(int index);

protected slots:
	void onWaitNext();
	void onAppend();

	void onAlwaysStartNow(bool checked);
	void onAlwaysWaitNext(bool checked);
	void onAlwaysAppend(bool checked);
	void onAlwaysCancel(bool checked);

protected:
	QString message_;
	bool alwaysStartNow_;
	bool alwaysWaitNext_;
	bool alwaysAppend_;
	bool alwaysCancel_;

	QVBoxLayout *vl_;
	QLabel *messageLabel_;
	QGridLayout *gl_;
	QPushButton *startNowButton_;
	QPushButton *waitNextButton_;
	QPushButton *appendButton_;
	QPushButton *cancelButton_;
	QLabel *startNowLabel_;
	QLabel *waitNextLabel_;
	QLabel *appendLabel_;
	QLabel *cancelLabel_;
	QLabel *setDefaultLabel_;
	QCheckBox *startNowCheck_;
	QCheckBox *waitNextCheck_;
	QCheckBox *appendCheck_;
	QCheckBox *cancelCheck_;
};

class AMFastScanConfigurationHolder : public AMScanConfigurationHolder
{
	Q_OBJECT
public:
	AMFastScanConfigurationHolder(QWidget *parent = 0);
	~AMFastScanConfigurationHolder();

public slots:
	/// This slot will be used to notify this widget when it becomes the current (active) widget
	void onBecameCurrentWidget();

protected slots:
	void createScanConfiguration();
	void destroyScanConfigurationViewer();

private:
	SGMFastScanConfiguration* cfg() { return qobject_cast<SGMFastScanConfiguration*>(cfg_);}

protected:
	SGMFastScanConfigurationViewer *sfscViewer_;

	QVBoxLayout *vl_;
};

#endif // AMSCANCONFIGURATIONVIEW_H
