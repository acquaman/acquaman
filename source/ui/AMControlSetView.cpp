#include "AMControlSetView.h"

/// Sets the title of the group box based on the name() function of the AMControlSet.
/// Loops through the list of AMControls in the AMControlSet and create an appropriate spinbox.
/// Adds the spin box and a label (from the AMControl objectName() function) and add to an internal form layout.
AMControlSetView::AMControlSetView(AMControlSet *viewSet, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	setTitle(viewSet->name());
	boxTrigger_ = false;
	QFormLayout *fl = new QFormLayout();
	QWidget *tmpWidget;
	QDoubleSpinBox *tmpDSB;
	QComboBox *tmpCB;
	AMControl *tmpCtrl;
	QString tmpName;
	QVariant tmpVal;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpCtrl = viewSet_->controlAt(x);
		tmpVal.clear();
		tmpName = tmpCtrl->name();
		if(tmpCtrl->isEnum()){
			tmpCB = new QComboBox(this);
			tmpCB->addItems(tmpCtrl->enumNames());
			tmpCB->setCurrentIndex((int)tmpCtrl->value());
			tmpVal = tmpCtrl->enumNames().at((int)tmpCtrl->value());
			connect(tmpCB, SIGNAL(currentIndexChanged(QString)), this, SLOT(onBoxUpdate(QString)));
			tmpWidget = tmpCB;
		}
		else{
			tmpDSB = new QDoubleSpinBox(this);
			tmpDSB->setValue(tmpCtrl->value());
			tmpDSB->setMaximum(tmpCtrl->maximumValue());
			tmpDSB->setMinimum(tmpCtrl->minimumValue());
			tmpVal = tmpCtrl->value();
			connect(tmpDSB, SIGNAL(valueChanged(QString)), this, SLOT(onBoxUpdate(QString)));
			tmpWidget = tmpDSB;
		}
		configValues_.insert(tmpName, tmpVal);
		QString adjName = tmpCtrl->objectName();
		adjName.replace(QRegExp("([A-Z])"), " \\1");
		QChar fCap = adjName[0].toUpper();
		adjName.replace(0, 1, fCap);
		fl->addRow(adjName, tmpWidget);
		controlBoxes_.append(tmpWidget);
		dirty_.append(false);
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(fl);
	setLayout(hl_);
	setFixedSize(300, 200);
}

void AMControlSetView::onBoxUpdate(const QString &value){
	Q_UNUSED(value);
	AMControl *tmpCtrl;
	QString tmpName;
	bool actualChange = false;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpCtrl = viewSet_->controlAt(x);
		tmpName = tmpCtrl->name();
		if(tmpCtrl->isEnum()){
			if( ((QComboBox*)(controlBoxes_.at(x)))->currentText() != configValues_[tmpName].toString() ){
				configValues_[tmpName] = ((QComboBox*)(controlBoxes_.at(x)))->currentText();
				if(dirty_.at(x)){
					dirty_[x] = false;
				}
				else
					actualChange = true;
			}
		}
		else{
			if( ((QDoubleSpinBox*)(controlBoxes_.at(x)))->value() != configValues_[tmpName].toDouble() ){
				configValues_[tmpName] = ((QDoubleSpinBox*)(controlBoxes_.at(x)))->value();
				if(dirty_.at(x)){
					dirty_[x] = false;
				}
				else
					actualChange = true;
			}
		}
	}
	if(actualChange){
		boxTrigger_ = true;
		emit configValuesChanged();
	}
}

void AMControlSetView::setConfigValues(QMap<QString, QVariant> configValues){
	QMap<QString, QVariant>::const_iterator i = configValues.constBegin();
	while(i != configValues.constEnd()){
		if( viewSet_->controlByName(i.key()) && configValues_.value(i.key()) != i.value() ){
			dirty_[viewSet_->indexOf(i.key())] = true;
			if( viewSet_->controlByName(i.key())->isEnum() && viewSet_->controlByName(i.key())->enumNames().contains(i.value().toString()) )
				((QComboBox*)(boxByName(i.key())))->setCurrentIndex(viewSet_->controlByName(i.key())->enumNames().indexOf(i.value().toString()));
			else if( !viewSet_->controlByName(i.key())->isEnum() )
				((QDoubleSpinBox*)(boxByName(i.key())))->setValue(i.value().toDouble());
		}
		++i;
	}
	emit configValuesChanged();
}

AMQuickDataSet::AMQuickDataSet(QMap<double, double> dataMap, QObject *parent) : QObject(parent){
	model_ = NULL;
	setDataMap(dataMap);
}

void AMQuickDataSet::setModel(MPlotRealtimeModel *model){
	model_ = model;
	setupModel();
}

void AMQuickDataSet::setupModel(){
	while (model_->count() > 0) {
		model_->removePointBack();
	}
	QMap<double, double>::const_iterator i = dataMap_.constBegin();
	while (i != dataMap_.constEnd()) {
		model_->insertPointBack(i.key(), i.value());
		++i;
	}
}

void AMQuickDataSet::setDataMap(QMap<double, double> dataMap){
	dataMap_ = dataMap;
	QMap<double, double>::const_iterator i = dataMap_.constBegin();
	minX_ = i.key();
	minY_ = i.value();
	maxX_ = i.key();
	maxY_ = i.value();
	double tmpX, tmpY;
	while(i != dataMap_.constEnd()){
		tmpX = i.key();
		tmpY = i.value();
		if(tmpX < minX_)
			minX_ = tmpX;
		else if(tmpX > maxX_)	\
				maxX_ = tmpX;
		if(tmpY < minY_)
			minY_ = tmpY;
		else if(tmpY > maxY_)
			maxY_ = tmpY;
		++i;
	}
	if(model_)
		setupModel();
}

AMControlOptimizationSetView::AMControlOptimizationSetView(AMControlOptimizationSet *viewSet, QWidget *parent) :
		AMControlSetView((AMControlSet*)viewSet, parent)
{
	lastContextParams_ = NULL;
	numPoints = 50;

	MPlotWidget *plotWindow = new MPlotWidget();
	//MPlot *plot = new MPlot();
	plot = new MPlot();
	plotWindow->setPlot(plot);
	plot->axisRight()->setTicks(3, MPlotAxis::Inside, 2);	// Set the approximate number and style of axis tick marks:
	plot->axisBottom()->setTicks(3, MPlotAxis::Inside, 2);
	plot->axisBottom()->setAxisName("Resolution");
	plot->axisLeft()->setAxisName("Flux");
	plot->setMarginTop(5);
	plot->setMarginRight(5);
	plot->setMarginLeft(15);
	plot->setMarginBottom(25);
	plot->setScalePadding(5);	// set axis scale padding in percent
	plot->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
	plotWindow->resize(500, 375);
	MPlotDragZoomerTool *dzTool5 = new MPlotDragZoomerTool();
	plot->addTool(dzTool5);
	MPlotPlotSelectorTool *psTool5 = new MPlotPlotSelectorTool;
	plot->addTool(psTool5);

	//MPlotRealtimeModel *tmpModel;
	//AMQuickDataSet *tmpDataSet;
	QMap<double, double> fkData;
	fkData.insert(1.0, 1.0);
	mANSWER = new MPlotRealtimeModel(this);
	ANSWER = new AMQuickDataSet(fkData, this);
	ANSWER->setModel(mANSWER);

	mCurrentPoint = new MPlotRealtimeModel(this);
	mCurrentPoint->insertPointBack(7500, 1);

	mVerticalCursor = new MPlotRealtimeModel(this);
	mVerticalCursor->insertPointBack(7500, 1);
	mVerticalCursor->insertPointBack(7500, 1);

	mHorizontalCursor = new MPlotRealtimeModel(this);
	mHorizontalCursor->insertPointBack(7500, 1);
	mHorizontalCursor->insertPointBack(7500, 1);

	MPlotSeriesBasic *tmpSeries;
	tmpSeries = new MPlotSeriesBasic();
	tmpSeries->setModel(mANSWER);
	QPen tmpPen(QBrush(QColor(255, 0, 255)), 2);
	tmpSeries->setLinePen(tmpPen);
	plot->addItem(tmpSeries);

	tmpSeries = new MPlotSeriesBasic();
	tmpSeries->setModel(mCurrentPoint);
	QPen tmpPen2(QBrush(QColor(0, 255, 255)), 3);
	tmpSeries->setMarker(MPlotMarkerShape::Triangle, 8, tmpPen2);
	plot->addItem(tmpSeries);

	tmpSeries = new MPlotSeriesBasic();
	tmpSeries->setModel(mVerticalCursor);
	QPen tmpPen3(QBrush(QColor(255, 0, 0)), 2);
	tmpSeries->setMarker(MPlotMarkerShape::Triangle, 4, tmpPen3);
	tmpSeries->setLinePen(tmpPen3);
	plot->addItem(tmpSeries);

	tmpSeries = new MPlotSeriesBasic();
	tmpSeries->setModel(mHorizontalCursor);
	QPen tmpPen4(QBrush(QColor(255, 0, 0)), 2);
	tmpSeries->setMarker(MPlotMarkerShape::None, 0, tmpPen4);
	tmpSeries->setLinePen(tmpPen4);
	plot->addItem(tmpSeries);

	hl_->insertWidget(0, plotWindow);

	connect(this, SIGNAL(configValuesChanged()), this, SLOT(onConfigValuesChanged()));
	setFixedSize(600, 375);
}

void AMControlOptimizationSetView::onRegionsUpdate(AMRegionsList* contextParams){
	bool forceEmit = false;
	if(!lastContextParams_)
		forceEmit = true;
	lastContextParams_ = contextParams;
	cANSWER_ = ((AMControlOptimizationSet*)viewSet_)->cOnePlot(contextParams);
	//ANSWER->setDataMap( ((AMControlOptimizationSet*)viewSet_)->cOnePlot(contextParams)->dataMap() );
	ANSWER->setDataMap( cANSWER_->dataMap() );
	if(forceEmit){
		boxTrigger_ = true;
		emit configValuesChanged();
	}
}

void AMControlOptimizationSetView::onConfigValuesChanged(){
	if(!lastContextParams_)
		return;
	const AMControlOptimization *tmpOpt = ((AMControlOptimizationSet*)viewSet_)->optimizationAt(1);
	if( tmpOpt->name() == "SGMResolution" ){
		QString gnh = "";
		if(configValues_["grating"] == "Low")
			gnh = "LEG1";
		else if(configValues_["grating"] == "Medium")
			gnh = "MEG1";
		else if(configValues_["harmonic"] == "First")
			gnh = "HEG1";
		else
			gnh = "HEG3";
		double lRes = ((AMControlOptimizationSet*)viewSet_)->cCollapseAt(1, lastContextParams_).value(gnh)->valueAt(configValues_["exitSlitGap"].toDouble());
		double lFlux = ((AMControlOptimizationSet*)viewSet_)->cCollapseAt(0, lastContextParams_).value(gnh)->valueAt(configValues_["exitSlitGap"].toDouble());
		mCurrentPoint->removePointBack();
		mCurrentPoint->insertPointBack(lRes, lFlux);

		AMCurve *oneCurve = ((AMControlOptimizationSet*)viewSet_)->cOnePlot(lastContextParams_);
		mVerticalCursor->removePointBack();
		mVerticalCursor->removePointBack();
		mVerticalCursor->insertPointBack(lRes-0.005*lRes, 0.9*oneCurve->minY().first );
		mVerticalCursor->insertPointBack(lRes+0.005*lRes, 1.1*oneCurve->maxY().first );

		mHorizontalCursor->removePointBack();
		mHorizontalCursor->removePointBack();
		mHorizontalCursor->insertPointBack( 1.1*oneCurve->maxX().first, lFlux);
		mHorizontalCursor->insertPointBack( 0.9*oneCurve->minX().first, lFlux);

		emit parameterValuesChanged(lRes, lFlux);
	}
}


AMCompactControlOptimizationSetView::AMCompactControlOptimizationSetView(AMControlSet *viewSet, QWidget *parent) :
		QGroupBox(parent)
{
	int width = 350;
	setFixedSize(width, 150);
	viewSet_ = (AMControlOptimizationSet*)viewSet;
	setTitle(viewSet->name());
	launchDetailButton_ = new QPushButton("More Details", this);
	detailView_ = new AMControlOptimizationSetView(viewSet_);
	connect(launchDetailButton_, SIGNAL(clicked()), detailView_, SLOT(show()));
	connect(detailView_, SIGNAL(configValuesChanged()), this, SLOT(onConfigValuesUpdate()));
	connect(detailView_, SIGNAL(parameterValuesChanged(double,double)), this, SLOT(onParamValuesChanged(double,double)));
	param1Trigger_= false;
	param2Trigger_ = false;
	details1Trigger_ = false;
	details2Trigger_ = false;
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
		paramsResult_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);

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
	if(details1Trigger_){
		details1Trigger_ = false;
		return true;
	}
	double param1Val = param1Curve_->valuesAtRange((double)val/100).first;
	double param2Val = param1Curve_->valuesAtRange((double)val/100).second;
	double otherPercent = param2Curve_->percentFromValue(param2Val)*100;
	param1Item_->updateCurveMarker(val);
	param2Item_->updateCurveMarker(otherPercent);

	param1Trigger_ = true;
	param2Slider->setValue(otherPercent);
	parseConfigValues(param1Curve_->metaAt(param1Val));
	return true;
}

bool AMCompactControlOptimizationSetView::onParam2SliderUpdate(int val){
	if(!param1Curve_ || !param2Curve_)
		return false;
	if(param1Trigger_){
		param1Trigger_ = false;
		return true;
	}
	if(details2Trigger_){
		details2Trigger_ = false;
		return true;
	}
	double param2Val = param2Curve_->valuesAtRange((double)val/100).first;
	double param1Val = param2Curve_->valuesAtRange((double)val/100).second;
	double otherPercent = param1Curve_->percentFromValue(param1Val)*100;
	param1Item_->updateCurveMarker(otherPercent);
	param2Item_->updateCurveMarker(val);

	param2Trigger_ = true;
	param1Slider->setValue(otherPercent);
	parseConfigValues(param2Curve_->metaAt(param2Val));
	return true;
}

bool AMCompactControlOptimizationSetView::onParamValuesChanged(double param1, double param2){
	QStringList optConfigs = param1Curve_->metaAt(param1);
	QMap<QString, QVariant> optMap, curMap;
	curMap = detailView_->configValues();
	for(int x = 0; x < optConfigs.count(); x++)
		if(optConfigs.at(x).split('|').at(2) == "d")
			optMap.insert(optConfigs.at(x).split('|').at(0), optConfigs.at(x).split('|').at(1));
	QMap<QString, QVariant>::const_iterator i = optMap.constBegin();
	bool optimum = true;
	while(i != optMap.constEnd()){
		if( (i.value() != curMap.value(i.key())) && (i.value() != "SWITCH") ){
			optimum = false;
		}
		++i;
	}
	nonOptimumValues(optimum);
	if(detailView_->boxTrigger()){
		details1Trigger_ = true;
		details2Trigger_ = true;
		detailView_->resetBoxTrigger();
		double param1Percent = param1Curve_->percentFromValue(param1)*100;
		double param2Percent = param2Curve_->percentFromValue(param2)*100;
		param1Slider->setValue(param1Percent);
		param2Slider->setValue(param2Percent);
		param1Item_->updateCurveMarker(param1Percent);
		param2Item_->updateCurveMarker(param2Percent);
	}

}

bool AMCompactControlOptimizationSetView::onConfigValuesUpdate(){
	QString resStr = "";
	QMap<QString, QVariant>::const_iterator i = detailView_->configValues().constBegin();
	while(i != detailView_->configValues().constEnd()){
		resStr.append(i.value().toString() );
		resStr.append(" ");
		++i;
	}
	paramsResult_->setText(resStr);
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

	detailView_->onRegionsUpdate(contextParams);
}

void AMCompactControlOptimizationSetView::parseConfigValues(const QStringList configList){
	QString tmpName, tmpVal;
	QMap<QString, QVariant> passMap;
	for(int x = 0; x < configList.count(); x++){
		tmpName = configList.at(x).split("|").at(0);
		tmpVal = configList.at(x).split("|").at(1);
		if(configList.at(x).split("|").at(2) == "c")
			passMap.insert(tmpName, tmpVal.toDouble());
		else
			passMap.insert(tmpName, tmpVal);
	}
	detailView_->setConfigValues(passMap);
}

void AMCompactControlOptimizationSetView::nonOptimumValues(bool optimumValues){
	if(optimumValues == optimumValues_)
		return;
	optimumValues_ = optimumValues;
	if(optimumValues_){
		paramsResult_->setStyleSheet("");
	}
	else{
		paramsResult_->setStyleSheet("QLabel {"
									 "background: red;}"
									 );
	}
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
	curveMarker_ = 10;
}

void CCOSVItem::updateCurve(AMCurve *dataCurve){
	if(!curve_){
		return;
	}
	dataCurve_ = dataCurve;
	double min = dataCurve_->minY().first;
	double max = dataCurve_->maxY().first;
	double tmpVal;

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

void CCOSVItem::updateCurveMarker(double percent){
	curveMarker_ = percent;
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
	if(curve_){
		painter->drawConvexPolygon(curve_, width_+2);
		painter->setBrush(QColor("red"));
		painter->setPen(QColor("red"));
		painter->drawLine((int)(curveMarker_*width_/100), 0, (int)(curveMarker_*width_/100), height_);
	}
}

AMDetectorInfoView::AMDetectorInfoView(AMDetectorInfo *detectorInfo, bool interactive, QWidget *parent) :
		QGroupBox(parent)
{
	detectorInfo_ = detectorInfo;
	interactive_ = interactive;
	vl_ = new QVBoxLayout();
	hl_ = new QHBoxLayout();
	switchToEditBox_ = new QPushButton("Switch to Edit Mode");
	switchToEditBox_->setEnabled(false);
	QSpacerItem *spc1 = new QSpacerItem(10, 10, QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
	hl_->addSpacerItem(spc1);
	hl_->addWidget(switchToEditBox_, 0, Qt::AlignRight);
	vl_->addLayout(hl_);
}

PGTDetectorInfoView::PGTDetectorInfoView(PGTDetectorInfo *detectorInfo, bool interactive, QWidget *parent) :
		AMDetectorInfoView(detectorInfo, interactive, parent)
{
	sDetectorInfo_ = detectorInfo;
	interactive_ = interactive;
	setTitle("PGT Silicon Drift Detector");
	fl_ = new QFormLayout();
	QHBoxLayout *tmpHB;
	tmpHB = new QHBoxLayout();
	integrationTimeBox_ = new QDoubleSpinBox();
	integrationTimeBox_->setValue(sDetectorInfo_->integrationTime());
	integrationTimeBox_->setEnabled(interactive_);
	QPair<double, double> itRange = sDetectorInfo_->integrationTimeRange();
	integrationTimeBox_->setRange(itRange.first, itRange.second);
	connect(integrationTimeBox_, SIGNAL(valueChanged(double)), sDetectorInfo_, SLOT(setIntegrationTime(double)));
	tmpHB->addWidget(integrationTimeBox_);
	fl_->addRow("Integration Time", tmpHB);
	tmpHB = new QHBoxLayout();
	integrationModeBox_ = new QComboBox();
	integrationModeBox_->addItems(sDetectorInfo_->integrationModeList());
	integrationModeBox_->setCurrentIndex( sDetectorInfo_->integrationModeList().indexOf(sDetectorInfo_->integrationMode()) );
	integrationModeBox_->setEnabled(interactive_);
	tmpHB->addWidget(integrationModeBox_);
	connect(integrationModeBox_, SIGNAL(currentIndexChanged(QString)), sDetectorInfo_, SLOT(setIntegrationMode(QString)));
	fl_->addRow("Integration Mode", tmpHB);
	tmpHB = new QHBoxLayout();
	hvSetpointBox_ = new QDoubleSpinBox();
	hvSetpointBox_->setValue(sDetectorInfo_->hvSetpoint());
	hvSetpointBox_->setEnabled(interactive_);
	QPair<double, double> hvRange = sDetectorInfo_->hvSetpointRange();
	hvSetpointBox_->setRange(hvRange.first, hvRange.second);
	connect(hvSetpointBox_, SIGNAL(valueChanged(double)), sDetectorInfo_, SLOT(setHVSetpoint(double)));
	tmpHB->addWidget(hvSetpointBox_);
	fl_->addRow("HV Setpoint", tmpHB);
	allBoxes_.append(integrationTimeBox_);
	allBoxes_.append(integrationModeBox_);
	allBoxes_.append(hvSetpointBox_);
	vl_->removeItem(hl_);
	vl_->addLayout(fl_);
	vl_->addLayout(hl_);
	setLayout(vl_);
	setMinimumWidth(250);
}

MCPDetectorInfoView::MCPDetectorInfoView(MCPDetectorInfo *detectorInfo, bool interactive, QWidget *parent) :
		AMDetectorInfoView(detectorInfo, interactive, parent)
{
	sDetectorInfo_ = detectorInfo;
	interactive_ = interactive;
	setTitle("MCP Total Fluoresence Yield");
	fl_ = new QFormLayout();
	QHBoxLayout *tmpHB;
	tmpHB = new QHBoxLayout();
	hvSetpointBox_ = new QDoubleSpinBox();
	hvSetpointBox_->setValue(sDetectorInfo_->hvSetpoint());
	hvSetpointBox_->setEnabled(interactive_);
	QPair<double, double> hvRange = sDetectorInfo_->hvSetpointRange();
	hvSetpointBox_->setRange(hvRange.first, hvRange.second);
	tmpHB->addWidget(hvSetpointBox_);
	connect(hvSetpointBox_, SIGNAL(valueChanged(double)), sDetectorInfo_, SLOT(setHVSetpoint(double)));
	fl_->addRow("HV Setpoint", tmpHB);
	allBoxes_.append(hvSetpointBox_);
	vl_->removeItem(hl_);
	vl_->addLayout(fl_);
	vl_->addLayout(hl_);
	setLayout(vl_);
	setMinimumWidth(250);
}

AMDetectorInfoSetView::AMDetectorInfoSetView(AMDetectorInfoSet *viewSet, bool setup, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	setTitle(viewSet->name());
	if(setup)
		runSetup();
}

void AMDetectorInfoSetView::runSetup(){
	QGridLayout *gl = new QGridLayout();
	QCheckBox *tmpBox;
	QPushButton *tmpDetails;
	QLabel *tmpLabel;
	AMDetectorInfo *tmpDetector;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpDetector = viewSet_->detectorAt(x);
		tmpBox = new QCheckBox();
		tmpBox->setChecked(viewSet_->isDefaultAt(x));
		tmpDetails = new QPushButton("Details");
		if( !tmpDetector->hasDetails() )
			tmpDetails->setEnabled(false);
		tmpLabel = new QLabel(tmpDetector->description());
		gl->addWidget(tmpLabel, x, 0, 1, 1, Qt::AlignLeft);
		gl->addWidget(tmpBox, x, 1, 1, 1, Qt::AlignLeft);
		gl->addWidget(tmpDetails, x, 2, 1, 1, Qt::AlignLeft);
		detectorBoxes_.append(tmpBox);
		detectorDetails_.append(tmpDetails);
		detailViews_.append(detailViewByType(tmpDetector));
		detailViews_.last()->hide();
		connect(tmpDetails, SIGNAL(clicked()), detailViews_.last(), SLOT(show()));
		connect(tmpDetails, SIGNAL(clicked()), detailViews_.last(), SLOT(raise()));
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(gl);
	setLayout(hl_);
}

QWidget* AMDetectorInfoSetView::detailViewByType(AMDetectorInfo *detectorInfo){
	if(detectorInfo->typeDescription() == "PGT SDD Spectrum-Output Detector")
		return new PGTDetectorInfoView( (PGTDetectorInfo*)detectorInfo, true);
	else if(detectorInfo->typeDescription() == "MCP Detector")
		return new MCPDetectorInfoView( (MCPDetectorInfo*)detectorInfo, true );
	else
		return new QGroupBox();
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
