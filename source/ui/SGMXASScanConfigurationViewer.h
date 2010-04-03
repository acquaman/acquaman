#ifndef ACQMAN_SGMXASScanConfigurationViewer_H
#define ACQMAN_SGMXASScanConfigurationViewer_H

#include <QtGui>
#include "ui_SGMXASScanConfigurationViewer.h"
#include "AMControlSetView.h"
#include "AMXASRegionsView.h"
#include <QVBoxLayout>
#include "acquaman/SGM/SGMXASScanConfiguration.h"

class SGMXASScanConfigurationViewer : public QWidget, private Ui::SGMXASScanConfigurationViewer {
Q_OBJECT
public:
		SGMXASScanConfigurationViewer(QWidget *parent = 0) : QWidget(parent){
			setupUi(this);
			connect(doLayoutButton, SIGNAL(clicked()), this, SLOT(onDoLayout()));
		}

signals:

public slots:
	void setScanConfiguration(AMScanConfiguration *cfg){
		cfg_ = cfg;
		SGMXASScanConfiguration *sxsc = (SGMXASScanConfiguration*)cfg_;
		sxsc->addRegion(0, 250, 1, 260);
		sxsc->addRegion(1, 260.5, 0.5, 265);
		sxsc->addRegion(2, 265.1, 0.1, 266);
//		setFluxResolutionSet(sxsc->fluxResolutionSet());
		regionsView_ = new AMXASRegionsView(sxsc->regionsPtr(), this);
		fluxResolutionView_ = new AMControlSetView(sxsc->fluxResolutionSet(), this);
		trackingView_ = new AMControlSetView(sxsc->trackingSet(), this);
		delete doLayoutButton;
		delete layout();
		vl_.addWidget(regionsView_);
		vl_.addWidget(fluxResolutionView_);
		vl_.addWidget(trackingView_);
		this->setLayout(&vl_);
	}

//	void setFluxResolutionSet(AMControlSet *viewSet){
//		fluxResolutionView_ = new AMControlSetView(viewSet, this);
//		delete doLayoutButton;
//		delete layout();
//		hl_.addWidget(fluxResolutionView_);
//		this->setLayout(&hl_);
//	}

protected slots:
	void onDoLayout(){
		AMScanConfiguration *sxsc = new SGMXASScanConfiguration(this);
		setScanConfiguration(sxsc);
	}

protected:
	AMScanConfiguration *cfg_;
	AMXASRegionsView *regionsView_;
	AMControlSetView *fluxResolutionView_;
	AMControlSetView *trackingView_;
	QVBoxLayout vl_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
