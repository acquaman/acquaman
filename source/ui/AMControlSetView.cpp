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
//	for(int x = 0; x < viewSet_->controls().count(); x++){
	for(int x = 0; x < viewSet_->count(); x++){
		tmpCtrl = viewSet_->controlAt(x);
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
		controlBoxes_.append(tmpASB);
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(fl);
	setLayout(hl_);
	setFixedSize(517, 200);
}

AMControlOptimizationSetView::AMControlOptimizationSetView(AMControlOptimizationSet *viewSet, QWidget *parent) :
		AMControlSetView((AMControlSet*)viewSet, parent)
{
	firstTime = true;
	MPlotWidget *plotWindow = new MPlotWidget();
//	numPoints = 25;
	numPoints = 50;
	//MPlot *plot = new MPlot();
	plot = new MPlot();
	plotWindow->setPlot(plot);
	plot->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:
	plot->axisBottom()->setTicks(3, MPlotAxis::Inside, 2);
	plot->axisBottom()->setAxisName("eV");
	plot->axisLeft()->setAxisName("Flux (arb. units)");
	plot->setMarginTop(5);
	plot->setMarginRight(5);
	plot->setMarginLeft(15);
	plot->setMarginBottom(25);
	data1 = new MPlotRealtimeModel();
	data2 = new MPlotRealtimeModel();
	MPlotSeriesBasic *series1 = new MPlotSeriesBasic();
	MPlotSeriesBasic *series2 = new MPlotSeriesBasic();
	series1->setName("series1");
	series2->setName("series2");
	series1->setModel(data1);
	series2->setModel(data2);
	QPen redSkinny(QBrush(QColor(Qt::red)), 1);	// red, 1pts wide
	QPen greenFat(QBrush(QColor(Qt::green)), 2);
	QPen pinkSkinny(QBrush(QColor(Qt::magenta)), 0);
	series1->setLinePen( redSkinny);	// set the pen for drawing the series
	series2->setLinePen( greenFat );
	series2->setMarker(MPlotMarkerShape::None);
	plot->addItem(series1);
	plot->addItem(series2);
	plot->setScalePadding(5);	// set axis scale padding in percent
	plot->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);

	plotWindow->resize(600, 450);
   series1->setMarker(MPlotMarkerShape::Cross, 24, QPen(QColor(Qt::yellow), 0)) ;
   MPlotDragZoomerTool *dzTool = new MPlotDragZoomerTool();
   plot->addTool(dzTool);
   MPlotPlotSelectorTool *psTool = new MPlotPlotSelectorTool;
   plot->addTool(psTool);

   MPlotWidget *plotWindow2 = new MPlotWidget();
   //MPlot *plot = new MPlot();
   plot2 = new MPlot();
   plotWindow2->setPlot(plot2);
   plot2->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:
   plot2->axisBottom()->setTicks(3, MPlotAxis::Inside, 2);
   plot2->axisBottom()->setAxisName("eV");
   plot2->axisLeft()->setAxisName("Resolution (arb. units)");
   plot2->setMarginTop(5);
   plot2->setMarginRight(5);
   plot2->setMarginLeft(15);
   plot2->setMarginBottom(25);
   plot2->setScalePadding(5);	// set axis scale padding in percent
   plot2->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
   plotWindow2->resize(600, 450);
   MPlotDragZoomerTool *dzTool2 = new MPlotDragZoomerTool();
   plot2->addTool(dzTool2);
   MPlotPlotSelectorTool *psTool2 = new MPlotPlotSelectorTool;
   plot2->addTool(psTool2);

   MPlotWidget *plotWindow3 = new MPlotWidget();
   //MPlot *plot = new MPlot();
   plot3 = new MPlot();
   plotWindow3->setPlot(plot3);
   plot3->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:
   plot3->axisBottom()->setTicks(3, MPlotAxis::Inside, 2);
   plot3->axisBottom()->setAxisName("eV");
   plot3->axisLeft()->setAxisName("Flux curve (arb. units)");
   plot3->setMarginTop(5);
   plot3->setMarginRight(5);
   plot3->setMarginLeft(15);
   plot3->setMarginBottom(25);
   plot3->setScalePadding(5);	// set axis scale padding in percent
   plot3->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
   plotWindow3->resize(600, 450);
   MPlotDragZoomerTool *dzTool3 = new MPlotDragZoomerTool();
   plot3->addTool(dzTool3);
   MPlotPlotSelectorTool *psTool3 = new MPlotPlotSelectorTool;
   plot3->addTool(psTool3);

   MPlotWidget *plotWindow4 = new MPlotWidget();
   //MPlot *plot = new MPlot();
   plot4 = new MPlot();
   plotWindow4->setPlot(plot4);
   plot4->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:
   plot4->axisBottom()->setTicks(3, MPlotAxis::Inside, 2);
   plot4->axisBottom()->setAxisName("eV");
   plot4->axisLeft()->setAxisName("Resolution curve (arb. units)");
   plot4->setMarginTop(5);
   plot4->setMarginRight(5);
   plot4->setMarginLeft(15);
   plot4->setMarginBottom(25);
   plot4->setScalePadding(5);	// set axis scale padding in percent
   plot4->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
   plotWindow4->resize(600, 450);
   MPlotDragZoomerTool *dzTool4 = new MPlotDragZoomerTool();
   plot4->addTool(dzTool4);
   MPlotPlotSelectorTool *psTool4 = new MPlotPlotSelectorTool;
   plot4->addTool(psTool4);

   MPlotWidget *plotWindow5 = new MPlotWidget();
   //MPlot *plot = new MPlot();
   plot5 = new MPlot();
   plotWindow5->setPlot(plot5);
   plot5->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:
   plot5->axisBottom()->setTicks(3, MPlotAxis::Inside, 2);
   plot5->axisBottom()->setAxisName("Resolution");
   plot5->axisLeft()->setAxisName("Flux");
   plot5->setMarginTop(5);
   plot5->setMarginRight(5);
   plot5->setMarginLeft(15);
   plot5->setMarginBottom(25);
   plot5->setScalePadding(5);	// set axis scale padding in percent
   plot5->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
   plotWindow5->resize(600, 450);
   MPlotDragZoomerTool *dzTool5 = new MPlotDragZoomerTool();
   plot5->addTool(dzTool5);
   MPlotPlotSelectorTool *psTool5 = new MPlotPlotSelectorTool;
   plot5->addTool(psTool5);

   MPlotRealtimeModel *tmpModel;
   AMQuickDataSet *tmpDataSet;
   QMap<double, double> fkData;
   fkData.insert(1.0, 1.0);
   for(int x = 0; x < numPoints; x++){
	   tmpModel = new MPlotRealtimeModel(this);
	   mFluxL1.append(tmpModel);
	   /*d*/
	   tmpModel = new MPlotRealtimeModel(this);
	   mFluxM1.append(tmpModel);
	   tmpModel = new MPlotRealtimeModel(this);
	   mFluxH1.append(tmpModel);
	   tmpModel = new MPlotRealtimeModel(this);
	   mFluxH3.append(tmpModel);
	   tmpModel = new MPlotRealtimeModel(this);
	   mResL1.append(tmpModel);
	   tmpModel = new MPlotRealtimeModel(this);
	   mResM1.append(tmpModel);
	   tmpModel = new MPlotRealtimeModel(this);
	   mResH1.append(tmpModel);
	   tmpModel = new MPlotRealtimeModel(this);
	   mResH3.append(tmpModel);
	   /*d*/

	   tmpDataSet = new AMQuickDataSet(fkData, this);
	   tmpDataSet->setModel(mFluxL1.at(x));
	   fluxL1.append(tmpDataSet);
	   /*d*/
	   tmpDataSet = new AMQuickDataSet(fkData, this);
	   tmpDataSet->setModel(mFluxM1.at(x));
	   fluxM1.append(tmpDataSet);
	   tmpDataSet = new AMQuickDataSet(fkData, this);
	   tmpDataSet->setModel(mFluxH1.at(x));
	   fluxH1.append(tmpDataSet);
	   tmpDataSet = new AMQuickDataSet(fkData, this);
	   tmpDataSet->setModel(mFluxH3.at(x));
	   fluxH3.append(tmpDataSet);
	   tmpDataSet = new AMQuickDataSet(fkData, this);
	   tmpDataSet->setModel(mResL1.at(x));
	   resL1.append(tmpDataSet);
	   tmpDataSet = new AMQuickDataSet(fkData, this);
	   tmpDataSet->setModel(mResM1.at(x));
	   resM1.append(tmpDataSet);
	   tmpDataSet = new AMQuickDataSet(fkData, this);
	   tmpDataSet->setModel(mResH1.at(x));
	   resH1.append(tmpDataSet);
	   tmpDataSet = new AMQuickDataSet(fkData, this);
	   tmpDataSet->setModel(mResH3.at(x));
	   resH3.append(tmpDataSet);
	   /*d*/
   }

   amFluxL1 = new MPlotRealtimeModel(this);
   aFluxL1 = new AMQuickDataSet(fkData, this);
   aFluxL1->setModel(amFluxL1);

   amFluxM1 = new MPlotRealtimeModel(this);
   aFluxM1 = new AMQuickDataSet(fkData, this);
   aFluxM1->setModel(amFluxM1);

   amFluxH1 = new MPlotRealtimeModel(this);
   aFluxH1 = new AMQuickDataSet(fkData, this);
   aFluxH1->setModel(amFluxH1);

   amFluxH3 = new MPlotRealtimeModel(this);
   aFluxH3 = new AMQuickDataSet(fkData, this);
   aFluxH3->setModel(amFluxH3);

   amResL1 = new MPlotRealtimeModel(this);
   aResL1 = new AMQuickDataSet(fkData, this);
   aResL1->setModel(amResL1);

   amResM1 = new MPlotRealtimeModel(this);
   aResM1 = new AMQuickDataSet(fkData, this);
   aResM1->setModel(amResM1);

   amResH1 = new MPlotRealtimeModel(this);
   aResH1 = new AMQuickDataSet(fkData, this);
   aResH1->setModel(amResH1);

   amResH3 = new MPlotRealtimeModel(this);
   aResH3 = new AMQuickDataSet(fkData, this);
   aResH3->setModel(amResH3);

   mLEG = new MPlotRealtimeModel(this);
   LEG = new AMQuickDataSet(fkData, this);
   LEG->setModel(mLEG);

   mMEG = new MPlotRealtimeModel(this);
   MEG = new AMQuickDataSet(fkData, this);
   MEG->setModel(mMEG);

   mHEG1 = new MPlotRealtimeModel(this);
   HEG1 = new AMQuickDataSet(fkData, this);
   HEG1->setModel(mHEG1);

   mHEG3 = new MPlotRealtimeModel(this);
   HEG3 = new AMQuickDataSet(fkData, this);
   HEG3->setModel(mHEG3);

   MPlotSeriesBasic *tmpSeries;
   for(int x = 0; x < numPoints; x++){
	   tmpSeries = new MPlotSeriesBasic();
	   tmpSeries->setModel(mFluxL1.at(x));
	   QPen tmpPen(QBrush(QColor(x*(250.0/numPoints), 127, 127)), 2);
	   tmpSeries->setLinePen(tmpPen);
	   plot->addItem(tmpSeries);
	   /*d*/
	   tmpSeries = new MPlotSeriesBasic();
	   tmpSeries->setModel(mFluxM1.at(x));
	   QPen tmpPen2(QBrush(QColor(127, x*(250.0/numPoints), 127)), 2);
	   tmpSeries->setLinePen(tmpPen2);
	   plot->addItem(tmpSeries);

	   tmpSeries = new MPlotSeriesBasic();
	   tmpSeries->setModel(mFluxH1.at(x));
	   QPen tmpPen3(QBrush(QColor(127, 127, x*(250.0/numPoints))), 2);
	   tmpSeries->setLinePen(tmpPen3);
	   plot->addItem(tmpSeries);

	   tmpSeries = new MPlotSeriesBasic();
	   tmpSeries->setModel(mFluxH3.at(x));
	   QPen tmpPen4(QBrush(QColor(255-x*(250.0/numPoints), 127, 127)), 2);
	   tmpSeries->setLinePen(tmpPen4);
	   plot->addItem(tmpSeries);

	   tmpSeries = new MPlotSeriesBasic();
	   tmpSeries->setModel(mResL1.at(x));
	   QPen tmpPen5(QBrush(QColor(x*(250.0/numPoints), 127, 127)), 2);
	   tmpSeries->setLinePen(tmpPen5);
	   plot2->addItem(tmpSeries);

	   tmpSeries = new MPlotSeriesBasic();
	   tmpSeries->setModel(mResM1.at(x));
	   QPen tmpPen6(QBrush(QColor(127, x*(250.0/numPoints), 127)), 2);
	   tmpSeries->setLinePen(tmpPen6);
	   plot2->addItem(tmpSeries);

	   tmpSeries = new MPlotSeriesBasic();
	   tmpSeries->setModel(mResH1.at(x));
	   QPen tmpPen7(QBrush(QColor(127, 127, x*(250.0/numPoints))), 2);
	   tmpSeries->setLinePen(tmpPen7);
	   plot2->addItem(tmpSeries);

	   tmpSeries = new MPlotSeriesBasic();
	   tmpSeries->setModel(mResH3.at(x));
	   QPen tmpPen8(QBrush(QColor(255-x*(250.0/numPoints), 127, 127)), 2);
	   tmpSeries->setLinePen(tmpPen8);
	   plot2->addItem(tmpSeries);
	   /*d*/
   }

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(amFluxL1);
   QPen tmpPen9(QBrush(QColor(255, 0, 0)), 2);
   tmpSeries->setLinePen(tmpPen9);
   plot3->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(amFluxM1);
   QPen tmpPen10(QBrush(QColor(0, 255, 0)), 2);
   tmpSeries->setLinePen(tmpPen10);
   plot3->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(amFluxH1);
   QPen tmpPen11(QBrush(QColor(0, 0, 255)), 2);
   tmpSeries->setLinePen(tmpPen11);
   plot3->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(amFluxH3);
   QPen tmpPen12(QBrush(QColor(255, 255, 0)), 2);
   tmpSeries->setLinePen(tmpPen12);
   plot3->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(amResL1);
   QPen tmpPen14(QBrush(QColor(255, 0, 0)), 2);
   tmpSeries->setLinePen(tmpPen14);
   plot4->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(amResM1);
   QPen tmpPen15(QBrush(QColor(0, 255, 0)), 2);
   tmpSeries->setLinePen(tmpPen15);
   plot4->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(amResH1);
   QPen tmpPen16(QBrush(QColor(0, 0, 255)), 2);
   tmpSeries->setLinePen(tmpPen16);
   plot4->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(amResH3);
   QPen tmpPen17(QBrush(QColor(255, 255, 0)), 2);
   tmpSeries->setLinePen(tmpPen17);
   plot4->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(mLEG);
   QPen tmpPen18(QBrush(QColor(255, 0, 0)), 2);
   tmpSeries->setLinePen(tmpPen18);
   plot5->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(mMEG);
   QPen tmpPen19(QBrush(QColor(0, 255, 0)), 2);
   tmpSeries->setLinePen(tmpPen19);
   plot5->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(mHEG1);
   QPen tmpPen20(QBrush(QColor(0, 0, 255)), 2);
   tmpSeries->setLinePen(tmpPen20);
   plot5->addItem(tmpSeries);

   tmpSeries = new MPlotSeriesBasic();
   tmpSeries->setModel(mHEG3);
   QPen tmpPen21(QBrush(QColor(0, 255, 255)), 2);
   tmpSeries->setLinePen(tmpPen21);
   plot5->addItem(tmpSeries);

//	plotWindow->setFixedSize(267, 200);
//	hl_->addWidget(plotWindow);
   plotWindow->show();
	setFixedSize(517, 200);
	//	plotWindow->setFixedSize(267, 200);
	//	hl_->addWidget(plotWindow);
	plotWindow2->show();
	plotWindow3->show();
	plotWindow4->show();
	plotWindow5->show();
}


AMCompactControlOptimizationSetView::AMCompactControlOptimizationSetView(AMControlSet *viewSet, QWidget *parent) :
		QGroupBox(parent)
{
	int width = 350;
	setFixedSize(width, 150);
	viewSet_ = (AMControlOptimizationSet*)viewSet;
	setTitle(viewSet->name());
	launchDetailButton_ = new QPushButton("More Details", this);
//	QFormLayout *fl = new QFormLayout();
//	fl->addRow("", launchDetailButton_);
	hl_ = new QHBoxLayout(this);
	QSlider *tmpSlider;
	AMControlOptimization *tmpOpt;
	if(viewSet_->optimizationCount() == 2){
		QGridLayout *gl = new QGridLayout(this);
		maxUpper = 1.24e12;
		minUpper = 8.41e9;
		maxLower = 5200;
		minLower = 1250;
		QString numStr;
		QLabel *leftTopLabel = new QLabel("(+ "+viewSet_->optimizationAt(0)->description()+")\n"+numStr.setNum(maxUpper));
		QLabel *rightTopLabel = new QLabel("(- "+viewSet_->optimizationAt(0)->description()+")\n"+numStr.setNum(minUpper));
		rightTopLabel->setAlignment(Qt::AlignRight);
		QLabel *leftBottomLabel = new QLabel(numStr.setNum(minLower)+"\n(- "+viewSet_->optimizationAt(1)->description()+")");
		QLabel *rightBottomLabel = new QLabel(numStr.setNum(maxLower)+"\n(+ "+viewSet_->optimizationAt(1)->description()+")");
		rightBottomLabel->setAlignment(Qt::AlignRight);
		tmpSlider = new QSlider(Qt::Horizontal, this);
		tmpSlider->setRange(0, 100);
		tmpSlider->setMinimumWidth( (int)(width*0.9));
		optValue_ = new QLabel();
		connect(tmpSlider, SIGNAL(valueChanged(int)), this, SLOT(adjustSlider(int)));
		tmpSlider->setValue(50);
		gl->addWidget(leftTopLabel, 0, 0, Qt::AlignLeft);
		gl->addWidget(launchDetailButton_, 0, 1, Qt::AlignCenter);
		gl->addWidget(rightTopLabel, 0, 2, Qt::AlignRight);
		gl->addWidget(tmpSlider, 1, 0, 1, 3, Qt::AlignCenter);
		gl->addWidget(leftBottomLabel, 2, 0, Qt::AlignLeft);
		gl->addWidget(optValue_, 2, 1, Qt::AlignCenter);
		gl->addWidget(rightBottomLabel, 2, 2, Qt::AlignRight);
		hl_->addLayout(gl);
		/*
		QGridLayout *gl = new QGridLayout(this);
		QLabel *leftLabel = new QLabel("More "+viewSet_->optimizationAt(0)->description());
		QLabel *rightLabel = new QLabel("More "+viewSet_->optimizationAt(1)->description());
		tmpSlider = new QSlider(Qt::Horizontal, this);
		tmpSlider->setRange(0, 100);
		tmpSlider->setMinimumWidth( (int)(width*0.9));
		gl->addWidget(leftLabel, 0, 0, Qt::AlignLeft);
		gl->addWidget(launchDetailButton_, 0, 1, Qt::AlignCenter);
		gl->addWidget(rightLabel, 0, 2, Qt::AlignRight);
		gl->addWidget(tmpSlider, 1, 0, 1, 3, Qt::AlignCenter);
		hl_->addLayout(gl);
		*/
	}
	else{
		for(int x = 0; x < viewSet_->optimizationCount(); x++){
			tmpOpt = (AMControlOptimization*)viewSet_->optimizationAt(x);
			qDebug() << "Has " << tmpOpt->name();

			/*
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
		controlBoxes_.append(tmpASB);
		*/
		}
	}

//	hl_ = new QHBoxLayout(this);
//	hl_->addLayout(fl);
	setLayout(hl_);

}

bool AMCompactControlOptimizationSetView::adjustSlider(int val){
	double upperDiff, lowerDiff, newUpper, newLower;
	upperDiff = maxUpper - minUpper;
	lowerDiff = maxLower - minLower;
	newUpper = (100-val)*upperDiff/100 + minUpper;
	newLower = val*lowerDiff/100 + minLower;
	QString upperStr, lowerStr;
	upperStr.setNum(newUpper, 'g', 3);
	lowerStr.setNum(newLower);
	optValue_->setText(upperStr+", "+lowerStr);
}

AMAbstractDetectorSetView::AMAbstractDetectorSetView(AMAbstractDetectorSet *viewSet, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	setTitle(viewSet->name());
	QFormLayout *fl = new QFormLayout();
	QCheckBox *tmpBox;
	AMAbstractDetector *tmpDetector;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpDetector = viewSet_->detectorAt(x);
		tmpBox = new QCheckBox(this);
//		tmpBox->setChecked(viewSet_->defaultDetectors().at(x));
		tmpBox->setChecked(viewSet_->isDefaultAt(x));
		fl->addRow(tmpDetector->name(), tmpBox);
		detectorBoxes_.append(tmpBox);
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(fl);
	setLayout(hl_);
	setFixedSize(517, 200);
}






AMColorControlOptimizationSetView::AMColorControlOptimizationSetView(AMControlSet *viewSet, QWidget *parent) :
		QGroupBox(parent)
{
	int width = 350;
	setFixedSize(width, 150);
	viewSet_ = (AMControlOptimizationSet*)viewSet;
	setTitle(viewSet->name());
//	setStyleSheet("QGroupBox::title { color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 green, stop: 1 blue); }");
	setStyleSheet("QGroupBox::title { color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 rgb(0, 255, 0), stop: 0.33 rbg(0, 127, 127), stop: 1 rgb(0, 0, 255)); }");
//	setStyleSheet("QGroupBox::title { color: qconicalgradient(cx: 0.33, cy: 0.5, angle:0, stop: 0 rgb(0, 255, 0), stop: 1 rgb(0, 0, 255)); }");
	launchDetailButton_ = new QPushButton("More Details", this);
	hl_ = new QHBoxLayout(this);
	QSlider *tmpSlider;
	AMControlOptimization *tmpOpt;
	if(viewSet_->optimizationCount() == 2){
		QGridLayout *gl = new QGridLayout(this);
		maxUpper = 1.24e12;
		minUpper = 8.41e9;
		maxLower = 5200;
		minLower = 1250;
		QString numStr;
		QLabel *leftTopLabel = new QLabel("(+ "+viewSet_->optimizationAt(0)->description()+")\n"+numStr.setNum(maxUpper));
		QLabel *rightTopLabel = new QLabel("(- "+viewSet_->optimizationAt(0)->description()+")\n"+numStr.setNum(minUpper));
		rightTopLabel->setAlignment(Qt::AlignRight);
		QLabel *leftBottomLabel = new QLabel(numStr.setNum(minLower)+"\n(- "+viewSet_->optimizationAt(1)->description()+")");
		QLabel *rightBottomLabel = new QLabel(numStr.setNum(maxLower)+"\n(+ "+viewSet_->optimizationAt(1)->description()+")");
		rightBottomLabel->setAlignment(Qt::AlignRight);
		tmpSlider = new QSlider(Qt::Horizontal, this);
		tmpSlider->setRange(0, 100);
		tmpSlider->setMinimumWidth( (int)(width*0.9));
		optValue_ = new QLabel();
		connect(tmpSlider, SIGNAL(valueChanged(int)), this, SLOT(adjustSlider(int)));
		tmpSlider->setValue(50);
		gl->addWidget(leftTopLabel, 0, 0, Qt::AlignLeft);
		gl->addWidget(launchDetailButton_, 0, 1, Qt::AlignCenter);
		gl->addWidget(rightTopLabel, 0, 2, Qt::AlignRight);
		gl->addWidget(tmpSlider, 1, 0, 1, 3, Qt::AlignCenter);
		gl->addWidget(leftBottomLabel, 2, 0, Qt::AlignLeft);
		gl->addWidget(optValue_, 2, 1, Qt::AlignCenter);
		gl->addWidget(rightBottomLabel, 2, 2, Qt::AlignRight);
		hl_->addLayout(gl);
	}
	else{
		for(int x = 0; x < viewSet_->optimizationCount(); x++){
			tmpOpt = (AMControlOptimization*)viewSet_->optimizationAt(x);
			qDebug() << "Has " << tmpOpt->name();
		}
	}
	setLayout(hl_);

}

bool AMColorControlOptimizationSetView::adjustSlider(int val){
	double upperDiff, lowerDiff, newUpper, newLower;
	upperDiff = maxUpper - minUpper;
	lowerDiff = maxLower - minLower;
	newUpper = (100-val)*upperDiff/100 + minUpper;
	newLower = val*lowerDiff/100 + minLower;
	QString upperStr, lowerStr;
	upperStr.setNum(newUpper, 'g', 3);
	lowerStr.setNum(newLower);
	optValue_->setText(upperStr+", "+lowerStr);
}
