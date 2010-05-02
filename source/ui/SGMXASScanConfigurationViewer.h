#ifndef ACQMAN_SGMXASScanConfigurationViewer_H
#define ACQMAN_SGMXASScanConfigurationViewer_H

#include <QtGui>
#include "ui_SGMXASScanConfigurationViewer.h"
#include "AMControlSetView.h"
#include "AMXASRegionsView.h"
#include <QVBoxLayout>
#include "acquaman/SGM/SGMXASScanConfiguration.h"

/*
#include "../MPlot/src/MPlot/MPlotWidget.h"
#include "../MPlot/src/MPlot/MPlotSeriesData.h"
#include "../MPlot/src/MPlot/MPlotSeries.h"
#include "../MPlot/src/MPlot/MPlotImageData.h"
#include "../MPlot/src/MPlot/MPlotImage.h"
#include "../MPlot/src/MPlot/MPlotTools.h"

#include <QTableView>
#include <QPen>
#include <QBrush>
*/

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
		sxsc->addRegion(0, 500, 1, 560);
		sxsc->addRegion(1, 560.5, 0.5, 620);
		sxsc->addRegion(2, 620.1, 0.1, 700);

//		setFluxResolutionSet(sxsc->fluxResolutionSet());
		regionsView_ = new AMXASRegionsView(sxsc->regionsPtr(), this);
		fluxResolutionView_ = new AMControlOptimizationSetView((AMControlOptimizationSet*)(sxsc->fluxResolutionSet()), this);
		QList<AMXASRegion*> oldRegions = sxsc->regions();
		QList<AMRegion*> newRegions;
		AMRegion* tmpRegion;
		for(int x = 0; x < oldRegions.count(); x++){
			tmpRegion = oldRegions.at(x);
			newRegions << tmpRegion;
		}
		fluxResolutionView_->onRegionsUpdate(newRegions);
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
	AMControlOptimizationSetView *fluxResolutionView_;
	AMControlSetView *trackingView_;
	QVBoxLayout vl_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
