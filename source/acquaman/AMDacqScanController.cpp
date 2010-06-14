#include "AMDacqScanController.h"
#include <qdebug.h>
#include <qtableview.h>

#include "../MPlot/src/MPlot/MPlotWidget.h"
#include "../MPlot/src/MPlot/MPlotSeriesData.h"
#include "../MPlot/src/MPlot/MPlotSeries.h"
#include "../MPlot/src/MPlot/MPlotImageData.h"
#include "../MPlot/src/MPlot/MPlotImage.h"
#include "../MPlot/src/MPlot/MPlotTools.h"

#include "ui/AMScanView.h"

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
		qDebug() << "Before scan view";
		AMScanView *sv = new AMScanView();
//		sv->show();
		AMScanSetModel* model = sv->model();
		model->addScan(pScan_());
		sv->show();
		qDebug() << "After scan view";
		*/

		/*
		MPlotWidget *plotWindow = new MPlotWidget();
		MPlot *plot = new MPlot();
		plotWindow->setPlot(plot);


		MPlotSeriesBasic *series1;
//		for(int y = 0; y < curScan_->numChannels(); y++){
		int maxChannels = pScan_()->numChannels();
		for(int y = 2; y < maxChannels-1024; y++){
			series1 = new MPlotSeriesBasic();

//			qDebug() << "Plotting " << pScan_()->channel(y)->name();
			series1->setModel(pScan_()->channel(y));
			plot->addItem(series1);
		}
		plot->setScalePadding(5);	// set axis scale padding in percent
		plot->setXDataRange(pScan_()->channel("eV")->value(0) - 10, pScan_()->channel("eV")->value(pScan_()->count()-1) + 10);		// Manually set the axis range

		plot->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
		plotWindow->show();

		play2d();
		*/
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

void AMDacqScanController::play2d()
{
	MPlotWidget *plotWindow = new MPlotWidget();

	MPlot *plot = new MPlot();
	plotWindow->setPlot(plot);

	plot->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:

	plot->axisBottom()->setAxisName("eV");
	plot->axisLeft()->setAxisName("Intensity (arb. units)");

	plot->setMarginTop(5);
	plot->setMarginRight(5);
	plot->setMarginLeft(15);
	plot->setMarginBottom(15);

	plot->axisTop()->setTicks(0);
	plot->axisTop()->setAxisName("time (s)");
	plot->axisTop()->showAxisName();

	MPlotSimpleImageData *data2d = new MPlotSimpleImageData(QRectF(pScan_()->channel("eV")->value(0),0,pScan_()->channel("eV")->value(pScan_()->count()-1),1024), QSize(pScan_()->count(),1024));
	QString chRef = "";
	for(int yy=0; yy<1024; yy++) {
		chRef.setNum(yy);
		chRef.prepend("PGT_COUNTS");
		AMChannel* ch = pScan_()->channel(chRef);

		for(int xx=0; (unsigned)xx<pScan_()->count(); xx++) {
			data2d->setZ(ch->value(xx), xx, yy);
		}
	}

	MPlotImageBasic *plot2d = new MPlotImageBasic(data2d);
	plot->addItem(plot2d);

	plot->setScalePadding(5);	// set axis scale padding in percent
	plot->setXDataRange(pScan_()->channel("eV")->value(0) - 10, pScan_()->channel("eV")->value(pScan_()->count()-1) + 10);		// Manually set the axis range
	plot->setYDataRangeLeft(0, 1024);

	plot->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);

	plotWindow->resize(400, 300);
	plotWindow->show();

   // DragZoomerTools need to be added first ("on the bottom") so they don't steal everyone else's mouse events
   MPlotDragZoomerTool dzTool;
   plot->addTool(&dzTool);
   // dzTool.setEnabled(false);

   // this tool selects a plot with the mouse
   MPlotPlotSelectorTool psTool;
   plot->addTool(&psTool);
   // psTool.setEnabled(false);

   // this tool adds mouse-wheel based zooming
   MPlotWheelZoomerTool wzTool;
   plot->addTool(&wzTool);

   MPlotCursorTool crsrTool;
   plot->addTool(&crsrTool);
   crsrTool.addCursor();
}
