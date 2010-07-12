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
	MPlotWidget *plotWindow = new MPlotWidget();
	MPlot *plot = new MPlot();
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

	plotWindow->resize(400, 300);
   series1->setMarker(MPlotMarkerShape::Cross, 24, QPen(QColor(Qt::yellow), 0)) ;
   MPlotDragZoomerTool *dzTool = new MPlotDragZoomerTool();
   plot->addTool(dzTool);
   MPlotPlotSelectorTool *psTool = new MPlotPlotSelectorTool;
   plot->addTool(psTool);

	plotWindow->setFixedSize(267, 200);
	hl_->addWidget(plotWindow);
	setFixedSize(517, 200);
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
