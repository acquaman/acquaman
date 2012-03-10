#include "REIXSXESImageABEditor.h"

#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSlider>

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotRectangle.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotTools.h"
#include "dataman/datasource/AMDataSourceImageData.h"

#include "analysis/REIXS/REIXSXESImageAB.h"

#include <QDebug>

REIXSXESImageABEditor::REIXSXESImageABEditor(REIXSXESImageAB *analysisBlock, QWidget *parent) :
	QWidget(parent)
{
	analysisBlock_ = analysisBlock;

	// setup GUI elements:
	rangeMinControl_ = new QSpinBox();
	rangeMinControl_->setSingleStep(1);
	rangeMinControl_->setMinimum(0);
	rangeMaxControl_ = new QSpinBox();
	rangeMaxControl_->setSingleStep(1);
	rangeMaxControl_->setMinimum(0);

	correlationCenterBox_ = new QSpinBox();
	correlationCenterBox_->setSingleStep(1);
	correlationCenterBox_->setMinimum(1);
	correlationPointsBox_ = new QSpinBox();
	correlationPointsBox_->setMinimum(10);
	correlationPointsBox_->setMaximum(500);
	correlationPointsBox_->setValue(200);

	liveCorrelationCheckBox_ = new QCheckBox("Real-time correlation");
	correlateNowButton_ = new QPushButton("Correlate Now");

	shiftDisplayOffsetSlider_ = new QSlider(Qt::Horizontal);

	plot_ = new MPlot();
	plotWidget_ = new MPlotWidget();
	plotWidget_->setPlot(plot_);

	plot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->axisRight()->setTicks(0);
	plot_->axisBottom()->setTicks(4);
	plot_->axisLeft()->showGrid(false);
	plot_->axisBottom()->showAxisName(false);
	plot_->axisLeft()->showAxisName(false);
	plot_->axisScaleBottom()->setPadding(0);
	plot_->axisScaleLeft()->setPadding(0);
	image_ = 0;

	shiftData_ = new REIXSXESImageABEditorShiftModel(analysisBlock_);
	shiftSeries_ = new MPlotSeriesBasic();
	shiftSeries_->setModel(shiftData_, true);
	shiftSeries_->setIgnoreWhenAutoScaling(true);
	plot_->addItem(shiftSeries_);
	shiftSeries_->setZValue(5000);	// put on top of everything.

	QColor white(Qt::white);
	QPen pen(white, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	white.setAlphaF(0.6);
	rangeRectangle1_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangle1_->setSelectable(false);
	rangeRectangle1_->setIgnoreWhenAutoScaling(true);
	rangeRectangle1_->setZValue(3000);
	plot_->addItem(rangeRectangle1_);
	rangeRectangle1_->setVisible(false);
	rangeRectangle2_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangle2_->setSelectable(false);
	rangeRectangle2_->setIgnoreWhenAutoScaling(true);
	rangeRectangle2_->setZValue(3000);
	plot_->addItem(rangeRectangle2_);
	rangeRectangle2_->setVisible(false);
	plot_->legend()->enableDefaultLegend(false);

	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());



	QVBoxLayout* vl = new QVBoxLayout();
	QFormLayout* fl = new QFormLayout();
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(rangeMinControl_);
	hl->addWidget(new QLabel("To"));
	hl->addWidget(rangeMaxControl_);
	fl->addRow("From (index)", hl);
	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(correlationCenterBox_);
	hl2->addWidget(new QLabel("Points"));
	hl2->addWidget(correlationPointsBox_);
	fl->addRow("Correlation: Center", hl2);
	fl->addRow(liveCorrelationCheckBox_);
	fl->addRow(correlateNowButton_);
	vl->addLayout(fl);
	vl->addWidget(plotWidget_);
	setLayout(vl);

	onAnalysisBlockInputDataSourcesChanged();
//	onShiftValuesChanged();

	// make connections:
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onAnalysisBlockInputDataSourcesChanged()));
//	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));

	connect(rangeMinControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMinControlChanged(int)));
	connect(rangeMaxControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMaxControlChanged(int)));
	connect(correlationCenterBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelationCenterBoxChanged(int)));
	connect(correlationPointsBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelationPointsBoxChanged(int)));

	connect(shiftDisplayOffsetSlider_, SIGNAL(valueChanged(int)), shiftData_, SLOT(setDisplayXOffset(int)));

	// direct connections:
	connect(correlateNowButton_, SIGNAL(clicked()), analysisBlock_, SLOT(correlateNow()));
	connect(liveCorrelationCheckBox_, SIGNAL(toggled(bool)), analysisBlock_, SLOT(enableLiveCorrelation(bool)));
}

void REIXSXESImageABEditor::onRangeMinControlChanged(int newRangeMin)
{
	if(newRangeMin == analysisBlock_->sumRangeMin())
		return;

	analysisBlock_->setSumRangeMin(newRangeMin);

	// don't let the max go below the min.
	if(rangeMaxControl_->value() < newRangeMin)
		rangeMaxControl_->setValue(newRangeMin);

	placeRangeRectangle();
}

void REIXSXESImageABEditor::onRangeMaxControlChanged(int newRangeMax)
{
	if(newRangeMax == analysisBlock_->sumRangeMax())
		return;

	analysisBlock_->setSumRangeMax(newRangeMax);

	if(rangeMinControl_->value() > newRangeMax)
		rangeMinControl_->setValue(newRangeMax);

	placeRangeRectangle();
}

void REIXSXESImageABEditor::onCorrelationCenterBoxChanged(int center)
{
	if(center == analysisBlock_->correlationCenterPixel())
		return;

	analysisBlock_->setCorrelationCenterPixel(center);

	// update lines showing correlation range.
}

void REIXSXESImageABEditor::onCorrelationPointsBoxChanged(int points)
{
	int halfWidth = points/2;

	if(halfWidth == analysisBlock_->correlationHalfWidth())
		return;

	analysisBlock_->setCorrelationHalfWidth(halfWidth);

	// update lines showing correlation range.
}

void REIXSXESImageABEditor::onAnalysisBlockInputDataSourcesChanged()
{
	if(image_) {
		delete image_;
		image_ = 0;
	}

	AMDataSource* inputSource;
	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		// inputSource is a valid data source

		rangeMinControl_->setEnabled(true);
		rangeMaxControl_->setEnabled(true);
		correlationCenterBox_->setEnabled(true);
		correlationPointsBox_->setEnabled(true);
		liveCorrelationCheckBox_->setEnabled(true);
		correlateNowButton_->setEnabled(true);
		shiftDisplayOffsetSlider_->setEnabled(true);


		rangeMinControl_->blockSignals(true);
		rangeMinControl_->setMaximum(inputSource->size(1) - 1);
		rangeMinControl_->setValue(analysisBlock_->sumRangeMin());
		rangeMinControl_->blockSignals(false);

		rangeMaxControl_->blockSignals(true);
		rangeMaxControl_->setMaximum(inputSource->size(1) - 1);
		rangeMaxControl_->setValue(analysisBlock_->sumRangeMax());
		rangeMaxControl_->blockSignals(false);

		correlationCenterBox_->blockSignals(true);
		correlationCenterBox_->setMaximum(inputSource->size(0) - 1);
		correlationCenterBox_->setValue(analysisBlock_->correlationCenterPixel());
		correlationCenterBox_->blockSignals(false);

		correlationPointsBox_->blockSignals(true);
		correlationPointsBox_->setValue(analysisBlock_->correlationHalfWidth()*2);
		correlationPointsBox_->blockSignals(false);

		liveCorrelationCheckBox_->blockSignals(true);
		liveCorrelationCheckBox_->setChecked(analysisBlock_->liveCorrelation());
		liveCorrelationCheckBox_->blockSignals(false);

		shiftDisplayOffsetSlider_->blockSignals(true);
		shiftDisplayOffsetSlider_->setRange(0, analysisBlock_->size(0)-1);
		shiftDisplayOffsetSlider_->setValue(analysisBlock_->size(0)/2);
		shiftDisplayOffsetSlider_->blockSignals(false);

		image_ = new MPlotImageBasic();
		image_->setModel(new AMDataSourceImageData(inputSource), true);
		plot_->addItem(image_);
	}

	else {
		// no input source. Not much we can do.
		rangeMinControl_->setEnabled(false);
		rangeMaxControl_->setEnabled(false);
		correlationCenterBox_->setEnabled(false);
		correlationPointsBox_->setEnabled(false);
		liveCorrelationCheckBox_->setEnabled(false);
		correlateNowButton_->setEnabled(false);
		shiftDisplayOffsetSlider_->setEnabled(false);
	}

	placeRangeRectangle();
}

void REIXSXESImageABEditor::placeRangeRectangle()
{
	AMDataSource* inputSource;
	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		double dataMin = inputSource->axisValue(1,0);
		double sumMin = inputSource->axisValue(1, analysisBlock_->sumRangeMin());
		double sumMax = inputSource->axisValue(1, analysisBlock_->sumRangeMax());
		double dataMax = inputSource->axisValue(1, inputSource->size(1)-1);

		rangeRectangle1_->setYAxisTarget(plot_->axisScaleLeft());	// note: does nothing if already correct
		rangeRectangle2_->setYAxisTarget(plot_->axisScaleLeft());	// note: does nothing if already correct
		rangeRectangle1_->setXAxisTarget(plot_->axisScaleHorizontalRelative());	// note: does nothing if already correct
		rangeRectangle2_->setXAxisTarget(plot_->axisScaleHorizontalRelative());	// note: does nothing if already correct

		rangeRectangle1_->setRect(QRectF(QPointF(0,dataMin), QPointF(1,sumMin)).normalized());
		rangeRectangle2_->setRect(QRectF(QPointF(0,sumMax), QPointF(1,dataMax)).normalized());

		rangeRectangle1_->setVisible(true);
		rangeRectangle2_->setVisible(true);
	}
	else {
		rangeRectangle1_->setVisible(false);
		rangeRectangle2_->setVisible(false);
	}
}

//void REIXSXESImageABEditor::onShiftValuesChanged()
//{
//	AMIntList shiftValues = analysisBlock_->shiftValues();
//	QVector<qreal> xs, ys;

//	int midPixel = analysisBlock_->size(0)/2;
//	// note: this is turned on its side, so the shift values become X, and the row values become Y.
//	for(int j=0,cc=shiftValues.count(); j<cc; j++) {
//		if(j>=analysisBlock_->sumRangeMin() && j<=analysisBlock_->sumRangeMax()) {
//			xs << midPixel + shiftValues.at(j);
//			ys << j;
//		}
//	}

//	shiftData_->setValues(xs, ys);
//}

REIXSXESImageABEditorShiftModel::REIXSXESImageABEditorShiftModel(REIXSXESImageAB *analysisBlock, QObject *parent)
	: QObject(parent)
{
	analysisBlock_ = analysisBlock;
	displayXOffset_ = analysisBlock_->size(0)/2;

	connect(analysisBlock_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));
}
REIXSXESImageABEditorShiftModel::~REIXSXESImageABEditorShiftModel() {
	qDebug() << "!!!!!!!!!!!!!!!!!\nShift model getting deleted. That means the series and plot were deleted. Good.";
}

qreal REIXSXESImageABEditorShiftModel::x(unsigned index) const
{
	return analysisBlock_->shiftValues().at(index) + displayXOffset_;
}

qreal REIXSXESImageABEditorShiftModel::y(unsigned index) const
{
	return index;
}

int REIXSXESImageABEditorShiftModel::count() const
{
	return analysisBlock_->shiftValues().count();
}

void REIXSXESImageABEditorShiftModel::setDisplayXOffset(int offset)
{
	displayXOffset_ = offset;
	emitDataChanged();
}

void REIXSXESImageABEditorShiftModel::onOutputSizeChanged()
{
	setDisplayXOffset(analysisBlock_->size(0)/2);
	emitDataChanged();
}

void REIXSXESImageABEditorShiftModel::onShiftValuesChanged()
{
	emitDataChanged();
}

