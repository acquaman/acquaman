#ifndef ACQMAN_SGMXASScanConfigurationViewer_H
#define ACQMAN_SGMXASScanConfigurationViewer_H

#include <QtGui>
#include "ui_SGMXASScanConfigurationViewer.h"
#include "AMControlSetView.h"
#include "AMXASRegionsView.h"
#include <QPushButton>
#include <QVBoxLayout>
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
	void setScanConfiguration(AMScanConfiguration *cfg);

protected slots:
	void onDoLayout(){
		if(!SGMBeamline::sgm()->isConnected())
		{qDebug() << "SGM not connected"; return;}
		AMScanConfiguration *sxsc = new SGMXASScanConfiguration(this);

		/*
		QList<chPair> newChannels(sxsc->defaultChannels());
		newChannels.append(chPair("dave", "dave"));
		sxsc->setDefaultChannels(newChannels);

		pScan_()->addChannel("bigTEYNorm", "10000*tey/I0");
		pScan_()->addChannel("bigTFYNorm", "-10000*tfy/I0");

		pScan_()->addChannel("bigTeyNorm", "10000*tey/I0");
		pScan_()->addChannel("pgtTest1", "pgt.pgtCounts[130]");
		pScan_()->addChannel("pgtTest2", "pgt[130].pgtCounts");


		QString indexer = "";
		for(int x = 0; x < 1024; x++){
			indexer.setNum(x);
			pScan_()->addChannel("PGT_COUNTS"+indexer, "pgt.pgtCounts["+indexer+"]");
		}
		*/

		setScanConfiguration(sxsc);
	}

	void onStartScanClicked(){
		SGMXASDacqScanController *xasCtrl = new SGMXASDacqScanController((SGMXASScanConfiguration*)cfg_, SGMBeamline::sgm());
		emit scanControllerReady((AMScanController*)xasCtrl);
		xasCtrl->initialize();
		xasCtrl->start();
	}

protected:
	AMScanConfiguration *cfg_;
	AMXASRegionsView *regionsView_;
	AMControlOptimizationSetView *fluxResolutionView_;
	AMControlSetView *trackingView_;
	AMAbstractDetectorSetView *detectorView_;
	QPushButton *startScanButton_;
	QVBoxLayout vl_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
