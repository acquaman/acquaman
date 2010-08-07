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
		SGMXASScanConfigurationViewer(QWidget *parent = 0);
		~SGMXASScanConfigurationViewer();

signals:
	void scanControllerReady(AMScanController *xasCtrl);

public slots:
	void onAddRegionClicked();

protected slots:
	void onStartScanClicked(){
		qDebug() << "Detector after: "
				<< ((PGTDetector*)(((SGMXASScanConfiguration*)(cfg_))->detectorSet()->detectorByName("pgt")))->integrationTime()
				<< ((PGTDetector*)(((SGMXASScanConfiguration*)(cfg_))->detectorSet()->detectorByName("pgt")))->integrationMode()
				<< ((PGTDetector*)(((SGMXASScanConfiguration*)(cfg_))->detectorSet()->detectorByName("pgt")))->hvSetpoint()
				<< ((MCPDetector*)(((SGMXASScanConfiguration*)(cfg_))->detectorSet()->detectorByName("tfy")))->hvSetpoint();

		qDebug() << "cfgDetectorInfo after: "
				<< ((PGTDetectorInfo*)cfgDetectorInfoSet_->detectorByName("pgt"))->integrationTime()
				<< ((PGTDetectorInfo*)cfgDetectorInfoSet_->detectorByName("pgt"))->integrationMode()
				<< ((PGTDetectorInfo*)cfgDetectorInfoSet_->detectorByName("pgt"))->hvSetpoint()
				<< ((MCPDetectorInfo*)cfgDetectorInfoSet_->detectorByName("tfy"))->hvSetpoint();

		((PGTDetector*)(((SGMXASScanConfiguration*)(cfg_))->detectorSet()->detectorByName("pgt")))->setControls( (PGTDetectorInfo*)cfgDetectorInfoSet_->detectorByName("pgt") );
		((MCPDetector*)(((SGMXASScanConfiguration*)(cfg_))->detectorSet()->detectorByName("tfy")))->setControls( (MCPDetectorInfo*)cfgDetectorInfoSet_->detectorByName("tfy") );

		//for(int x = 0; x < ((SGMXASScanConfiguration*)(cfg_))->detectorSet()->count(); x++)

		/*SGMXASDacqScanController *xasCtrl = new SGMXASDacqScanController((SGMXASScanConfiguration*)cfg_, SGMBeamline::sgm());
		emit scanControllerReady((AMScanController*)xasCtrl);
		xasCtrl->initialize();
		xasCtrl->start();
		*/
	}
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
	QVBoxLayout vl_;
	QGridLayout gl_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
