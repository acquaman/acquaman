#ifndef AMSCANCONFIGURATIONVIEW_H
#define AMSCANCONFIGURATIONVIEW_H

#include <QWidget>
#include "ui/SGMXASScanConfigurationViewer.h"
#include "ui/SGMXASScanConfigurationWizard.h"

class AMXASScanConfigurationHolder : public QWidget
{
	Q_OBJECT
public:
	AMXASScanConfigurationHolder(QWidget *parent = 0);
	~AMXASScanConfigurationHolder();

public slots:
	void onFreeToScan(bool ready);

signals:
	void startScanRequested();
	void addToQueueRequested(AMScanConfiguration *cfg);

protected slots:
	void createScanConfiguration();
	void onSidebarLinkChanged();
	void onStartScanRequested();
	void onAddToQueueRequested();

protected:
	SGMXASScanConfiguration *cfg_;
	AMDetectorInfoSet *cfgDetectorInfoSet_;

	SGMXASScanConfigurationViewer *sxscViewer;
	SGMXASScanConfigurationWizard *sxscWizard;
	QVBoxLayout *vl_;
};

#endif // AMSCANCONFIGURATIONVIEW_H
