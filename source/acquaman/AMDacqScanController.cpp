#include "AMDacqScanController.h"
#include <qdebug.h>

#include "../MPlot/src/MPlot/MPlotWidget.h"
#include "../MPlot/src/MPlot/MPlotSeriesData.h"
#include "../MPlot/src/MPlot/MPlotSeries.h"
#include "../MPlot/src/MPlot/MPlotImageData.h"
#include "../MPlot/src/MPlot/MPlotImage.h"
#include "../MPlot/src/MPlot/MPlotTools.h"

AMDacqScanController::AMDacqScanController(AMScanConfiguration *cfg, QObject *parent) : AMScanController(cfg, parent)
{
	_pCfg_ = &generalCfg_;
	_pScan_ = &generalScan_;

	running_ = FALSE;
	paused_ = FALSE;
	cancelled_ = FALSE;
	QEpicsAcqLocal *lAcq = new QEpicsAcqLocal((QWidget*)parent);
	advAcq_ = new QEpicsAdvAcq(lAcq);
	connect(advAcq_, SIGNAL(onStart()), this, SLOT(onStart()));
	connect(advAcq_, SIGNAL(onStop()), this, SLOT(onStop()));
	connect(advAcq_, SIGNAL(onPause(int)), this, SLOT(onPause(int)));
	connect(advAcq_, SIGNAL(sendCompletion(int)), this, SLOT(onSendCompletion(int)));
}

///// Sets a new scan configuration
//void AMDacqScanController::newConfigurationLoad(AMScanConfiguration &cfg)
//{
//}

/// Cancel scan if currently running or paused
void AMDacqScanController::cancel()
{
	advAcq_->Stop();
	cancelled_ = TRUE;
	emit cancelled();
}

void AMDacqScanController::onStart()
{
	running_ = TRUE;
	startTime_.start();
	emit started();
}

void AMDacqScanController::onStop()
{
	running_ = FALSE;
	if(cancelled_)
		cancelled_ = FALSE;
	else
		emit finished();

	if(pScan_()){
/*
		qDebug() << "END OF SCAN\n\n\nStarting to print scan data for";
		qDebug() << curScan_->detectors().count() << " columns";
		qDebug() << curScan_->d_->count() << " rows";

		for(int x = 0; unsigned(x) < curScan_->d_->count(); x++){
			qDebug() << "Starting row " << x;
			for(int y = 0; y < curScan_->detectors().count(); y++)
				qDebug() << "Value is " << curScan_->d_->value(y, x);
			qDebug() << "Done row\n";
		}
		*/

		MPlotWidget *plotWindow = new MPlotWidget();
		MPlot *plot = new MPlot();
		plotWindow->setPlot(plot);

		MPlotSeriesBasic *series1;
//		for(int y = 0; y < curScan_->numChannels(); y++){
		for(int y = 2; y < 5; y++){
			series1 = new MPlotSeriesBasic();

			qDebug() << "Plotting " << pScan_()->channel(y)->name();
			series1->setModel(pScan_()->channel(y));
			plot->addItem(series1);
		}


		plot->setScalePadding(5);
		plot->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
		plotWindow->show();
	}

}

void AMDacqScanController::onPause(int mode)
{
	if(mode == 0){
		paused_ = TRUE;
		emit paused();
	}
	else if(mode == 1){
		paused_ = FALSE;
		emit resumed();
	}
}

void AMDacqScanController::onSendCompletion(int completion){
	double tc = ((double)startTime_.elapsed())/1000;
	double remaining = (completion != 0) ? (100*tc)/((double)completion) - tc : tc*100000;
	emit timeRemaining(remaining);
	emit progress(tc, tc+remaining);
}
