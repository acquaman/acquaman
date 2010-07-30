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
	numPoints = 50;

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

	mANSWER = new MPlotRealtimeModel(this);
	ANSWER = new AMQuickDataSet(fkData, this);
	ANSWER->setModel(mANSWER);


	MPlotSeriesBasic *tmpSeries;

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

	tmpSeries = new MPlotSeriesBasic();
	tmpSeries->setModel(mANSWER);
	QPen tmpPen22(QBrush(QColor(255, 0, 255)), 2);
	tmpSeries->setLinePen(tmpPen22);
	plot5->addItem(tmpSeries);

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
	param1Trigger_= false;
	param2Trigger_ = false;
	hl_ = new QHBoxLayout(this);

	AMControlOptimization *tmpOpt;
	if(viewSet_->optimizationCount() == 2){
		gl_ = new QGridLayout();
		param1Slider = new QSlider(Qt::Horizontal, this);
		param1Slider->setRange(0, 100);
		param1Slider->setMinimumWidth( (int)(width*0.9));

		param2Slider = new QSlider(Qt::Horizontal, this);
		param2Slider->setRange(0, 100);
		param2Slider->setMinimumWidth( (int)(width*0.9));

		param1Scene = new QGraphicsScene(0, 0, (int)(width*0.9), 20);
		param1View = new QGraphicsView(param1Scene);
		param1View->setRenderHint(QPainter::Antialiasing);
		param1View->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
		param1View->setBackgroundBrush(QColor(230, 222, 214));
		param1View->setWindowTitle("Test Test");
		param1View->resize((int)(width*0.9), 20);
		param1View->setMaximumHeight(20);
		param1View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		param1View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		param2Scene = new QGraphicsScene(0, 0, (int)(width*0.9), 20);
		param2View = new QGraphicsView(param2Scene);
		param2View->setRenderHint(QPainter::Antialiasing);
		param2View->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
		param2View->setBackgroundBrush(QColor(230, 222, 214));
		param2View->setWindowTitle("Test Test");
		param2View->resize((int)(width*0.9), 20);
		param2View->setMaximumHeight(20);
		param2View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		param2View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		param1Curve_ = NULL;
		param1Item_ = new CCOSVItem((int)(width*0.9), 20, QColor("blue"), true, true);
		param1Scene->addItem(param1Item_);

		param2Curve_ = NULL;
		param2Item_ = new CCOSVItem((int)(width*0.9), 20, QColor("green"), false, true);
		param2Scene->addItem(param2Item_);

		paramsResult_ = new QLabel("", this);

		connect(param1Slider, SIGNAL(valueChanged(int)), this, SLOT(onParam1SliderUpdate(int)));
		connect(param2Slider, SIGNAL(valueChanged(int)), this, SLOT(onParam2SliderUpdate(int)));

		gl_->addWidget(paramsResult_, 0, 0, 1, 2, Qt::AlignCenter);
		gl_->addWidget(launchDetailButton_, 0, 2, 1, 1, Qt::AlignCenter);
		gl_->addWidget(param1View, 1, 0, 1, 3, Qt::AlignCenter);
		gl_->addWidget(param1Slider, 2, 0, 1, 3, Qt::AlignCenter);
		gl_->addWidget(param2Slider, 3, 0, 1, 3, Qt::AlignCenter);
		gl_->addWidget(param2View, 4, 0, 1, 3, Qt::AlignCenter);
		hl_->addLayout(gl_);
	}
	else{
		for(int x = 0; x < viewSet_->optimizationCount(); x++){
			tmpOpt = (AMControlOptimization*)viewSet_->optimizationAt(x);
			qDebug() << "Has " << tmpOpt->name();

		}
	}
	setLayout(hl_);
}

bool AMCompactControlOptimizationSetView::onParam1SliderUpdate(int val){
	if(!param1Curve_ || !param2Curve_)
		return false;
	if(param2Trigger_){
		param2Trigger_ = false;
		return true;
	}
	double param1Val = param1Curve_->valuesAtRange((double)val/100).first;
	double param2Val = param1Curve_->valuesAtRange((double)val/100).second;
	double otherPercent = param2Curve_->percentFromValue(param2Val)*100;
	QStringList metasList = param1Curve_->metaMap().lowerBound(param1Val).value();
	paramsResult_->setText(metasList.at(0)+" with "+metasList.at(1)+" um slits");
	qDebug() << "Wants percent " << val << " for output " << param1Val << param2Val << "on metas " << metasList << " wants other to be " << otherPercent;
	param1Trigger_ = true;
	param2Slider->setValue(otherPercent);
	return true;
}

bool AMCompactControlOptimizationSetView::onParam2SliderUpdate(int val){
	if(!param1Curve_ || !param2Curve_)
		return false;
	if(param1Trigger_){
		param1Trigger_ = false;
		return true;
	}
	qDebug() << "Wants percent " << val << " for output " << param2Curve_->valuesAtRange((double)val/100).first << param2Curve_->valuesAtRange((double)val/100).second;
	double param2Val = param2Curve_->valuesAtRange((double)val/100).first;
	double param1Val = param2Curve_->valuesAtRange((double)val/100).second;
	double otherPercent = param1Curve_->percentFromValue(param1Val)*100;
	QStringList metasList = param2Curve_->metaMap().lowerBound(param2Val).value();
	paramsResult_->setText(metasList.at(0)+" with "+metasList.at(1)+" um slits");
	qDebug() << "Wants percent " << val << " for output " << param2Val << param1Val << "on metas " << metasList << " wants other to be " << otherPercent;
	param2Trigger_ = true;
	param1Slider->setValue(otherPercent);
	return true;
}

void AMCompactControlOptimizationSetView::onRegionsUpdate(AMRegionsList* contextParams){
	if(param1Curve_)
		delete param1Curve_;
	if(param2Curve_)
		delete param2Curve_;
	param1Curve_ = ((AMControlOptimizationSet*)viewSet_)->cOnePlot(contextParams);
	param2Curve_ = param1Curve_->transposeCurve();
	param1Item_->updateCurve(param1Curve_);
	param2Item_->updateCurve(param2Curve_);
}

CCOSVItem::CCOSVItem(int width, int height, QColor curveColor, bool invert, bool log)
{
	curve_ = NULL;
	width_ = width;
	height_ = height;
	curveColor_ = curveColor;
	invert_ = invert;
	log_ = log;

	curve_ = new QPointF[width_+2];
	int top = 0;
	if(invert)
		top = height_;
	curve_[0] = QPointF(0, top);
	for(int x = 0; x < width_; x++){
		curve_[x+1] = QPointF(x, height_ - 0.5*height_*sin(0.25*x)+0.5*height_);
	}
	curve_[width_+1] = QPointF(width_, top);
	QMap<double, double> tmpMap;
	tmpMap[1] = 1;
	dataCurve_ = new AMCurve(tmpMap);
}

void CCOSVItem::updateCurve(AMCurve *dataCurve){
	if(!curve_){
		qDebug() << "No curve";
		return;
	}
	qDebug() << "Has curve";
	dataCurve_ = dataCurve;
	double min = dataCurve_->minY().first;
	double max = dataCurve_->maxY().first;
	double tmpVal;

	qDebug() << "Max/Min: " << max << min;
	for(int x = 0; x < width_; x++){
		if(log_)
			tmpVal = height_*( log(dataCurve_->valueAtRange((double)x/(double)width_)/min) / log(max/min) );
		else
			tmpVal = height_*( (dataCurve_->valueAtRange((double)x/(double)width_)-min) / (max-min) );
		if(invert_)
			curve_[x+1].setY( height_ - tmpVal );
		else
			curve_[x+1].setY( tmpVal );
	}
	update();
}

QRectF CCOSVItem::boundingRect() const{
	return QRectF(0, 0, width_, height_);
}

void CCOSVItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(option);
	Q_UNUSED(widget);
//	painter->setBrush(Qt::darkGray);
	painter->setBrush(curveColor_);
	painter->setPen(Qt::NoPen);
	if(curve_)
		painter->drawConvexPolygon(curve_, width_+2);
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
