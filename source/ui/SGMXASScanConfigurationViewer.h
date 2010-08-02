#ifndef ACQMAN_SGMXASScanConfigurationViewer_H
#define ACQMAN_SGMXASScanConfigurationViewer_H

#include <QtGui>
#include "ui_SGMXASScanConfigurationViewer.h"
#include "AMControlSetView.h"
#include "AMXASRegionsView.h"
#include "AMRegionsLineView.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"


class SGMXASScanConfigurationViewer : public QWidget, private Ui::SGMXASScanConfigurationViewer {
Q_OBJECT
public:
		SGMXASScanConfigurationViewer(QWidget *parent = 0);
		~SGMXASScanConfigurationViewer();

signals:
	void scanControllerReady(AMScanController *xasCtrl);

public slots:
	void onAddRegionClicked();

protected slots:
	void onStartScanClicked(){
		SGMXASDacqScanController *xasCtrl = new SGMXASDacqScanController((SGMXASScanConfiguration*)cfg_, SGMBeamline::sgm());
		emit scanControllerReady((AMScanController*)xasCtrl);
		xasCtrl->initialize();
		xasCtrl->start();
	}
	void onRegionsChanged(){
		qDebug() << "Master detected regionUpdate";
		if(cfg_ && fluxResolutionView_)
			fluxResolutionView_->onRegionsUpdate( ((SGMXASScanConfiguration*)cfg_)->regions() );
		if(cfg_ && fluxResolutionView2_){
			qDebug() << "Calling compact update";
			fluxResolutionView2_->onRegionsUpdate( ((SGMXASScanConfiguration*)cfg_)->regions() );
		}
	}

protected:
	AMScanConfiguration *cfg_;
	AMXASRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
	AMControlOptimizationSetView *fluxResolutionView_;
	AMCompactControlOptimizationSetView *fluxResolutionView2_;
//	AMColorControlOptimizationSetView *fluxResolutionView2_;
	AMControlSetView *trackingView_;
	AMAbstractDetectorSetView *detectorView_;
	QPushButton *startScanButton_;
	QVBoxLayout vl_;
	QGridLayout gl_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
