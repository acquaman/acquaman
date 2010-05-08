#include "AMControlSetView.h"

/// Sets the title of the group box based on the name() function of the AMControlSet.
/// Loops through the list of AMControls in the AMControlSet and create an appropriate spinbox.
/// Adds the spin box and a label (from the AMControl objectName() function) and add to an internal form layout.
AMControlSetView::AMControlSetView(AMControlSet *viewSet, QWidget *parent) :
	QGroupBox(parent)
{
	viewSet_ = viewSet;
	setTitle(viewSet->name());
	QFormLayout *fl = new QFormLayout();
	QAbstractSpinBox *tmpASB;
	AMControl *tmpCtrl;
	for(int x = 0; x < viewSet_->controls().count(); x++){
		tmpCtrl = viewSet_->controls().at(x);
		if(tmpCtrl->isEnum()){
			tmpASB = new QSpinBox(this);
			((QSpinBox*)tmpASB)->setValue(tmpCtrl->value());
			((QSpinBox*)tmpASB)->setMaximum(tmpCtrl->maximumValue());
			((QSpinBox*)tmpASB)->setMinimum(tmpCtrl->minimumValue());
		}
		else{
			tmpASB = new QDoubleSpinBox(this);
			((QDoubleSpinBox*)tmpASB)->setValue(tmpCtrl->value());
			((QDoubleSpinBox*)tmpASB)->setMaximum(tmpCtrl->maximumValue());
			((QDoubleSpinBox*)tmpASB)->setMinimum(tmpCtrl->minimumValue());
		}
		fl->addRow(tmpCtrl->objectName(), tmpASB);
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(fl);
	setLayout(hl_);
	setFixedSize(517, 200);
}

AMControlOptimizationSetView::AMControlOptimizationSetView(AMControlOptimizationSet *viewSet, QWidget *parent) :
		AMControlSetView((AMControlSet*)viewSet, parent)
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
	plot->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:
	plot->axisBottom()->setTicks(3, MPlotAxis::Inside, 2);
	plot->axisBottom()->setAxisName("eV");
	plot->axisLeft()->setAxisName("Flux (arb. units)");
	// Change the margins: (in % of the plot width/height)
	plot->setMarginTop(5);
	plot->setMarginRight(5);
	plot->setMarginLeft(15);
	plot->setMarginBottom(25);
	// Disable tick marks completely:
//	plot->axisTop()->setTicks(0);
//	plot->axisTop()->setAxisName("Energy (eV)");
//	plot->axisTop()->showAxisName();

	// 13: Adding 2d data and Image plots:
//	MPlotSimpleImageData *data2d = new MPlotSimpleImageData(QRectF(-0.5,-0.5,1,1), QSize(1024,1024));
//	for(int yy=0; yy<1024; yy++) {
//		for(int xx=0; xx<1024; xx++) {
//			double x = data2d->x(xx);
//			double y = data2d->y(yy);
			//double r2 = x*x + y*y;
			//data2d->setZ(exp(-r2/0.1), xx, yy);
//			data2d->setZ(sin(x*4*M_PI)*sin(y*2*M_PI), xx, yy);
//		}
//	}
//	MPlotImageBasic *plot2d = new MPlotImageBasic(data2d);
//	plot->addItem(plot2d);
	// 3. Add data. Data is contained in the first two columns of an MPlotSeriesData:
	//////////////////////////////
	data1 = new MPlotRealtimeModel();
	data2 = new MPlotRealtimeModel();
//	MPlotRealtimeModel *data1 = new MPlotRealtimeModel();
//	MPlotRealtimeModel *data2 = new MPlotRealtimeModel();
	// These class wrap MPlotRealtimeModel so that they can be used as plot series data.
	MPlotRealtimeModelSeriesData *sdata1 = new MPlotRealtimeModelSeriesData(*data1);
	MPlotRealtimeModelSeriesData *sdata2 = new MPlotRealtimeModelSeriesData(*data2);

	/*
	QList<AMRegion*> contextParams;
	AMRegion *myRegion = new AMRegion();
	myRegion->setStart(500);
	myRegion->setDelta(1);
	myRegion->setEnd(700);
	contextParams << myRegion;
	onRegionsUpdate(contextParams);
	*/

	/*
	QList<QVariant> stateParams, stateParams2;
	stateParams << 250.0 << 0 << 1;
	stateParams2 << 250.0 << 1 << 1;
	QList<AMRegion*> contextParams;
	AMRegion *myRegion = new AMRegion();
	myRegion->setStart(500);
	myRegion->setDelta(1);
	myRegion->setEnd(700);
	contextParams << myRegion;
	QMap<double, double> map1 = ((AMControlOptimizationSet*)viewSet_)->curveAt(0, stateParams, contextParams);
	QMap<double, double> map2 = ((AMControlOptimizationSet*)viewSet_)->curveAt(0, stateParams2, contextParams);

	qDebug() << "Count is " << map1.count();
	QMap<double, double>::const_iterator i = map1.constBegin();
	while (i != map1.constEnd()) {
		qDebug() << i.key() << " " << i.value();
		data1->insertPointBack(i.key(), i.value());
		++i;
	}

	i = map2.constBegin();
	while (i != map2.constEnd()) {
		qDebug() << i.key() << " " << i.value();
		data2->insertPointBack(i.key(), i.value());
		++i;
	}
	*/

	// 4.  View the data.  A basic scatter/line plot is an MPlotSeries:
	////////////////////////////////////////////////////
	MPlotSeriesBasic *series1 = new MPlotSeriesBasic();
	MPlotSeriesBasic *series2 = new MPlotSeriesBasic();
	series1->setObjectName("series1");
	series2->setObjectName("series2");
	// Enable to plot on the right axis instead of the left axis
	// series1->setYAxisTarget(MPlotAxis::Right);
	// connect this plot series as a view on its model (data1, data2)
	series1->setModel(sdata1);
	series2->setModel(sdata2);
	// 5. Configure look of the plots:
	//////////////////////////////////////
	QPen redSkinny(QBrush(QColor(Qt::red)), 1);	// red, 1pts wide
	QPen greenFat(QBrush(QColor(Qt::green)), 2);
	QPen pinkSkinny(QBrush(QColor(Qt::magenta)), 0);
	// Line style: set using pens.  (Can create dashed pen for dashed/dotted lines)
	series1->setLinePen( redSkinny);	// set the pen for drawing the series
	series2->setLinePen( greenFat );
	// Marker size and shape: (always set shape before size/pen/brush)
	 // series2->setMarkerShape(MPlotMarkerShape::StarCircle);
	series2->setMarker(MPlotMarkerShape::None);
//	series2->setMarkerShape(MPlotMarkerShape::None);
//	series2->setMarkerSize(12);
	// Can also configure the marker pen and brush:
//	series2->setMarkerPen(pinkSkinny);
//	series2->setMarkerBrush(QBrush(QColor(Qt::black)));
	// Gridlines:
	// plot->axisRight()->showGrid(true);
	// plot->axisRight()->setGridPen(greenFat);
	// 6. Adding a series to a plot:
	///////////////////////////////
	plot->addItem(series1);
	plot->addItem(series2);
	// 2. (continued) Axis / Axis Scale Settings
	///////////////////////
	// How much scale padding to add around data (in percent)
	plot->setScalePadding(5);	// set axis scale padding in percent
	// Manual axis range:
//	plot->setXDataRange(myRegion->start(), myRegion->end());		// Manually set the axis range
//	plot->setYDataRangeLeft(-0.5, 0.5);
	// To auto-scale once only (using the current data):
	// plot->setXDataRange(0, 0, true);
	// plot->setYDataRangeLeft(0, 0, true);
	// Auto-scale always (ie: rescale as new data arrives)
	plot->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
	// 7. Testing adding points to the series after the series is created.
	//////////////////////////////////////
//	data2->insertPointBack(0, 0);
//	data2->insertPointBack(0.2, 0);
//	data2->insertPointFront(0, 0.2);
	// 8. Data-point editor. Standard QTableViews can also act as views on the MPlotSeriesData model.
	//////////////////////////////
//		QTableView editor;
//		editor.setModel(&data1);
	// 9. Display UI:
	//////////////////////
//		editor.resize(300, 400);
//		editor.show();

	plotWindow->resize(400, 300);
//		plotWindow->show();
   // 5. (continued) More fun with marker shapes... Testing changes after a plot is created:
   //////////////////
   series1->setMarker(MPlotMarkerShape::Cross, 24, QPen(QColor(Qt::yellow), 0)) ;
//   series1->setMarkerSize(24);
//   series1->setMarkerPen(QPen(QColor(Qt::yellow), 0));
   // 12: Testing plot tools: MPlotPlotSelectorTool:
   // DragZoomerTools need to be added first ("on the bottom") so they don't steal everyone else's mouse events
   MPlotDragZoomerTool *dzTool = new MPlotDragZoomerTool();
   plot->addTool(dzTool);
   // dzTool.setEnabled(false);
   MPlotPlotSelectorTool *psTool = new MPlotPlotSelectorTool;
   plot->addTool(psTool);

	plotWindow->setFixedSize(267, 200);

	hl_->addWidget(plotWindow);
//	setLayout(hl);
	setFixedSize(517, 200);
}
