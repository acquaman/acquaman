#include "AMDacqScanController.h"
#include <qdebug.h>
#include <qtableview.h>

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

		/*
		MPlotWidget *plotWindow = new MPlotWidget();
		MPlot *plot = new MPlot();
		plotWindow->setPlot(plot);


		qDebug() << "\n\n\n\nEND STAGE PLOTTING";
		MPlotSeriesBasic *series1;
//		for(int y = 0; y < curScan_->numChannels(); y++){
		int maxChannels = pScan_()->numChannels();
		for(int y = 2; y < maxChannels; y++){
			series1 = new MPlotSeriesBasic();

			qDebug() << "Plotting " << pScan_()->channel(y)->name();
			series1->setModel(pScan_()->channel(y));
			plot->addItem(series1);
		}
		autoresize?
		plotWindow->show();
		*/

		play2d();
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
	// 1. Creating Plots:
	////////////////////////////

	// An MPlotWidget is needed to view a plot:
	MPlotWidget *plotWindow = new MPlotWidget();

	// An MPlot is the QGraphicsItem representing a 2D Plot:
	MPlot *plot = new MPlot();
	// Connecting the window to view the plot:
	plotWindow->setPlot(plot);

	// 2. Configuring Axis settings (Look and style; placement and number of tick marks)
	//////////////////////////////////

	//plot->axisTop()->setTickPen(QPen(QBrush(QColor(Qt::yellow)), 0));
	//plot->axisBottom()->setTickPen(QPen(QBrush(QColor(Qt::red)), 0));
	//plot->axisLeft()->setTickPen(QPen(QBrush(QColor(Qt::green)), 0));
	//plot->axisRight()->setTickPen(QPen(QBrush(QColor(Qt::blue)), 0));
	plot->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:
	// plot->axisRight()->showTickLabels(false);
	// plot->axisRight()->setAxisName("y_right");
	// plot->axisRight()->showAxisName(true);

	plot->axisBottom()->setAxisName("eV");
	plot->axisLeft()->setAxisName("Intensity (arb. units)");


	// Change the margins: (in % of the plot width/height)
	plot->setMarginTop(5);
	plot->setMarginRight(5);
	plot->setMarginLeft(15);
	plot->setMarginBottom(15);


	// Show(hide) an axis completely
	//plot->axisRight()->setVisible(true);

	// Show ticks but not value labels:
	//plot->axisRight()->showTickLabels(false);

	// Disable tick marks completely:
	plot->axisTop()->setTicks(0);
	plot->axisTop()->setAxisName("time (s)");
	plot->axisTop()->showAxisName();

   // plot->axisTop()->showTickLabels(true);


	/*
	MPlotSeriesBasic *series1;
//		for(int y = 0; y < curScan_->numChannels(); y++){
	int maxChannels = pScan_()->numChannels();
	for(int y = 2; y < maxChannels; y++){
		series1 = new MPlotSeriesBasic();

		qDebug() << "Plotting " << pScan_()->channel(y)->name();
		series1->setModel(pScan_()->channel(y));
		plot->addItem(series1);
	}
	*/

	// 13: Adding 2d data and Image plots:
//	MPlotSimpleImageData *data2d = new MPlotSimpleImageData(QRectF(-0.5,-0.5,1,1), QSize(1024,1024));
//	MPlotSimpleImageData *data2d = new MPlotSimpleImageData(QRectF(950,0,10,1024), QSize(6,1024));
	qDebug() << "\n\nFirst x " << pScan_()->channel("eV")->value(0) << " last x " << pScan_()->channel("eV")->value(pScan_()->count()-1) << "\n\n";
	MPlotSimpleImageData *data2d = new MPlotSimpleImageData(QRectF(pScan_()->channel("eV")->value(0),0,pScan_()->channel("eV")->value(pScan_()->count()-1),1024), QSize(pScan_()->count(),1024));
	QString chRef = "";
//	for(int yy=0; yy<1024; yy++) {
	qDebug() << "Looping for yy " << 1024 << " by xx " << pScan_()->count();
	for(int yy=0; yy<1024; yy++) {
		chRef.setNum(yy);
		chRef.prepend("PGT_COUNTS");
		for(int xx=0; xx<pScan_()->count(); xx++) {
//			double x = data2d->x(pScan_()->channel("eV")->value(xx));
			double x = data2d->x(xx);
			double y = data2d->y(yy);
			//double r2 = x*x + y*y;
			//data2d.setZ(exp(-r2/0.1), xx, yy);
//			data2d->setZ(sin(x*4*M_PI)*sin(y*2*M_PI), xx, yy);
			data2d->setZ(pScan_()->channel(chRef)->value(xx), xx, yy);
		}
	}

	MPlotImageBasic *plot2d = new MPlotImageBasic(data2d);
	plot->addItem(plot2d);

	// 2. (continued) Axis / Axis Scale Settings
	///////////////////////

	// How much scale padding to add around data (in percent)
	plot->setScalePadding(5);	// set axis scale padding in percent

	// Manual axis range:
//	plot->setXDataRange(-1.5, 1.5);		// Manually set the axis range
//	plot->setYDataRangeLeft(-0.5, 0.5);
//	plot->setXDataRange(940, 970);		// Manually set the axis range
//	plot->setYDataRangeLeft(0, 1024);
	plot->setXDataRange(pScan_()->channel("eV")->value(0) - 10, pScan_()->channel("eV")->value(pScan_()->count()-1) + 10);		// Manually set the axis range
	plot->setYDataRangeLeft(0, 1024);

	// To auto-scale once only (using the current data):
	// plot->setXDataRange(0, 0, true);
	// plot->setYDataRangeLeft(0, 0, true);

	// Auto-scale always (ie: rescale as new data arrives)
	plot->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);

	/*
	// 7. Testing adding points to the series after the series is created.
	//////////////////////////////////////
	data2.insertPointBack(0, 0);
	data2.insertPointBack(0.2, 0);
	data2.insertPointFront(0, 0.2);
	//data2.removePointBack();
   // data2.removePointFront();
	*/

	// For real-time data: most optimized for memory consumption if you always do "insertPointBack()" and "removePointFront()".


/*
	// 8. Data-point editor. Standard QTableViews can also act as views on the MPlotSeriesData model.
	//////////////////////////////
	QTableView editor;
	editor.setModel(&data1);


	// 9. Display UI:
	//////////////////////
	editor.resize(300, 400);
	editor.show();
	*/

	plotWindow->resize(400, 300);
	plotWindow->show();

	/*
   // 5. (continued) More fun with marker shapes... Testing changes after a plot is created:
   //////////////////
   series1.setMarker(MPlotMarkerShape::Cross, 12, QPen(QColor(Qt::yellow), 0));


   // 10: single markers:
   MPlotPoint p1;
   p1.setValue(QPointF(0.5,0.5));
   p1.setMarker(MPlotMarkerShape::CrossCircle, 24, QPen(QColor(Qt::red)));

   plot->addItem(&p1);
*/

	// 11. Enable, disable, and selection?
	/////////////////////////////
   // series2.setEnabled(true);
   // series2.setSelected(true);

	// 10. Printing:
	////////////////////


/*
	QPrinter printer;
	printer.setOrientation(QPrinter::Landscape);
	if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
		QPainter painter(&printer);
		painter.setRenderHint(QPainter::Antialiasing);
		plotWindow.scene()->render(&painter);
	} // Print this to a PDF to see vector-graphics export.  Wow that was easy!
*/

	// PNG export:
/*
   QPixmap pixmap(800, 600);
   QPainter painter(&pixmap);
   painter.setRenderHint(QPainter::Antialiasing);
   plotWindow.scene()->render(&painter);
   painter.end();

   pixmap.save("/Users/mboots/Desktop/scene.png");
*/


	// Try out openGl viewport: (instead of CoreGraphics on Mac OS X)
	//plotWindow.setViewport(new QGLWidget);

   // 12: Testing plot tools: MPlotPlotSelectorTool:

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
