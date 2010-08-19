#ifndef AMSCANCONFIGURATIONVIEW_H
#define AMSCANCONFIGURATIONVIEW_H

#include <QWidget>
#include "ui/SGMXASScanConfigurationViewer.h"
#include "ui/SGMXASScanConfigurationWizard.h"

class AMScanConfigurationQueueDirector;


class AMXASScanConfigurationHolder : public QWidget
{
	Q_OBJECT
public:
	AMXASScanConfigurationHolder(QWidget *parent = 0);
	~AMXASScanConfigurationHolder();

public slots:
	void onFreeToScan(bool ready);
	void onAddedToQueue(AMScanConfiguration *cfg);

	/// This slot will be used to notify this widget when it becomes the current (active) widget
	void onBecameCurrentWidget();


signals:
	void startScanRequested();
	void addToQueueRequested(AMScanConfiguration *cfg);
	void goToQueueRequested();

protected slots:
	void createScanConfiguration();
	void destroyScanConfigurationViewer();


	void onStartScanRequested();
	void onAddToQueueRequested();

	void goToQueue();
	void goToNewScan();


protected:
	SGMXASScanConfiguration *cfg_;
	AMDetectorInfoSet *cfgDetectorInfoSet_;

	SGMXASScanConfigurationViewer *sxscViewer;
	SGMXASScanConfigurationWizard *sxscWizard;
	AMScanConfigurationQueueDirector *director;
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

#endif // AMSCANCONFIGURATIONVIEW_H
