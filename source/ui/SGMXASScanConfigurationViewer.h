#ifndef ACQMAN_SGMXASScanConfigurationViewer_H
#define ACQMAN_SGMXASScanConfigurationViewer_H

#include <QtGui>
#include "ui_SGMXASScanConfigurationViewer.h"
#include "AMControlSetView.h"
#include "AMDetectorView.h"
#include "AMXASRegionsView.h"
#include "AMRegionsLineView.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"


class SGMXASScanConfigurationViewer : public QWidget, private Ui::SGMXASScanConfigurationViewer {
Q_OBJECT
public:
		SGMXASScanConfigurationViewer(SGMXASScanConfiguration *sxsc, AMDetectorInfoSet *cfgDetectorInfoSet, QWidget *parent = 0);
		~SGMXASScanConfigurationViewer();

signals:
	void scanControllerReady(AMScanController *xasCtrl);
	void startScanRequested();
	void addToQueueRequested();

public slots:
	void onAddRegionClicked();

protected slots:
	void onStartScanClicked(){ emit startScanRequested(); }
	void onAddToQueueRequested() { emit addToQueueRequested(); }
	void onRegionsChanged(){
		if(cfg_ && fluxResolutionView_){
			fluxResolutionView_->onRegionsUpdate( ((SGMXASScanConfiguration*)cfg_)->regions() );
		}
	}

protected:
	AMScanConfiguration *cfg_;
	AMXASRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
	AMCompactControlOptimizationSetView *fluxResolutionView_;
	AMControlSetView *trackingView_;
	AMDetectorInfoSetView *detectorView_;
	AMDetectorInfoSet *cfgDetectorInfoSet_;
	QPushButton *startScanButton_;
	QPushButton *addToQueueButton_;
	QVBoxLayout vl_;
	QGridLayout gl_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
