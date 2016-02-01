/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "REIXSXESImageABEditor.h"

#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSlider>
#include <QToolButton>
#include <QComboBox>

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotRectangle.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotColorMap.h"
#include "MPlot/MPlotPoint.h"
#include "dataman/datasource/AMDataSourceImageData.h"
#include "ui/dataman/AMChooseScanDialog.h"

#include "analysis/REIXS/REIXSXESImageAB.h"

#include <QAction>
#include <QMenu>

REIXSXESImageABEditor::REIXSXESImageABEditor(REIXSXESImageAB *analysisBlock, QWidget *parent) :
	QWidget(parent)
{
	chooseScanDialog_ = 0;

	analysisBlock_ = analysisBlock;

	// setup GUI elements:
	energyCalibrationOffsetBox_ = new QDoubleSpinBox();
	energyCalibrationOffsetBox_->setRange(-100, 100);
	energyCalibrationOffsetBox_->setDecimals(3);
	energyCalibrationOffsetBox_->setSingleStep(0.2);
	energyCalibrationOffsetBox_->setSuffix(" eV");
	tiltCalibrationOffsetBox_ = new QDoubleSpinBox();
	tiltCalibrationOffsetBox_->setRange(-10, 10);
	tiltCalibrationOffsetBox_->setDecimals(3);
	tiltCalibrationOffsetBox_->setSingleStep(0.1);
	tiltCalibrationOffsetBox_->setSuffix(" deg");

	rangeMinYControl_ = new QSpinBox();
	rangeMinYControl_->setSingleStep(1);
	rangeMinYControl_->setMinimum(0);
	rangeMaxYControl_ = new QSpinBox();
	rangeMaxYControl_->setSingleStep(1);
	rangeMaxYControl_->setMinimum(0);

	rangeMinXControl_ = new QSpinBox();
	rangeMinXControl_->setSingleStep(1);
	rangeMinXControl_->setMinimum(0);
	rangeMaxXControl_ = new QSpinBox();
	rangeMaxXControl_->setSingleStep(1);
	rangeMaxXControl_->setMinimum(0);

	rangeRoundControl_ = new QDoubleSpinBox();
	rangeRoundControl_->setSingleStep(0.05);
	rangeRoundControl_->setMinimum(0.0);
	rangeRoundControl_->setMaximum(1.0);

	correlationCenterBox_ = new QSpinBox();
	correlationCenterBox_->setSingleStep(1);
	correlationCenterBox_->setMinimum(1);
	correlationPointsBox_ = new QSpinBox();
	correlationPointsBox_->setMinimum(10);
	correlationPointsBox_->setMaximum(500);
	correlationPointsBox_->setValue(200);

	correlationSmoothingBox_ = new QComboBox();
	correlationSmoothingBox_->addItem("None");

	/*
	correlationSmoothingBox_->addItem("Quadratic");
	correlationSmoothingBox_->addItem("Cubic");
	correlationSmoothingBox_->addItem("Quartic");
	*/

	correlationSmoothingBox_->addItem("Poly Fit");
	correlationSmoothingBox_->addItem("Moving Median");
	correlationSmoothingBox_->addItem("Moving Average");

	smoothModeBox_ = new QSpinBox();
	smoothModeBox_->setSingleStep(1);
	smoothModeBox_->setMinimum(1);
	smoothModeBox_->setValue(1);
	smoothModeBox_->setMaximum(99);

	liveCorrelationCheckBox_ = new QCheckBox("Real-time");
	correlateNowButton_ = new QPushButton("Now");

	manualShiftEntryButton_ = new QToolButton();
	manualShiftEntryButton_->setText("Manual shift...");

	applyToOtherScansButton_ = new QToolButton();
	applyToOtherScansButton_->setText("Apply to Scans...");
	applyToOtherScansButton_->setPopupMode(QToolButton::MenuButtonPopup);

	batchApplyCorrelationSettings_ = new QAction("Correlation Settings", this);
	batchApplyShiftCurve_ = new QAction("Shift Curve", this);
	batchApplySumRange_ = new QAction("Sum Range min, max", this);
	batchApplyCalibrationOffsets_ = new QAction("Calib. offsets: energy, tilt", this);
	batchApplyCorrelationSettings_->setCheckable(true);
	batchApplyCorrelationSettings_->setChecked(true);
	batchApplyShiftCurve_->setCheckable(true);
	batchApplyCalibrationOffsets_->setCheckable(true);
	batchApplyShiftCurve_->setChecked(false);
	batchApplySumRange_->setCheckable(true);
	batchApplySumRange_->setChecked(false);
	batchApplyCalibrationOffsets_->setChecked(false);
	QMenu* batchApplyMenu = new QMenu(this);
	batchApplyMenu->addAction(batchApplySumRange_);
	batchApplyMenu->addAction(batchApplyCalibrationOffsets_);
	batchApplyMenu->addAction(batchApplyCorrelationSettings_);
	batchApplyMenu->addAction(batchApplyShiftCurve_);
	applyToOtherScansButton_->setMenu(batchApplyMenu);

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
	colorMap_ = new MPlotColorMap(MPlotColorMap::Jet);
	colorMap_->setBrightness(0.08);
	colorMap_->setContrast(2.1);
	colorMap_->setGamma(1.0);

	shiftData_ = new REIXSXESImageABEditorShiftModel(analysisBlock_);
	shiftSeries_ = new MPlotSeriesBasic();
	shiftSeries_->setModel(shiftData_, true);
	shiftSeries_->setIgnoreWhenAutoScaling(true);
	shiftSeries_->setMarker(MPlotMarkerShape::None);
	plot_->addItem(shiftSeries_);
	shiftSeries_->setZValue(2999);	// put on top of plot, but below range rectangles.


	ellipseData_ = new REIXSXESImageABEditorEllipticalMask(analysisBlock_);
	ellipseSeries_ = new MPlotSeriesBasic();
	ellipseSeries_->setModel(ellipseData_, true);
	ellipseSeries_->setIgnoreWhenAutoScaling(true);
	ellipseSeries_->setLinePen(QPen(QBrush(QColor(Qt::white)),1));
	ellipseSeries_->setMarker(MPlotMarkerShape::None);
	plot_->addItem(ellipseSeries_);
	ellipseSeries_->setZValue(2999);	// put on top of plot, but below range rectangles.




	corrRegionLeft_ = new MPlotPoint(QPointF(analysisBlock_->correlationCenterPixel()-analysisBlock_->correlationHalfWidth(), 0));
	corrRegionRight_ = new MPlotPoint(QPointF(analysisBlock_->correlationCenterPixel()+analysisBlock_->correlationHalfWidth(), 0));
	corrRegionLeft_->setMarker(MPlotMarkerShape::VerticalBeam, 2000, QPen(QColor(Qt::black)));
	corrRegionRight_->setMarker(MPlotMarkerShape::VerticalBeam, 2000, QPen(QColor(Qt::black)));
	corrRegionLeft_->setIgnoreWhenAutoScaling(true);
	corrRegionRight_->setIgnoreWhenAutoScaling(true);
	plot_->addItem(corrRegionLeft_);
	plot_->addItem(corrRegionRight_);
	corrRegionLeft_->setZValue(2998);
	corrRegionRight_->setZValue(2997);

	QColor white(Qt::white);
	QPen pen(white, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	white.setAlphaF(0.6);
	rangeRectangleY1_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangleY1_->setSelectable(false);
	rangeRectangleY1_->setIgnoreWhenAutoScaling(true);
	rangeRectangleY1_->setZValue(3000);
	plot_->addItem(rangeRectangleY1_);
	rangeRectangleY1_->setVisible(false);
	rangeRectangleY2_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangleY2_->setSelectable(false);
	rangeRectangleY2_->setIgnoreWhenAutoScaling(true);
	rangeRectangleY2_->setZValue(3000);
	plot_->addItem(rangeRectangleY2_);
	rangeRectangleY2_->setVisible(false);

	rangeRectangleX1_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangleX1_->setSelectable(false);
	rangeRectangleX1_->setIgnoreWhenAutoScaling(true);
	rangeRectangleX1_->setZValue(3000);
	plot_->addItem(rangeRectangleX1_);
	rangeRectangleX1_->setVisible(false);
	rangeRectangleX2_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangleX2_->setSelectable(false);
	rangeRectangleX2_->setIgnoreWhenAutoScaling(true);
	rangeRectangleX2_->setZValue(3000);
	plot_->addItem(rangeRectangleX2_);
	rangeRectangleX2_->setVisible(false);


	plot_->legend()->enableDefaultLegend(false);
	plot_->axisBottom()->setTicks(3);
	plot_->axisLeft()->setTicks(3);

	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());


	// Build Layout:

	QVBoxLayout* vl = new QVBoxLayout();
	QFormLayout* fl = new QFormLayout();
	QHBoxLayout* hl = new QHBoxLayout();

	QHBoxLayout* h2 = new QHBoxLayout();

	hl->addWidget(rangeMinYControl_);
	hl->addWidget(new QLabel("To"));
	hl->addWidget(rangeMaxYControl_);

	h2->addWidget(rangeMinXControl_);
	h2->addWidget(new QLabel("To"));
	h2->addWidget(rangeMaxXControl_);
	h2->addWidget(new QLabel("Round:"));
	h2->addWidget(rangeRoundControl_);

	fl->addRow("From (y):", hl);

	fl->addRow("From (x):", h2);

	QHBoxLayout* hl4 = new QHBoxLayout();
	hl4->addWidget(energyCalibrationOffsetBox_);
	hl4->addWidget(tiltCalibrationOffsetBox_);
	fl->addRow("Offset:", hl4);
	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(correlationCenterBox_);
	hl2->addWidget(new QLabel("#"));
	hl2->addWidget(correlationPointsBox_);
	fl->addRow("C. center:", hl2);
	QHBoxLayout* hl25 = new QHBoxLayout();
	hl25->addWidget(correlationSmoothingBox_);
	hl25->addWidget(smoothModeBox_);
	/*
	fl->addRow("C. smooth:", correlationSmoothingBox_);
	*/
	fl->addRow("C. smooth:", hl25);
	QHBoxLayout* hl5 = new QHBoxLayout();
	hl5->addWidget(liveCorrelationCheckBox_);
	hl5->addWidget(correlateNowButton_);
	fl->addRow("Correlate:", hl5);

	vl->addLayout(fl);
	QHBoxLayout* hl3 = new QHBoxLayout();
	hl3->addWidget(manualShiftEntryButton_);
	hl3->addWidget(applyToOtherScansButton_);
	vl->addLayout(hl3);
	vl->addWidget(plotWidget_);
	vl->addWidget(shiftDisplayOffsetSlider_);
	setLayout(vl);

	onAnalysisBlockInputDataSourcesChanged();
	/*
	onShiftValuesChanged();
	*/

	// make connections:
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onAnalysisBlockInputDataSourcesChanged()));
	/*
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));
	*/

	connect(rangeMinYControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMinYControlChanged(int)));
	connect(rangeMaxYControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMaxYControlChanged(int)));
	connect(rangeMinXControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMinXControlChanged(int)));
	connect(rangeMaxXControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMaxXControlChanged(int)));

	connect(rangeRoundControl_,SIGNAL(valueChanged(double)),this, SLOT(onRangeRoundControlChanged(double)));


	connect(correlationCenterBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelationCenterBoxChanged(int)));
	connect(correlationPointsBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelationPointsBoxChanged(int)));

	connect(shiftDisplayOffsetSlider_, SIGNAL(valueChanged(int)), shiftData_, SLOT(setDisplayXOffset(int)));

	connect(manualShiftEntryButton_, SIGNAL(clicked()), this, SLOT(onManualShiftEntryButtonClicked()));

	// direct connections:
	connect(correlateNowButton_, SIGNAL(clicked()), analysisBlock_, SLOT(correlateNow()));
	connect(liveCorrelationCheckBox_, SIGNAL(toggled(bool)), analysisBlock_, SLOT(enableLiveCorrelation(bool)));
	connect(energyCalibrationOffsetBox_, SIGNAL(valueChanged(double)), analysisBlock_, SLOT(setEnergyCalibrationOffset(double)));
	connect(tiltCalibrationOffsetBox_, SIGNAL(valueChanged(double)), analysisBlock_, SLOT(setTiltCalibrationOffset(double)));

	/*
	connect(correlationSmoothingBox_, SIGNAL(currentIndexChanged(int)), analysisBlock_, SLOT(setCorrelationSmoothing(int)));
	*/

	connect(correlationSmoothingBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCSmoothBoxChanged()));
	connect(smoothModeBox_, SIGNAL(valueChanged(int)), this, SLOT(onCSmoothModeChanged()));


	connect(applyToOtherScansButton_, SIGNAL(clicked()), this, SLOT(onApplyToOtherScansMenuClicked()));


}

REIXSXESImageABEditor::~REIXSXESImageABEditor() {
	delete colorMap_;
}

void REIXSXESImageABEditor::onRangeMinYControlChanged(int newRangeMinY)
{
	if(newRangeMinY == analysisBlock_->sumRangeMinY())
		return;

	analysisBlock_->setSumRangeMinY(newRangeMinY);

	// don't let the max go below the min.
	if(rangeMaxYControl_->value() < newRangeMinY)
		rangeMaxYControl_->setValue(newRangeMinY);

	placeRangeRectangle();
	ellipseData_->rangeValuesChanged();

}


void REIXSXESImageABEditor::onRangeMaxYControlChanged(int newRangeMaxY)
{
	if(newRangeMaxY == analysisBlock_->sumRangeMaxY())
		return;

	analysisBlock_->setSumRangeMaxY(newRangeMaxY);

	if(rangeMinYControl_->value() > newRangeMaxY)
		rangeMinYControl_->setValue(newRangeMaxY);

	placeRangeRectangle();
	ellipseData_->rangeValuesChanged();

}

void REIXSXESImageABEditor::onRangeMinXControlChanged(int newRangeMinX)
{
	if(newRangeMinX == analysisBlock_->sumRangeMinX())
		return;

	analysisBlock_->setSumRangeMinX(newRangeMinX);

	// don't let the max go below the min.
	if(rangeMaxXControl_->value() < newRangeMinX)
		rangeMaxXControl_->setValue(newRangeMinX);

	placeRangeRectangle();
	ellipseData_->rangeValuesChanged();

}


void REIXSXESImageABEditor::onRangeMaxXControlChanged(int newRangeMaxX)
{
	if(newRangeMaxX == analysisBlock_->sumRangeMaxX())
		return;

	analysisBlock_->setSumRangeMaxX(newRangeMaxX);

	if(rangeMinXControl_->value() > newRangeMaxX)
		rangeMinXControl_->setValue(newRangeMaxX);

	placeRangeRectangle();
	ellipseData_->rangeValuesChanged();

}

void REIXSXESImageABEditor::onRangeRoundControlChanged(double newRangeRound)
{
	if(newRangeRound == analysisBlock_->rangeRound())
		return;

	analysisBlock_->setRangeRound(newRangeRound);

	//update ellipse...
	ellipseData_->rangeValuesChanged();

}

void REIXSXESImageABEditor::onCorrelationCenterBoxChanged(int center)
{
	if(center == analysisBlock_->correlationCenterPixel())
		return;

	analysisBlock_->setCorrelationCenterPixel(center);

	// update lines showing correlation range.
	corrRegionLeft_->setValue(QPointF(analysisBlock_->correlationCenterPixel()-analysisBlock_->correlationHalfWidth(), 0));
	corrRegionRight_->setValue(QPointF(analysisBlock_->correlationCenterPixel()+analysisBlock_->correlationHalfWidth(), 0));
}


void REIXSXESImageABEditor::onCSmoothBoxChanged()
{
	//index 0 for none, 1 for poly, 2 for median, 3 for average

	/*
	smoothBoxType type;
	*/

	switch((smoothBoxType)correlationSmoothingBox_->currentIndex()) {
	case None:
		smoothModeBox_->setDisabled(true);
		break;
	case Poly:
		smoothModeBox_->setEnabled(true);
		smoothModeBox_->setMinimum(2);
		smoothModeBox_->setMaximum(4);
		smoothModeBox_->setSingleStep(1);
		smoothModeBox_->setValue(2);
		break;
	case Median:
		smoothModeBox_->setEnabled(true);
		smoothModeBox_->setMinimum(1);
		smoothModeBox_->setMaximum(63);
		smoothModeBox_->setSingleStep(2);
		smoothModeBox_->setValue(3);
		break;
	case Average:
		smoothModeBox_->setEnabled(true);
		smoothModeBox_->setMinimum(1);
		smoothModeBox_->setMaximum(63);
		smoothModeBox_->setSingleStep(2);
		smoothModeBox_->setValue(3);
		break;
	default:
		break;
	}

	analysisBlock_->setCorrelationSmoothing(QPair<int,int>(correlationSmoothingBox_->currentIndex(), smoothModeBox_->value()));
}

void REIXSXESImageABEditor::onCSmoothModeChanged() {
	analysisBlock_->setCorrelationSmoothing(QPair<int,int>(correlationSmoothingBox_->currentIndex(), smoothModeBox_->value()));
}


void REIXSXESImageABEditor::onCorrelationPointsBoxChanged(int points)
{
	int halfWidth = points/2;

	if(halfWidth == analysisBlock_->correlationHalfWidth())
		return;

	analysisBlock_->setCorrelationHalfWidth(halfWidth);

	// update lines showing correlation range.
	corrRegionLeft_->setValue(QPointF(analysisBlock_->correlationCenterPixel()-analysisBlock_->correlationHalfWidth(), 0));
	corrRegionRight_->setValue(QPointF(analysisBlock_->correlationCenterPixel()+analysisBlock_->correlationHalfWidth(), 0));
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

		rangeMinYControl_->setEnabled(true);
		rangeMaxYControl_->setEnabled(true);
		rangeMinXControl_->setEnabled(true);
		rangeMaxXControl_->setEnabled(true);
		rangeRoundControl_->setEnabled(true);

		correlationCenterBox_->setEnabled(true);
		correlationPointsBox_->setEnabled(true);
		correlationSmoothingBox_->setEnabled(true);
		liveCorrelationCheckBox_->setEnabled(true);
		correlateNowButton_->setEnabled(true);
		shiftDisplayOffsetSlider_->setEnabled(true);
		energyCalibrationOffsetBox_->setEnabled(true);
		tiltCalibrationOffsetBox_->setEnabled(true);
		manualShiftEntryButton_->setEnabled(true);

		energyCalibrationOffsetBox_->blockSignals(true);
		energyCalibrationOffsetBox_->setValue(analysisBlock_->energyCalibrationOffset());
		energyCalibrationOffsetBox_->blockSignals(false);

		tiltCalibrationOffsetBox_->blockSignals(true);
		tiltCalibrationOffsetBox_->setValue(analysisBlock_->tiltCalibrationOffset());
		tiltCalibrationOffsetBox_->blockSignals(false);

		rangeMinYControl_->blockSignals(true);
		rangeMinYControl_->setMaximum(inputSource->size(1) - 1);
		rangeMinYControl_->setValue(analysisBlock_->sumRangeMinY());
		rangeMinYControl_->blockSignals(false);

		rangeMaxYControl_->blockSignals(true);
		rangeMaxYControl_->setMaximum(inputSource->size(1) - 1);
		rangeMaxYControl_->setValue(analysisBlock_->sumRangeMaxY());
		rangeMaxYControl_->blockSignals(false);

		rangeMinXControl_->blockSignals(true);
		rangeMinXControl_->setMaximum(inputSource->size(0) - 1);
		rangeMinXControl_->setValue(analysisBlock_->sumRangeMinX());
		rangeMinXControl_->blockSignals(false);

		rangeMaxXControl_->blockSignals(true);
		rangeMaxXControl_->setMaximum(inputSource->size(0) - 1);
		rangeMaxXControl_->setValue(analysisBlock_->sumRangeMaxX());
		rangeMaxXControl_->blockSignals(false);

		rangeRoundControl_->blockSignals(true);
		rangeRoundControl_->setMaximum(1.0);
		rangeRoundControl_->setMinimum(0.0);
		rangeRoundControl_->setValue(analysisBlock_->rangeRound());
		rangeRoundControl_->blockSignals(false);

		correlationCenterBox_->blockSignals(true);
		correlationCenterBox_->setMaximum(inputSource->size(0) - 1);
		correlationCenterBox_->setValue(analysisBlock_->correlationCenterPixel());
		correlationCenterBox_->blockSignals(false);

		correlationPointsBox_->blockSignals(true);
		correlationPointsBox_->setValue(analysisBlock_->correlationHalfWidth()*2);
		correlationPointsBox_->blockSignals(false);

		correlationSmoothingBox_->blockSignals(true);
		correlationSmoothingBox_->setCurrentIndex(analysisBlock_->correlationSmoothing().first);
		correlationSmoothingBox_->blockSignals(false);

		smoothModeBox_->blockSignals(true);
		smoothModeBox_->setValue(analysisBlock_->correlationSmoothing().second);
		if(analysisBlock_->correlationSmoothing().first == 1)
			smoothModeBox_->setMinimum(2);
		smoothModeBox_->blockSignals(false);

		liveCorrelationCheckBox_->blockSignals(true);
		liveCorrelationCheckBox_->setChecked(analysisBlock_->liveCorrelation());
		liveCorrelationCheckBox_->blockSignals(false);

		shiftDisplayOffsetSlider_->blockSignals(true);
		shiftDisplayOffsetSlider_->setRange(0, analysisBlock_->size(0)-1);
		shiftDisplayOffsetSlider_->setValue(analysisBlock_->size(0)/2);
		shiftDisplayOffsetSlider_->blockSignals(false);

		image_ = new MPlotImageBasic();
		image_->setColorMap(*colorMap_);
		AMDataSourceImageData *model = new AMDataSourceImageData;
		model->setDataSource(inputSource);
		image_->setModel(model, true);
		plot_->addItem(image_);

		corrRegionLeft_->setValue(QPointF(analysisBlock_->correlationCenterPixel()-analysisBlock_->correlationHalfWidth(), 0));
		corrRegionRight_->setValue(QPointF(analysisBlock_->correlationCenterPixel()+analysisBlock_->correlationHalfWidth(), 0));
	}

	else {
		// no input source. Not much we can do.
		rangeMinYControl_->setEnabled(false);
		rangeMaxYControl_->setEnabled(false);
		rangeMinXControl_->setEnabled(false);
		rangeMaxXControl_->setEnabled(false);
		rangeRoundControl_->setEnabled(false);

		correlationCenterBox_->setEnabled(false);
		correlationPointsBox_->setEnabled(false);
		correlationSmoothingBox_->setEnabled(false);
		liveCorrelationCheckBox_->setEnabled(false);
		correlateNowButton_->setEnabled(false);
		shiftDisplayOffsetSlider_->setEnabled(false);
		energyCalibrationOffsetBox_->setEnabled(false);
		tiltCalibrationOffsetBox_->setEnabled(false);
		manualShiftEntryButton_->setEnabled(false);
	}

	placeRangeRectangle();
}

void REIXSXESImageABEditor::placeRangeRectangle()
{
	AMDataSource* inputSource;
	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		double dataYMin = inputSource->axisValue(1,0);
		double sumYMin = inputSource->axisValue(1, analysisBlock_->sumRangeMinY());
		double sumYMax = inputSource->axisValue(1, analysisBlock_->sumRangeMaxY());
		double dataYMax = inputSource->axisValue(1, inputSource->size(1)-1);

		double dataXMin = inputSource->axisValue(0,0);
		double sumXMin = inputSource->axisValue(0, analysisBlock_->sumRangeMinX());
		double sumXMax = inputSource->axisValue(0, analysisBlock_->sumRangeMaxX());
		double dataXMax = inputSource->axisValue(0, inputSource->size(0)-1);


		rangeRectangleY1_->setYAxisTarget(plot_->axisScaleLeft());	// note: does nothing if already correct
		rangeRectangleY2_->setYAxisTarget(plot_->axisScaleLeft());	// note: does nothing if already correct
		rangeRectangleY1_->setXAxisTarget(plot_->axisScaleHorizontalRelative());	// note: does nothing if already correct
		rangeRectangleY2_->setXAxisTarget(plot_->axisScaleHorizontalRelative());	// note: does nothing if already correct

		rangeRectangleX1_->setYAxisTarget(plot_->axisScaleLeft());	// note: does nothing if already correct
		rangeRectangleX2_->setYAxisTarget(plot_->axisScaleLeft());	// note: does nothing if already correct
		rangeRectangleX1_->setXAxisTarget(plot_->axisScaleBottom());	// note: does nothing if already correct
		rangeRectangleX2_->setXAxisTarget(plot_->axisScaleBottom());	// note: does nothing if already correct

		rangeRectangleY1_->setRect(QRectF(QPointF(0,dataYMin), QPointF(1,sumYMin)).normalized());
		rangeRectangleY2_->setRect(QRectF(QPointF(0,sumYMax), QPointF(1,dataYMax)).normalized());
		rangeRectangleX1_->setRect(QRectF(QPointF(dataXMin,sumYMin), QPointF(sumXMin,sumYMax)).normalized());
		rangeRectangleX2_->setRect(QRectF(QPointF(sumXMax,sumYMin), QPointF(dataXMax,sumYMax)).normalized());


		rangeRectangleY1_->setVisible(true);
		rangeRectangleY2_->setVisible(true);
		rangeRectangleX1_->setVisible(true);
		rangeRectangleX2_->setVisible(true);

	}
	else {
		rangeRectangleY1_->setVisible(false);
		rangeRectangleY2_->setVisible(false);
		rangeRectangleX1_->setVisible(false);
		rangeRectangleX2_->setVisible(false);
	}
}


/*
void REIXSXESImageABEditor::onShiftValuesChanged()
{
	AMIntList shiftValues = analysisBlock_->shiftValues();
	QVector<qreal> xs, ys;

	int midPixel = analysisBlock_->size(0)/2;
	// note: this is turned on its side, so the shift values become X, and the row values become Y.
	for(int j=0,cc=shiftValues.count(); j<cc; j++) {
		if(j>=analysisBlock_->sumRangeMin() && j<=analysisBlock_->sumRangeMax()) {
			xs << midPixel + shiftValues.at(j);
			ys << j;
		}
	}

	shiftData_->setValues(xs, ys);
}
*/


 REIXSXESImageABEditorEllipticalMask::~REIXSXESImageABEditorEllipticalMask(){}
REIXSXESImageABEditorEllipticalMask::REIXSXESImageABEditorEllipticalMask(REIXSXESImageAB *analysisBlock, QObject *parent)
	: QObject(parent)
{
	analysisBlock_ = analysisBlock;

	/*
	connect(analysisBlock_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));
	*/
}


qreal REIXSXESImageABEditorEllipticalMask::x(unsigned index) const
{
	if(analysisBlock_->rangeRound() == 0.0) {
		//rectangular masking
		if(index == 0 || index == 3 || index == 4)
			return (qreal)analysisBlock_->sumRangeMinX();
		else
			return (qreal)analysisBlock_->sumRangeMaxX();
	}
	else {
		//rounded mask
		if(index <= (unsigned(analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX()))) {
			//upper half of ellipse
			return (qreal)(analysisBlock_->sumRangeMinX() + index);
		}
		else {
			//lower half of ellipse (counting down x values)
			return (qreal)(analysisBlock_->sumRangeMaxX() - (index - (analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX())));
		}
	}
}

qreal REIXSXESImageABEditorEllipticalMask::y(unsigned index) const
{
	if(analysisBlock_->rangeRound() == 0.0) {
		//rectangular masking
		if(index == 0 || index == 1 || index == 4)
			return (qreal)analysisBlock_->sumRangeMaxY();
		else
			return (qreal)analysisBlock_->sumRangeMinY();
	}
	else {
		//rounded mask
		qreal dx = analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX();
		qreal dy = analysisBlock_->sumRangeMaxY() - analysisBlock_->sumRangeMinY();
		double D = analysisBlock_->rangeRound(); //just for shorter formulas
		qreal i = (qreal)index;

		//done to here
		if(index == 0 || index == unsigned(dx) || index == unsigned(2*dx)) {
			return (qreal)analysisBlock_->sumRangeMinY() + dy/2.0;
		}
		else if(i < D * dx/2.0) {
			//upper left elliptical part
			return sqrt(1.0 - ((i-D*dx/2.0)/(D*dx/2.0))*((i-D*dx/2.0)/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0) + (analysisBlock_->sumRangeMinY()) + dy/2.0;
		}
		else if(i < dx - D * dx/2.0) {
			//upper flat part
			return analysisBlock_->sumRangeMaxY();
		}
		else if(i < dx) {
			//upper right elliptical part
			return sqrt(1.0 - ((i-(dx-D*dx/2.0))/(D*dx/2.0))*((i-(dx-D*dx/2.0))/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0) + (analysisBlock_->sumRangeMinY()) + dy/2.0;
		}
		else if(i < dx + D * dx/2.0) {
			//lower right elliptical part
			return -(sqrt(1.0 - ((i-(dx + D*dx/2.0))/(D*dx/2.0))*((i-(dx+D*dx/2.0))/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0)) + ((analysisBlock_->sumRangeMinY()) + dy/2.0);
		}
		else if(i < 2*dx - D * dx/2.0) {
			//lower flat part
			return analysisBlock_->sumRangeMinY();
		}
		else if(i < 2*dx) {
			//lower left elliptical part
			return -(sqrt(1.0 - ((i- (2*dx-D*dx/2.0))/(D*dx/2.0))*((i-(2*dx-D*dx/2.0))/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0)) + ((analysisBlock_->sumRangeMinY()) + dy/2.0);
		}
		else {
			//index out of range...shouldn't happen
			return -1.0;
		}
	}
}

int REIXSXESImageABEditorEllipticalMask::count() const
{
	if(analysisBlock_->rangeRound() == 0.0) {
		return 5; //rectangular masking, 5th point is to close rectangle (equals first point)
	}
	else {
		//rounded mask
		return (analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX() + 1)*2 - 1; //note that we include the first point twice, to close the ellipse
	}
}

void REIXSXESImageABEditorEllipticalMask::rangeValuesChanged()
{
	emitDataChanged();
}

/*
void REIXSXESImageABEditorEllipticalMask::onOutputSizeChanged()
{
	emitDataChanged();
}
*/

void REIXSXESImageABEditorEllipticalMask::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned index=indexStart; index<=indexEnd; ++index) {
		if(analysisBlock_->rangeRound() == 0.0) {
			//rectangular masking
			if(index == 0 || index == 3 || index == 4)
				*(outputValues++) = (qreal)analysisBlock_->sumRangeMinX();
			else
				*(outputValues++) = (qreal)analysisBlock_->sumRangeMaxX();
		}
		else {
			//rounded mask
			if(index <= unsigned(analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX())) {
				//upper half of ellipse
				*(outputValues++) = (qreal)(analysisBlock_->sumRangeMinX() + index);
			}
			else {
				//lower half of ellipse (counting down x values)
				*(outputValues++) = (qreal)(analysisBlock_->sumRangeMaxX() - (index - (analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX())));
			}
		}
	}
}

void REIXSXESImageABEditorEllipticalMask::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned index=indexStart; index<=indexEnd; ++index) {
		*(outputValues++) = REIXSXESImageABEditorEllipticalMask::y(index);

		/*
		if(analysisBlock_->rangeRound() == 0.0) {
			//rectangular masking
			if(index == 0 || index == 1 || index == 4)
				*(outputValues++) = (qreal)analysisBlock_->sumRangeMaxY();
			else
				*(outputValues++) = (qreal)analysisBlock_->sumRangeMinY();
		}
		else {
			//rounded mask
			qreal dx = analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX();
			qreal dy = analysisBlock_->sumRangeMaxY() - analysisBlock_->sumRangeMinY();
			double D = analysisBlock_->rangeRound(); //just for shorter formulas
			qreal i = (qreal)index;

			//done to here
			if(index == 0 || index == (int)dx || index == 2*(int)dx) {
				*(outputValues++) = (qreal)analysisBlock_->sumRangeMinY() + dy/2.0;
			}
			else if(i < D * dx/2.0) {
				//upper left elliptical part
				*(outputValues++) = sqrt(1.0 - ((i-D*dx/2.0)/(D*dx/2.0))*((i-D*dx/2.0)/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0) + (analysisBlock_->sumRangeMinY()) + dy/2.0;
			}
			else if(i < dx - D * dx/2.0) {
				//upper flat part
				*(outputValues++) = analysisBlock_->sumRangeMaxY();
			}
			else if(i < dx) {
				//upper right elliptical part
				*(outputValues++) = sqrt(1.0 - ((i-(dx-D*dx/2.0))/(D*dx/2.0))*((i-(dx-D*dx/2.0))/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0) + (analysisBlock_->sumRangeMinY()) + dy/2.0;
			}
			else if(i < dx + D * dx/2.0) {
				//lower right elliptical part
qDebug() << "Lower right index - value:" << i << -(sqrt(-1.0 + ((i-(dx-D*dx/2.0))/(D*dx/2.0))*((i-(dx-D*dx/2.0))/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0) + (analysisBlock_->sumRangeMinY()) + dy/2.0);
				*(outputValues++) = -(sqrt(1.0 - ((i-(dx-D*dx/2.0))/(D*dx/2.0))*((i-(dx-D*dx/2.0))/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0) + (analysisBlock_->sumRangeMinY()) + dy/2.0);
			}
			else if(i < 2*dx - D * dx/2.0) {
				//lower flat part
				*(outputValues++) = analysisBlock_->sumRangeMinY();
			}
			else if(i < 2*dx) {
				//lower left elliptical part
				*(outputValues++) = -(sqrt(1.0 - ((i-D*dx/2.0)/(D*dx/2.0))*((i-D*dx/2.0)/(D*dx/2.0)))*(D*dy/2.0) + ((1.0-D)*dy/2.0) + (analysisBlock_->sumRangeMinY()) + dy/2.0);
			}
			else {
				//index out of range...shouldn't happen
				*(outputValues++) = -1.0;
			}
		}
		*/

	}
}



























 REIXSXESImageABEditorShiftModel::~REIXSXESImageABEditorShiftModel(){}
REIXSXESImageABEditorShiftModel::REIXSXESImageABEditorShiftModel(REIXSXESImageAB *analysisBlock, QObject *parent)
	: QObject(parent)
{
	analysisBlock_ = analysisBlock;
	displayXOffset_ = analysisBlock_->size(0)/2;

	connect(analysisBlock_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));
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

void REIXSXESImageABEditorShiftModel::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned i=indexStart; i<=indexEnd; ++i)
		*(outputValues++) = analysisBlock_->shiftValues().at(i) + displayXOffset_;
}

void REIXSXESImageABEditorShiftModel::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned i=indexStart; i<=indexEnd; ++i)
		*(outputValues++) = i;
}

void REIXSXESImageABEditor::onApplyToOtherScansMenuClicked()
{
	if(!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Choose scans...", "Choose the scans you want to apply these analysis parameters to.", this);
		/*
		chooseScanDialog_->dataView_->setOrganizeMode(AMDataViews::OrganizeScanTypes);
		*/
		chooseScanDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
	}
	connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onApplyToOtherScansChosen()));

	// User feedback on what will happen:
	QStringList operationsCompleted;
	if(batchApplySumRange_->isChecked())
		operationsCompleted << "Sum Range";
	if(batchApplyCalibrationOffsets_->isChecked())
		operationsCompleted << "Calibration offsets (energy, tilt)";
	if(batchApplyCorrelationSettings_->isChecked())
		operationsCompleted << "Correlation Settings";
	if(batchApplyShiftCurve_->isChecked())
		operationsCompleted << "Shift Curve";

	chooseScanDialog_->setPrompt(QString("Choose the scans you want to apply these analysis parameters (%1) to.").arg(operationsCompleted.join(", ")));
	chooseScanDialog_->show();
}

#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include <QProgressDialog>
#include <QUrl>

void REIXSXESImageABEditor::onApplyToOtherScansChosen()
{
	disconnect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onApplyToOtherScansChosen()));

	QList<QUrl> scans = chooseScanDialog_->getSelectedScans();
	if(scans.isEmpty()) {
		chooseScanDialog_->close();
		return;
	}

	QProgressDialog* progressDialog = new QProgressDialog(QString("Applying analysis parameters to %1 scans...").arg(scans.count()), "Stop", 0, scans.count(), chooseScanDialog_);
	progressDialog->setMinimumDuration(0);

	int scansModified = 0;

	for(int s=0; s<scans.count(); ++s) {
		progressDialog->setValue(s);
		QUrl scanUrl = scans.at(s);
		bool isScanning;
		QString scanName;
		AMScan* scan = AMScan::createFromDatabaseUrl(scanUrl, false, &isScanning, &scanName);
		if(isScanning) {
			AMErrorMon::alert(this, -333, QString("Batch shifting: Cannot apply shift parameters to scan '%1' because it's still acquiring data.").arg(scanName));
			continue;
		}

		if(!scan) {
			AMErrorMon::alert(this, -334, QString("Batch shifting: Could not open the scan at '%1'. This looks like a problem in the database; please report this problem to the REIXS Acquaman developers.").arg(scanUrl.toString()));
			continue;
		}

		bool xesABFound = false;
		for(int i=0, cc=scan->analyzedDataSourceCount(); i<cc; ++i) {
			REIXSXESImageAB* xesAB = qobject_cast<REIXSXESImageAB*>(scan->analyzedDataSources()->at(i));
			if(xesAB) {
				xesABFound = true;
				if(batchApplySumRange_->isChecked()) {
					xesAB->setSumRangeMaxY(analysisBlock_->sumRangeMaxY());
					xesAB->setSumRangeMinY(analysisBlock_->sumRangeMinY());
					xesAB->setSumRangeMaxX(analysisBlock_->sumRangeMaxX());
					xesAB->setSumRangeMinX(analysisBlock_->sumRangeMinX());
				}
				if(batchApplyCalibrationOffsets_->isChecked()) {
					xesAB->setEnergyCalibrationOffset(analysisBlock_->energyCalibrationOffset());
					xesAB->setTiltCalibrationOffset(analysisBlock_->tiltCalibrationOffset());
				}
				if(batchApplyCorrelationSettings_->isChecked()) {
					xesAB->setCorrelationCenterPixel(analysisBlock_->correlationCenterPixel());
					xesAB->setCorrelationHalfWidth(analysisBlock_->correlationHalfWidth());
					xesAB->setCorrelationSmoothing(analysisBlock_->correlationSmoothing());
					xesAB->enableLiveCorrelation(analysisBlock_->liveCorrelation());
				}
				if(batchApplyShiftCurve_->isChecked()) {
					xesAB->setShiftValues(analysisBlock_->shiftValues());
				}
			}
		}

		if(!xesABFound) {
			AMErrorMon::alert(this, -335, QString("Batch shifting: Could not find an analyzed XES image in the scan '%1'").arg(scan->fullName()));
		}
		else {
			scansModified++;
			scan->storeToDb(scan->database());
		}

		scan->deleteLater();	// delete scan.
	}

	progressDialog->setValue(scans.count());
	progressDialog->deleteLater();

	// User feedback on what just happened:
	QStringList operationsCompleted;
	if(batchApplySumRange_->isChecked())
		operationsCompleted << "Sum Range";
	if(batchApplyCalibrationOffsets_->isChecked())
		operationsCompleted << "Calibration offsets (energy, tilt)";
	if(batchApplyCorrelationSettings_->isChecked())
		operationsCompleted << "Correlation Settings";
	if(batchApplyShiftCurve_->isChecked())
		operationsCompleted << "Shift Curve";

	AMErrorMon::information(this, 0, QString("Batch shifting: set the analysis parameters (%1) successfully for %2 XES scans.").arg(operationsCompleted.join(", ")).arg(scansModified));
	chooseScanDialog_->close();
}

#include <QInputDialog>
#include <QRegExp>
#include <QApplication>
void REIXSXESImageABEditor::onManualShiftEntryButtonClicked()
{
	// Grab the current shift and convert to text.
	QStringList shifts;
	foreach(int d, analysisBlock_->shiftValues())
		shifts << QString::number(d);

	bool ok;
	QString newShiftString = QInputDialog::getText(this, "Set shift values manually", QString("Please enter the desired shift amounts for each line (%1)").arg(shifts.count()), QLineEdit::Normal, shifts.join(";"), &ok);
	if(!ok)
		return;

	QStringList newShifts = newShiftString.split(QRegExp("[,; ]+"), QString::SkipEmptyParts);
	if(newShifts.count() != shifts.count()) {
		QApplication::beep();
		AMErrorMon::alert(this, -370, QString("Could not set the shift values manually: you need to provide %1 shift numbers.").arg(shifts.count()));
		return;
	}

	AMIntList newShiftNumbers;
	foreach(QString s, newShifts) {
		newShiftNumbers << s.toInt(&ok);
		if(!ok) {
			AMErrorMon::alert(this, -370, QString("Could not set the shift values manually: '%1' is not a number.").arg(s));
			return;
		}
	}

	analysisBlock_->setShiftValues(newShiftNumbers);
}
