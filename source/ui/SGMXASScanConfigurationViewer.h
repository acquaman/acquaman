#ifndef ACQMAN_SGMXASScanConfigurationViewer_H
#define ACQMAN_SGMXASScanConfigurationViewer_H

#include <QtGui>
#include "ui_SGMXASScanConfigurationViewer.h"
#include "AMControlSetView.h"
#include <QHBoxLayout>
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
		setFluxResolutionSet(sxsc->fluxResolutionSet());
	}

	void setFluxResolutionSet(AMControlSet *viewSet){
		fluxResolutionView_ = new AMControlSetView(viewSet, this);
//		mainHl->addWidget(fluxResolutionView_);
		delete doLayoutButton;
		delete layout();
		hl_.addWidget(fluxResolutionView_);
		this->setLayout(&hl_);
	}

protected slots:
	void onDoLayout(){
//		setFluxResolutionSet(SGMBeamline::sgm()->fluxResolutionSet());
		AMScanConfiguration *sxsc = new SGMXASScanConfiguration(this);
		setScanConfiguration(sxsc);
	}

protected:
	AMScanConfiguration *cfg_;
	AMControlSetView *fluxResolutionView_;
	QHBoxLayout hl_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
