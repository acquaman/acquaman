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


#include "REIXSXESImageInterpolationABEditor.h"

#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QToolButton>
#include <QTabWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QMessageBox>
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

#include "analysis/REIXS/REIXSXESImageInterpolationAB.h"

#include <QAction>
#include <QMenu>

REIXSXESImageInterpolationABEditor::REIXSXESImageInterpolationABEditor(REIXSXESImageInterpolationAB *analysisBlock, QWidget *parent) :
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

	binningLevelBox_ = new QSpinBox();
	binningLevelBox_->setMinimum(1);
	binningLevelBox_->setMaximum(32);
	binningLevelBox_->setMaximumWidth(64);


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

	curve2DisabledCheckBox_ = new QCheckBox("Disable");

	correlation1CenterBox_ = new QSpinBox();
	correlation1CenterBox_->setSingleStep(1);
	correlation1CenterBox_->setMinimum(1);
	correlation1PointsBox_ = new QSpinBox();
	correlation1PointsBox_->setMinimum(10);
	correlation1PointsBox_->setMaximum(500);
	correlation1PointsBox_->setValue(200);

	correlation2CenterBox_ = new QSpinBox();
	correlation2CenterBox_->setSingleStep(1);
	correlation2CenterBox_->setMinimum(1);
	correlation2PointsBox_ = new QSpinBox();
	correlation2PointsBox_->setMinimum(10);
	correlation2PointsBox_->setMaximum(500);
	correlation2PointsBox_->setValue(200);

	correlation1SmoothingBox_ = new QComboBox();
	correlation1SmoothingBox_->addItem("None");
	correlation1SmoothingBox_->addItem("Poly Fit");
	correlation1SmoothingBox_->addItem("Moving Median");
	correlation1SmoothingBox_->addItem("Moving Average");

	correlation2SmoothingBox_ = new QComboBox();
	correlation2SmoothingBox_->addItem("None");
	correlation2SmoothingBox_->addItem("Poly Fit");
	correlation2SmoothingBox_->addItem("Moving Median");
	correlation2SmoothingBox_->addItem("Moving Average");

	smooth1ModelBox_ = new QSpinBox();
	smooth1ModelBox_->setSingleStep(1);
	smooth1ModelBox_->setMinimum(1);
	smooth1ModelBox_->setValue(1);
	smooth1ModelBox_->setMaximum(99);

	smooth2Mode1Box_ = new QSpinBox();
	smooth2Mode1Box_->setSingleStep(1);
	smooth2Mode1Box_->setMinimum(1);
	smooth2Mode1Box_->setValue(1);
	smooth2Mode1Box_->setMaximum(99);

	liveCorrelationCheckBox_ = new QCheckBox("Real-time");
	correlateNowButton_ = new QPushButton("Now");

	shift1LineEdit_ = new QLineEdit();
	shift2LineEdit_ = new QLineEdit();

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
	colorMap_ = new MPlotColorMap(MPlotColorMap::Bone);
	colorMap_->setBrightness(0.08);
	colorMap_->setContrast(2.1);
	colorMap_->setGamma(1.0);

	shiftData_ = new REIXSXESImageInterpolationABEditorShiftModel(analysisBlock_);
	shiftSeries_ = new MPlotSeriesBasic();
	shiftSeries_->setModel(shiftData_, true);
	shiftSeries_->setIgnoreWhenAutoScaling(true);
	shiftSeries_->setLinePen(QPen(QBrush(QColor(Qt::magenta)),2));
	shiftSeries_->setMarker(MPlotMarkerShape::None);
	plot_->addItem(shiftSeries_);
	shiftSeries_->setZValue(2999);	// put on top of plot, but below range rectangles.

	shift1Data_ = new REIXSXESImageInterpolationABEditorShift1Model(analysisBlock_);
	shift1Series_ = new MPlotSeriesBasic();
	shift1Series_->setModel(shift1Data_, true);
	shift1Series_->setIgnoreWhenAutoScaling(true);
	shift1Series_->setLinePen(QPen(QBrush(QColor(Qt::red)),2));
	shift1Series_->setMarker(MPlotMarkerShape::None);
	plot_->addItem(shift1Series_);
	shift1Series_->setZValue(2994);	// put on top of plot, but below range rectangles.

	shift2Data_ = new REIXSXESImageInterpolationABEditorShift2Model(analysisBlock_);
	shift2Series_ = new MPlotSeriesBasic();
	shift2Series_->setModel(shift2Data_, true);
	shift2Series_->setIgnoreWhenAutoScaling(true);
	shift2Series_->setLinePen(QPen(QBrush(QColor(Qt::green)),2));
	shift2Series_->setMarker(MPlotMarkerShape::None);
	plot_->addItem(shift2Series_);
	shift2Series_->setZValue(2993);	// put on top of plot, but below range rectangles.



	ellipseData_ = new REIXSXESImageInterpolationABEditorEllipticalMask(analysisBlock_);
	ellipseSeries_ = new MPlotSeriesBasic();
	ellipseSeries_->setModel(ellipseData_, true);
	ellipseSeries_->setIgnoreWhenAutoScaling(true);
	ellipseSeries_->setLinePen(QPen(QBrush(QColor(Qt::white)),1));
	ellipseSeries_->setMarker(MPlotMarkerShape::None);
	plot_->addItem(ellipseSeries_);
	ellipseSeries_->setZValue(2999);	// put on top of plot, but below range rectangles.




	corrRegion1Left_ = new MPlotPoint(QPointF(analysisBlock_->correlation1CenterPixel()-analysisBlock_->correlation1HalfWidth(), 0));
	corrRegion1Right_ = new MPlotPoint(QPointF(analysisBlock_->correlation1CenterPixel()+analysisBlock_->correlation1HalfWidth(), 0));
	corrRegion1Left_->setMarker(MPlotMarkerShape::VerticalBeam, 2000, QPen(QColor(128,32,32)));
	corrRegion1Right_->setMarker(MPlotMarkerShape::VerticalBeam, 2000, QPen(QColor(128,32,32)));
	corrRegion1Left_->setIgnoreWhenAutoScaling(true);
	corrRegion1Right_->setIgnoreWhenAutoScaling(true);
	plot_->addItem(corrRegion1Left_);
	plot_->addItem(corrRegion1Right_);
	corrRegion1Left_->setZValue(2998);
	corrRegion1Right_->setZValue(2997);

	corrRegion2Left_ = new MPlotPoint(QPointF(analysisBlock_->correlation2CenterPixel()-analysisBlock_->correlation2HalfWidth(), 0));
	corrRegion2Right_ = new MPlotPoint(QPointF(analysisBlock_->correlation2CenterPixel()+analysisBlock_->correlation2HalfWidth(), 0));
	corrRegion2Left_->setMarker(MPlotMarkerShape::VerticalBeam, 2000, QPen(QColor(32,128,32)));
	corrRegion2Right_->setMarker(MPlotMarkerShape::VerticalBeam, 2000, QPen(QColor(32,128,32)));
	corrRegion2Left_->setIgnoreWhenAutoScaling(true);
	corrRegion2Right_->setIgnoreWhenAutoScaling(true);
	plot_->addItem(corrRegion2Left_);
	plot_->addItem(corrRegion2Right_);
	corrRegion2Left_->setZValue(2996);
	corrRegion2Right_->setZValue(2995);


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

	QVBoxLayout* mainLayout = new QVBoxLayout();
		QTabWidget* tabWidget_= new QTabWidget();


		//START OF MASK PAGE
				QWidget* maskGroupBox = new QWidget();
								QFormLayout* maskRangeLayout = new QFormLayout();
									QHBoxLayout* yMaskRangeLayout = new QHBoxLayout();
										yMaskRangeLayout->addWidget(rangeMinYControl_);
										yMaskRangeLayout->addWidget(new QLabel("To"));
										yMaskRangeLayout->addWidget(rangeMaxYControl_);
								maskRangeLayout->addRow("From (y):", yMaskRangeLayout);
									QHBoxLayout* xMaskRangeLayout = new QHBoxLayout();
										xMaskRangeLayout->addWidget(rangeMinXControl_);
										xMaskRangeLayout->addWidget(new QLabel("To"));
										xMaskRangeLayout->addWidget(rangeMaxXControl_);
								maskRangeLayout->addRow("From (x):", xMaskRangeLayout);
								maskRangeLayout->addRow("Round:", rangeRoundControl_);
						maskGroupBox->setLayout(maskRangeLayout);
	//END OF MASK PAGE LAYOUT
	tabWidget_->addTab(maskGroupBox,"Mask");



	//START OF CALIBRATION PAGE
	QWidget* calibrationGroupBox = new QWidget();
			QFormLayout* calibrationLayout = new QFormLayout();
				QHBoxLayout* calibrationSpinnerLayout = new QHBoxLayout();
					calibrationSpinnerLayout->addWidget(energyCalibrationOffsetBox_);
					calibrationSpinnerLayout->addWidget(tiltCalibrationOffsetBox_);
				calibrationLayout->addRow("Offset:",calibrationSpinnerLayout);
				calibrationLayout->addRow("Binning:",binningLevelBox_);
			calibrationGroupBox->setLayout(calibrationLayout);
		//END OF CALIBRATION PAGE LAYOUT
	tabWidget_->addTab(calibrationGroupBox,"Cal/Bin");



	//START OF SHIFT 1 PAGE LAYOUT
		QWidget* shift1PageWidget_ = new QWidget();
			QVBoxLayout* shift1PageLayout = new QVBoxLayout();
				QFormLayout* shift1PageFormLayout = new QFormLayout();
					QHBoxLayout* correlation1SettingsLayout = new QHBoxLayout();
						correlation1SettingsLayout->addWidget(correlation1CenterBox_);
						correlation1SettingsLayout->addWidget(new QLabel("<b><FONT COLOR='#802020' FONT SIZE = 3>Width:</b>"));
						correlation1SettingsLayout->addWidget(correlation1PointsBox_);

				shift1PageFormLayout->addRow("<b><FONT COLOR='#ff0000' FONT SIZE = 3>Center:</b>", correlation1SettingsLayout);
					QHBoxLayout* correlation1SmothingLayout = new QHBoxLayout();
						correlation1SmothingLayout->addWidget(correlation1SmoothingBox_);
						correlation1SmothingLayout->addWidget(smooth1ModelBox_);
				shift1PageFormLayout->addRow("C. smooth:", correlation1SmothingLayout);
			shift1PageLayout->addLayout(shift1PageFormLayout);

			QHBoxLayout* shiftButtonsLayout = new QHBoxLayout();
				QLabel* manualShift1Label = new QLabel("Manual shift:");
				shiftButtonsLayout->addWidget(manualShift1Label);
							shiftButtonsLayout->addWidget(shift1LineEdit_);
			shift1PageLayout->addLayout(shiftButtonsLayout);
		shift1PageWidget_->setLayout(shift1PageLayout);
	//END OF SHIFT 1 PAGE LAYOUT
	tabWidget_->addTab(shift1PageWidget_,"Curve 1");



	//START OF SHIFT 2 PAGE LAYOUT
		QWidget* shift2PageWidget_ = new QWidget();
			QVBoxLayout* shift2PageLayout = new QVBoxLayout();
				QFormLayout* shift2PageFormLayout = new QFormLayout();
					QHBoxLayout* correlation2SettingsLayout = new QHBoxLayout();
						correlation2SettingsLayout->addWidget(correlation2CenterBox_);
						correlation2SettingsLayout->addWidget(new QLabel("<b><FONT COLOR='#208020' FONT SIZE = 3>Width:</b>"));
						correlation2SettingsLayout->addWidget(correlation2PointsBox_);
				shift2PageFormLayout->addRow("<b><FONT COLOR='#00ff00' FONT SIZE = 3>Center:</b>", correlation2SettingsLayout);
					QHBoxLayout* correlation2SmothingLayout = new QHBoxLayout();
						correlation2SmothingLayout->addWidget(correlation2SmoothingBox_);
						correlation2SmothingLayout->addWidget(smooth2Mode1Box_);
				shift2PageFormLayout->addRow("C. smooth:", correlation2SmothingLayout);
			shift2PageLayout->addLayout(shift2PageFormLayout);

			QHBoxLayout* shiftButtons2Layout = new QHBoxLayout();
				QLabel* manualShift2Label = new QLabel("Manual shift:");
				shiftButtons2Layout->addWidget(manualShift2Label);
				shiftButtons2Layout->addWidget(shift2LineEdit_);
				shiftButtons2Layout->addWidget(curve2DisabledCheckBox_);
			shift2PageLayout->addLayout(shiftButtons2Layout);
		shift2PageWidget_->setLayout(shift2PageLayout);
	//END OF SHIFT 2 PAGE LAYOUT
	tabWidget_->addTab(shift2PageWidget_,"Curve 2");

	//START OF APPLY TO PAGE LAYOUT
			QGroupBox* applyGroupBox = new QGroupBox("Apply to other Scans:");
				QVBoxLayout* applyPageLayout = new QVBoxLayout();
					QHBoxLayout* applyPageColumnLayout = new QHBoxLayout();
						batchApplyCorrelationSettings_ = new QCheckBox("Curve Settings");
						batchApplyShiftCurve_ = new QCheckBox("Curves");
						batchApplySumRange_ = new QCheckBox("Mask");
						batchApplyCalibrationOffsets_ = new QCheckBox("Calibration");
						batchApplyCorrelationSettings_->setCheckable(true);
						batchApplyCorrelationSettings_->setChecked(true);
						batchApplyShiftCurve_->setCheckable(true);
						batchApplyCalibrationOffsets_->setCheckable(true);
						batchApplyShiftCurve_->setChecked(true);
						batchApplySumRange_->setCheckable(true);
						batchApplySumRange_->setChecked(true);
						batchApplyCalibrationOffsets_->setChecked(true);
						applyToOtherScansButton_ = new QPushButton();
						applyToOtherScansButton_->setText("Choose Scans...");
						QVBoxLayout* applyPageLeftLayout = new QVBoxLayout();
							applyPageLeftLayout->addWidget(batchApplySumRange_);
							applyPageLeftLayout->addWidget(batchApplyShiftCurve_);
					applyPageColumnLayout->addLayout(applyPageLeftLayout);
						QVBoxLayout* applyPageRightLayout = new QVBoxLayout();
							applyPageRightLayout->addWidget(batchApplyCalibrationOffsets_);
							applyPageRightLayout->addWidget(batchApplyCorrelationSettings_);
					applyPageColumnLayout->addLayout(applyPageRightLayout);
				applyPageLayout->addLayout(applyPageColumnLayout);
				applyPageLayout->addWidget(applyToOtherScansButton_);
			applyGroupBox->setLayout(applyPageLayout);
		//END OF APPLY TO PAGE LAYOUT
		tabWidget_->addTab(applyGroupBox,"Apply");



	//Shared curve fit buttons
	mainLayout->addWidget(tabWidget_);
	QWidget* correlation1ButtonsWidget = new QWidget();
		QHBoxLayout* correlation1ButtonsLayout = new QHBoxLayout();
			correlation1ButtonsLayout->addWidget(new QLabel("Re-Fit Curves:"));
			correlation1ButtonsLayout->addWidget(liveCorrelationCheckBox_);
			correlation1ButtonsLayout->addWidget(correlateNowButton_);
		correlation1ButtonsWidget->setLayout(correlation1ButtonsLayout);
	mainLayout->addWidget(correlation1ButtonsWidget);

	mainLayout->addWidget(plotWidget_);
	mainLayout->addWidget(shiftDisplayOffsetSlider_);

	setLayout(mainLayout);

	onAnalysisBlockInputDataSourcesChanged();

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

	connect(curve2DisabledCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onCurve2DisabledCheckBoxChanged(bool)));

	connect(correlation1CenterBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelation1CenterBoxChanged(int)));
	connect(correlation1PointsBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelation1PointsBoxChanged(int)));
	connect(correlation2CenterBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelation2CenterBoxChanged(int)));
	connect(correlation2PointsBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelation2PointsBoxChanged(int)));


	connect(shiftDisplayOffsetSlider_, SIGNAL(valueChanged(int)), shiftData_, SLOT(setDisplayXOffset(int)));

	//connect(manualShiftEntryButton_, SIGNAL(clicked()), this, SLOT(onManualShiftEntryButtonClicked()));
	connect(shift1LineEdit_, SIGNAL(editingFinished()), this, SLOT(onShift1LineEdited()));
	connect(shift2LineEdit_, SIGNAL(editingFinished()), this, SLOT(onShift2LineEdited()));
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));


	// direct connections:
	connect(correlateNowButton_, SIGNAL(clicked()), analysisBlock_, SLOT(correlateNow()));
	connect(liveCorrelationCheckBox_, SIGNAL(toggled(bool)), analysisBlock_, SLOT(enableLiveCorrelation(bool)));
	connect(energyCalibrationOffsetBox_, SIGNAL(valueChanged(double)), analysisBlock_, SLOT(setEnergyCalibrationOffset(double)));
	connect(tiltCalibrationOffsetBox_, SIGNAL(valueChanged(double)), analysisBlock_, SLOT(setTiltCalibrationOffset(double)));

	/*
	connect(correlationSmoothingBox_, SIGNAL(currentIndexChanged(int)), analysisBlock_, SLOT(setCorrelationSmoothing(int)));
	*/

	connect(correlation1SmoothingBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCSmoothBox1Changed()));
	connect(correlation2SmoothingBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCSmoothBox2Changed()));
	connect(binningLevelBox_, SIGNAL(valueChanged(int)), this, SLOT(onBinningLevelBoxChanged(int)));
	connect(smooth1ModelBox_, SIGNAL(valueChanged(int)), this, SLOT(onCSmooth1ModeChanged()));
	connect(smooth2Mode1Box_, SIGNAL(valueChanged(int)), this, SLOT(onCSmooth2ModeChanged()));


	connect(applyToOtherScansButton_, SIGNAL(clicked()), this, SLOT(onApplyToOtherScansMenuClicked()));


}

REIXSXESImageInterpolationABEditor::~REIXSXESImageInterpolationABEditor() {
	delete colorMap_;
}

void REIXSXESImageInterpolationABEditor::onRangeMinYControlChanged(int newRangeMinY)
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


void REIXSXESImageInterpolationABEditor::onRangeMaxYControlChanged(int newRangeMaxY)
{
	if(newRangeMaxY == analysisBlock_->sumRangeMaxY())
		return;

	analysisBlock_->setSumRangeMaxY(newRangeMaxY);

	if(rangeMinYControl_->value() > newRangeMaxY)
		rangeMinYControl_->setValue(newRangeMaxY);

	placeRangeRectangle();
	ellipseData_->rangeValuesChanged();

}

void REIXSXESImageInterpolationABEditor::onRangeMinXControlChanged(int newRangeMinX)
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


void REIXSXESImageInterpolationABEditor::onRangeMaxXControlChanged(int newRangeMaxX)
{
	if(newRangeMaxX == analysisBlock_->sumRangeMaxX())
		return;

	analysisBlock_->setSumRangeMaxX(newRangeMaxX);

	if(rangeMinXControl_->value() > newRangeMaxX)
		rangeMinXControl_->setValue(newRangeMaxX);

	placeRangeRectangle();
	ellipseData_->rangeValuesChanged();

}

void REIXSXESImageInterpolationABEditor::onRangeRoundControlChanged(double newRangeRound)
{
	if(newRangeRound == analysisBlock_->rangeRound())
		return;

	analysisBlock_->setRangeRound(newRangeRound);

	//update ellipse...
	placeRangeRectangle();
	ellipseData_->rangeValuesChanged();

}

void REIXSXESImageInterpolationABEditor::onCurve2DisabledCheckBoxChanged(bool disabled)
{
	if(disabled == analysisBlock_->curve2Disabled())
		return;

	correlation2CenterBox_->setDisabled(disabled);
	correlation2PointsBox_->setDisabled(disabled);
	correlation2SmoothingBox_->setDisabled(disabled);
	smooth2Mode1Box_->setDisabled(disabled);
	shift2LineEdit_->setDisabled(disabled);

	shift1Series_->setVisible(!disabled);
	shift2Series_->setVisible(!disabled);
	corrRegion2Left_->setVisible(!disabled);
	corrRegion2Right_->setVisible(!disabled);


	analysisBlock_->setCurve2Disabled(disabled);

}



void REIXSXESImageInterpolationABEditor::onCorrelation1CenterBoxChanged(int center)
{
	if(center == analysisBlock_->correlation1CenterPixel())
		return;

	if(center == analysisBlock_->correlation2CenterPixel() && center == 1)
		center = 2;
	else if(center == analysisBlock_->correlation2CenterPixel() && center == 1023)
		center = 1022;
	else if(center == analysisBlock_->correlation2CenterPixel())
		center--;

	analysisBlock_->setCorrelation1CenterPixel(center);

	// update lines showing correlation range.
	corrRegion1Left_->setValue(QPointF(analysisBlock_->correlation1CenterPixel()-analysisBlock_->correlation1HalfWidth(), 0));
	corrRegion1Right_->setValue(QPointF(analysisBlock_->correlation1CenterPixel()+analysisBlock_->correlation1HalfWidth(), 0));
}

void REIXSXESImageInterpolationABEditor::onCorrelation2CenterBoxChanged(int center)
{
	if(center == analysisBlock_->correlation2CenterPixel())
		return;

	if(center == analysisBlock_->correlation1CenterPixel() && center == 1)
		center = 2;
	else if(center == analysisBlock_->correlation1CenterPixel() && center == 1023)
		center = 1022;
	else if(center == analysisBlock_->correlation1CenterPixel())
		center++;


	analysisBlock_->setCorrelation2CenterPixel(center);

	// update lines showing correlation range.
	corrRegion2Left_->setValue(QPointF(analysisBlock_->correlation2CenterPixel()-analysisBlock_->correlation2HalfWidth(), 0));
	corrRegion2Right_->setValue(QPointF(analysisBlock_->correlation2CenterPixel()+analysisBlock_->correlation2HalfWidth(), 0));
}



void REIXSXESImageInterpolationABEditor::onCSmoothBox1Changed()
{
	//index 0 for none, 1 for poly, 2 for median, 3 for average

	switch((smoothBoxType)correlation1SmoothingBox_->currentIndex()) {
	case None:
		smooth1ModelBox_->setDisabled(true);
		break;
	case Poly:
		smooth1ModelBox_->setEnabled(true);
		smooth1ModelBox_->setMinimum(2);
		smooth1ModelBox_->setMaximum(4);
		smooth1ModelBox_->setSingleStep(1);
		smooth1ModelBox_->setValue(2);
		break;
	case Median:
		smooth1ModelBox_->setEnabled(true);
		smooth1ModelBox_->setMinimum(1);
		smooth1ModelBox_->setMaximum(63);
		smooth1ModelBox_->setSingleStep(2);
		smooth1ModelBox_->setValue(3);
		break;
	case Average:
		smooth1ModelBox_->setEnabled(true);
		smooth1ModelBox_->setMinimum(1);
		smooth1ModelBox_->setMaximum(63);
		smooth1ModelBox_->setSingleStep(2);
		smooth1ModelBox_->setValue(3);
		break;
	default:
		break;
	}

	analysisBlock_->setCorrelation1Smoothing(QPair<int,int>(correlation1SmoothingBox_->currentIndex(), smooth1ModelBox_->value()));
}


void REIXSXESImageInterpolationABEditor::onCSmoothBox2Changed()
{
	//index 0 for none, 1 for poly, 2 for median, 3 for average


	switch((smoothBoxType)correlation2SmoothingBox_->currentIndex()) {
	case None:
		smooth2Mode1Box_->setDisabled(true);
		break;
	case Poly:
		smooth2Mode1Box_->setEnabled(true);
		smooth2Mode1Box_->setMinimum(2);
		smooth2Mode1Box_->setMaximum(4);
		smooth2Mode1Box_->setSingleStep(1);
		smooth2Mode1Box_->setValue(2);
		break;
	case Median:
		smooth2Mode1Box_->setEnabled(true);
		smooth2Mode1Box_->setMinimum(1);
		smooth2Mode1Box_->setMaximum(63);
		smooth2Mode1Box_->setSingleStep(2);
		smooth2Mode1Box_->setValue(3);
		break;
	case Average:
		smooth2Mode1Box_->setEnabled(true);
		smooth2Mode1Box_->setMinimum(1);
		smooth2Mode1Box_->setMaximum(63);
		smooth2Mode1Box_->setSingleStep(2);
		smooth2Mode1Box_->setValue(3);
		break;
	default:
		break;
	}

	analysisBlock_->setCorrelation2Smoothing(QPair<int,int>(correlation2SmoothingBox_->currentIndex(), smooth2Mode1Box_->value()));
}


void REIXSXESImageInterpolationABEditor::onCSmooth1ModeChanged() {
	analysisBlock_->setCorrelation1Smoothing(QPair<int,int>(correlation1SmoothingBox_->currentIndex(), smooth1ModelBox_->value()));
}

void REIXSXESImageInterpolationABEditor::onCSmooth2ModeChanged() {
	analysisBlock_->setCorrelation2Smoothing(QPair<int,int>(correlation2SmoothingBox_->currentIndex(), smooth2Mode1Box_->value()));
}

void REIXSXESImageInterpolationABEditor::onCorrelation1PointsBoxChanged(int points)
{
	int halfWidth = points/2;

	if(halfWidth == analysisBlock_->correlation1HalfWidth())
		return;

	analysisBlock_->setCorrelation1HalfWidth(halfWidth);

	// update lines showing correlation range.
	corrRegion1Left_->setValue(QPointF(analysisBlock_->correlation1CenterPixel()-analysisBlock_->correlation1HalfWidth(), 0));
	corrRegion1Right_->setValue(QPointF(analysisBlock_->correlation1CenterPixel()+analysisBlock_->correlation1HalfWidth(), 0));
}

void REIXSXESImageInterpolationABEditor::onCorrelation2PointsBoxChanged(int points)
{
	int halfWidth = points/2;

	if(halfWidth == analysisBlock_->correlation2HalfWidth())
		return;

	analysisBlock_->setCorrelation2HalfWidth(halfWidth);

	// update lines showing correlation range.
	corrRegion2Left_->setValue(QPointF(analysisBlock_->correlation2CenterPixel()-analysisBlock_->correlation2HalfWidth(), 0));
	corrRegion2Right_->setValue(QPointF(analysisBlock_->correlation2CenterPixel()+analysisBlock_->correlation2HalfWidth(), 0));
}


void REIXSXESImageInterpolationABEditor::onAnalysisBlockInputDataSourcesChanged()
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

		curve2DisabledCheckBox_->setEnabled(true);

		correlation1CenterBox_->setEnabled(true);
		correlation1PointsBox_->setEnabled(true);
		correlation1SmoothingBox_->setEnabled(true);
		correlation2CenterBox_->setEnabled(true);
		correlation2PointsBox_->setEnabled(true);
		correlation2SmoothingBox_->setEnabled(true);
		liveCorrelationCheckBox_->setEnabled(true);
		correlateNowButton_->setEnabled(true);
		shiftDisplayOffsetSlider_->setEnabled(true);
		energyCalibrationOffsetBox_->setEnabled(true);
		tiltCalibrationOffsetBox_->setEnabled(true);

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

		bool disabled = analysisBlock_->curve2Disabled();
		curve2DisabledCheckBox_->blockSignals(true);
		correlation2CenterBox_->setDisabled(disabled);
		correlation2PointsBox_->setDisabled(disabled);
		correlation2SmoothingBox_->setDisabled(disabled);
		smooth2Mode1Box_->setDisabled(disabled);
		shift2LineEdit_->setDisabled(disabled);

		shift1Series_->setVisible(!disabled);
		shift2Series_->setVisible(!disabled);
		corrRegion2Left_->setVisible(!disabled);
		corrRegion2Right_->setVisible(!disabled);
		curve2DisabledCheckBox_->setChecked(analysisBlock_->curve2Disabled());
		curve2DisabledCheckBox_->blockSignals(false);

		correlation1CenterBox_->blockSignals(true);
		correlation1CenterBox_->setMaximum(inputSource->size(0) - 1);
		correlation1CenterBox_->setValue(analysisBlock_->correlation1CenterPixel());
		correlation1CenterBox_->blockSignals(false);

		correlation2CenterBox_->blockSignals(true);
		correlation2CenterBox_->setMaximum(inputSource->size(0) - 1);
		correlation2CenterBox_->setValue(analysisBlock_->correlation2CenterPixel());
		correlation2CenterBox_->blockSignals(false);

		correlation1PointsBox_->blockSignals(true);
		correlation1PointsBox_->setValue(analysisBlock_->correlation1HalfWidth()*2);
		correlation1PointsBox_->blockSignals(false);

		correlation2PointsBox_->blockSignals(true);
		correlation2PointsBox_->setValue(analysisBlock_->correlation2HalfWidth()*2);
		correlation2PointsBox_->blockSignals(false);

		correlation1SmoothingBox_->blockSignals(true);
		correlation1SmoothingBox_->setCurrentIndex(analysisBlock_->correlation1Smoothing().first);
		correlation1SmoothingBox_->blockSignals(false);

		correlation2SmoothingBox_->blockSignals(true);
		correlation2SmoothingBox_->setCurrentIndex(analysisBlock_->correlation2Smoothing().first);
		correlation2SmoothingBox_->blockSignals(false);

		binningLevelBox_->blockSignals(true);
		binningLevelBox_->setValue(analysisBlock_->binningLevel());
		binningLevelBox_->blockSignals(false);

		smooth1ModelBox_->blockSignals(true);
		smooth1ModelBox_->setValue(analysisBlock_->correlation1Smoothing().second);
		if(analysisBlock_->correlation1Smoothing().first == 1)
			smooth1ModelBox_->setMinimum(2);
		smooth1ModelBox_->blockSignals(false);

		smooth2Mode1Box_->blockSignals(true);
		smooth2Mode1Box_->setValue(analysisBlock_->correlation2Smoothing().second);
		if(analysisBlock_->correlation2Smoothing().first == 1)
			smooth2Mode1Box_->setMinimum(2);
		smooth2Mode1Box_->blockSignals(false);

		liveCorrelationCheckBox_->blockSignals(true);
		liveCorrelationCheckBox_->setChecked(analysisBlock_->liveCorrelation());
		liveCorrelationCheckBox_->blockSignals(false);

		shiftDisplayOffsetSlider_->blockSignals(true);
		shiftDisplayOffsetSlider_->setRange(0, inputSource->size(0)-1);
		shiftDisplayOffsetSlider_->setValue(inputSource->size(0)/2);
		shiftDisplayOffsetSlider_->blockSignals(false);

		image_ = new MPlotImageBasic();
		image_->setColorMap(*colorMap_);
		AMDataSourceImageData *model = new AMDataSourceImageData;
		model->setDataSource(inputSource);
		image_->setModel(model, true);
		plot_->addItem(image_);

		corrRegion1Left_->setValue(QPointF(analysisBlock_->correlation1CenterPixel()-analysisBlock_->correlation1HalfWidth(), 0));
		corrRegion1Right_->setValue(QPointF(analysisBlock_->correlation1CenterPixel()+analysisBlock_->correlation1HalfWidth(), 0));
		corrRegion2Left_->setValue(QPointF(analysisBlock_->correlation2CenterPixel()-analysisBlock_->correlation2HalfWidth(), 0));
		corrRegion2Right_->setValue(QPointF(analysisBlock_->correlation2CenterPixel()+analysisBlock_->correlation2HalfWidth(), 0));

		onShiftValuesChanged();

	}

	else {
		// no input source. Not much we can do.
		rangeMinYControl_->setEnabled(false);
		rangeMaxYControl_->setEnabled(false);
		rangeMinXControl_->setEnabled(false);
		rangeMaxXControl_->setEnabled(false);
		rangeRoundControl_->setEnabled(false);

		correlation1CenterBox_->setEnabled(false);
		correlation1PointsBox_->setEnabled(false);
		correlation1SmoothingBox_->setEnabled(false);
		liveCorrelationCheckBox_->setEnabled(false);
		correlateNowButton_->setEnabled(false);
		shiftDisplayOffsetSlider_->setEnabled(false);
		energyCalibrationOffsetBox_->setEnabled(false);
		tiltCalibrationOffsetBox_->setEnabled(false);
	}

	placeRangeRectangle();
}

void REIXSXESImageInterpolationABEditor::placeRangeRectangle()
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



REIXSXESImageInterpolationABEditorEllipticalMask::REIXSXESImageInterpolationABEditorEllipticalMask(REIXSXESImageInterpolationAB *analysisBlock, QObject *parent)
	: QObject(parent)
{
	analysisBlock_ = analysisBlock;

	/*
	connect(analysisBlock_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));
	*/
}


qreal REIXSXESImageInterpolationABEditorEllipticalMask::x(unsigned index) const
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

qreal REIXSXESImageInterpolationABEditorEllipticalMask::y(unsigned index) const
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
		if(index == 0 || index == (unsigned)dx || index == 2*(unsigned)dx) {
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

int REIXSXESImageInterpolationABEditorEllipticalMask::count() const
{
	if(analysisBlock_->rangeRound() == 0.0) {
		return 5; //rectangular masking, 5th point is to close rectangle (equals first point)
	}
	else {
		//rounded mask
		return (analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX() + 1)*2 - 1; //note that we include the first point twice, to close the ellipse
	}
}

void REIXSXESImageInterpolationABEditorEllipticalMask::rangeValuesChanged()
{
	emitDataChanged();
}

/*
void REIXSXESImageInterpolationABEditorEllipticalMask::onOutputSizeChanged()
{
	emitDataChanged();
}
*/

void REIXSXESImageInterpolationABEditorEllipticalMask::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
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
			if(index <= (unsigned(analysisBlock_->sumRangeMaxX() - analysisBlock_->sumRangeMinX()))) {
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

void REIXSXESImageInterpolationABEditorEllipticalMask::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned index=indexStart; index<=indexEnd; ++index) {
		*(outputValues++) = REIXSXESImageInterpolationABEditorEllipticalMask::y(index);

	}
}



























REIXSXESImageInterpolationABEditorShiftModel::REIXSXESImageInterpolationABEditorShiftModel(REIXSXESImageInterpolationAB *analysisBlock, QObject *parent)
	: QObject(parent)
{
	analysisBlock_ = analysisBlock;
	displayXOffset_ = analysisBlock_->inputDataSourceAt(0)->size(0)/2;

	connect(analysisBlock_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));
}


qreal REIXSXESImageInterpolationABEditorShiftModel::x(unsigned index) const
{
	if (analysisBlock_->curve2Disabled())
		return analysisBlock_->shiftValues1().at(index) + displayXOffset_;
	else
		return analysisBlock_->shiftValuesAt(displayXOffset_).at(index) + displayXOffset_;
}

qreal REIXSXESImageInterpolationABEditorShiftModel::y(unsigned index) const
{
	return index;
}

int REIXSXESImageInterpolationABEditorShiftModel::count() const
{
	return analysisBlock_->shiftValuesAt(displayXOffset_).count();
}

void REIXSXESImageInterpolationABEditorShiftModel::setDisplayXOffset(int offset)
{
	displayXOffset_ = offset;
	emitDataChanged();
}

void REIXSXESImageInterpolationABEditorShiftModel::onOutputSizeChanged()
{
	if (displayXOffset_ > analysisBlock_->inputDataSourceAt(0)->size(0))
			setDisplayXOffset(analysisBlock_->inputDataSourceAt(0)->size(0)/2);
	emitDataChanged();
}

void REIXSXESImageInterpolationABEditorShiftModel::onShiftValuesChanged()
{
	emitDataChanged();
}

void REIXSXESImageInterpolationABEditorShiftModel::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	if (analysisBlock_->curve2Disabled())
		for(unsigned i=indexStart; i<=indexEnd; ++i)
			*(outputValues++) = analysisBlock_->shiftValues1().at(i) + displayXOffset_;
	else
		for(unsigned i=indexStart; i<=indexEnd; ++i)
			*(outputValues++) = analysisBlock_->shiftValuesAt(displayXOffset_).at(i) + displayXOffset_;
}

void REIXSXESImageInterpolationABEditorShiftModel::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned i=indexStart; i<=indexEnd; ++i)
		*(outputValues++) = i;
}






REIXSXESImageInterpolationABEditorShift1Model::REIXSXESImageInterpolationABEditorShift1Model(REIXSXESImageInterpolationAB *analysisBlock, QObject *parent)
	: QObject(parent)
{
	analysisBlock_ = analysisBlock;

	connect(analysisBlock_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));
}


qreal REIXSXESImageInterpolationABEditorShift1Model::x(unsigned index) const
{
	return analysisBlock_->shiftValues1().at(index) + analysisBlock_->correlation1CenterPixel();
}

qreal REIXSXESImageInterpolationABEditorShift1Model::y(unsigned index) const
{
	return index;
}

int REIXSXESImageInterpolationABEditorShift1Model::count() const
{
	return analysisBlock_->shiftValues1().count();
}

void REIXSXESImageInterpolationABEditorShift1Model::onOutputSizeChanged()
{
	emitDataChanged();
}

void REIXSXESImageInterpolationABEditorShift1Model::onShiftValuesChanged()
{
	emitDataChanged();
}

void REIXSXESImageInterpolationABEditorShift1Model::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned i=indexStart; i<=indexEnd; ++i)
		*(outputValues++) = analysisBlock_->shiftValues1().at(i) + analysisBlock_->correlation1CenterPixel();
}

void REIXSXESImageInterpolationABEditorShift1Model::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned i=indexStart; i<=indexEnd; ++i)
		*(outputValues++) = i;
}


REIXSXESImageInterpolationABEditorShift2Model::REIXSXESImageInterpolationABEditorShift2Model(REIXSXESImageInterpolationAB *analysisBlock, QObject *parent)
	: QObject(parent)
{
	analysisBlock_ = analysisBlock;

	connect(analysisBlock_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onOutputSizeChanged()));
	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));
}


qreal REIXSXESImageInterpolationABEditorShift2Model::x(unsigned index) const
{
	return analysisBlock_->shiftValues2().at(index) + analysisBlock_->correlation2CenterPixel();
}

qreal REIXSXESImageInterpolationABEditorShift2Model::y(unsigned index) const
{
	return index;
}

int REIXSXESImageInterpolationABEditorShift2Model::count() const
{
	return analysisBlock_->shiftValues2().count();
}

void REIXSXESImageInterpolationABEditorShift2Model::onOutputSizeChanged()
{
	emitDataChanged();
}

void REIXSXESImageInterpolationABEditorShift2Model::onShiftValuesChanged()
{
	emitDataChanged();
}

void REIXSXESImageInterpolationABEditorShift2Model::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned i=indexStart; i<=indexEnd; ++i)
		*(outputValues++) = analysisBlock_->shiftValues2().at(i) + analysisBlock_->correlation2CenterPixel();
}

void REIXSXESImageInterpolationABEditorShift2Model::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	for(unsigned i=indexStart; i<=indexEnd; ++i)
		*(outputValues++) = i;
}




























void REIXSXESImageInterpolationABEditor::onApplyToOtherScansMenuClicked()
{
	int confirm = QMessageBox::Yes;

	QMessageBox confirmBox;
	confirmBox.setText("This will apply changes directly to the database, open scan will not be affected until they are closed (without saving) and re-opened. It is recommended that you close all scans that you wish to apply setting to before proceeding.");
	confirmBox.setInformativeText("Do you wish to proceed now?");
	confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Abort);
	confirmBox.setDefaultButton(QMessageBox::Yes);
	confirm = confirmBox.exec();

	if (confirm == QMessageBox::Yes)
	{
		if(!chooseScanDialog_) {
			chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Choose scans...", "Choose the scans you want to apply these analysis parameters to.", this);
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
}

#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include <QProgressDialog>
#include <QUrl>

#include <QDebug>
void REIXSXESImageInterpolationABEditor::onApplyToOtherScansChosen()
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
			REIXSXESImageInterpolationAB* xesAB = qobject_cast<REIXSXESImageInterpolationAB*>(scan->analyzedDataSources()->at(i));
			if(xesAB) {
				xesABFound = true;
				xesAB->enableLiveCorrelation(analysisBlock_->liveCorrelation());
				if(batchApplySumRange_->isChecked()) {
					xesAB->setSumRangeMaxY(analysisBlock_->sumRangeMaxY());
					xesAB->setSumRangeMinY(analysisBlock_->sumRangeMinY());
					xesAB->setSumRangeMaxX(analysisBlock_->sumRangeMaxX());
					xesAB->setSumRangeMinX(analysisBlock_->sumRangeMinX());
					xesAB->setRangeRound(analysisBlock_->rangeRound());
				}
				if(batchApplyCalibrationOffsets_->isChecked()) {
					xesAB->setEnergyCalibrationOffset(analysisBlock_->energyCalibrationOffset());
					xesAB->setTiltCalibrationOffset(analysisBlock_->tiltCalibrationOffset());
					xesAB->setBinningLevel(analysisBlock_->binningLevel());
				}
				if(batchApplyCorrelationSettings_->isChecked()) {
					xesAB->setCorrelation1CenterPixel(analysisBlock_->correlation1CenterPixel());
					xesAB->setCorrelation1HalfWidth(analysisBlock_->correlation1HalfWidth());
					xesAB->setCorrelation1Smoothing(analysisBlock_->correlation1Smoothing());
					xesAB->setCorrelation2CenterPixel(analysisBlock_->correlation2CenterPixel());
					xesAB->setCorrelation2HalfWidth(analysisBlock_->correlation2HalfWidth());
					xesAB->setCorrelation2Smoothing(analysisBlock_->correlation2Smoothing());
				}
				if(batchApplyShiftCurve_->isChecked()) {
					xesAB->setShiftValues1(analysisBlock_->shiftValues1());
					xesAB->setShiftValues2(analysisBlock_->shiftValues2());
					xesAB->setCurve2Disabled(analysisBlock_->curve2Disabled());

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


void REIXSXESImageInterpolationABEditor::onShift1LineEdited()
{
	QStringList newShifts = shift1LineEdit_->text().split(QRegExp("[,; ]+"), QString::SkipEmptyParts);
	if(newShifts.count() != analysisBlock_->shiftValues1().count()) {
		QApplication::beep();
		AMErrorMon::alert(this, -370, QString("Could not set the shift values manually: you need to provide %1 shift numbers.").arg(analysisBlock_->shiftValues1().count()));
		return;
	}

	bool ok;
	AMIntList newShiftNumbers;
	foreach(QString s, newShifts) {
		newShiftNumbers << s.toInt(&ok);
		if(!ok) {
			AMErrorMon::alert(this, -370, QString("Could not set the shift values manually: '%1' is not a number.").arg(s));
			return;
		}
	}

	analysisBlock_->setShiftValues1(newShiftNumbers);
}


void REIXSXESImageInterpolationABEditor::onShift2LineEdited()
{
	QStringList newShifts = shift2LineEdit_->text().split(QRegExp("[,; ]+"), QString::SkipEmptyParts);
	if(newShifts.count() != analysisBlock_->shiftValues2().count()) {
		QApplication::beep();
		AMErrorMon::alert(this, -370, QString("Could not set the shift values manually: you need to provide %1 shift numbers.").arg(analysisBlock_->shiftValues2().count()));
		return;
	}

	bool ok;
	AMIntList newShiftNumbers;
	foreach(QString s, newShifts) {
		newShiftNumbers << s.toInt(&ok);
		if(!ok) {
			AMErrorMon::alert(this, -370, QString("Could not set the shift values manually: '%1' is not a number.").arg(s));
			return;
		}
	}

	analysisBlock_->setShiftValues2(newShiftNumbers);
}

void REIXSXESImageInterpolationABEditor::onShiftValuesChanged()
{
	// Grab the current shift and convert to text.
	QStringList shifts1StringList;
	foreach(int d, analysisBlock_->shiftValues1())
		shifts1StringList << QString::number(d);
	shift1LineEdit_->setText(shifts1StringList.join(";"));


	QStringList shifts2StringList;
	foreach(int d, analysisBlock_->shiftValues2())
		shifts2StringList << QString::number(d);
	shift2LineEdit_->setText(shifts2StringList.join(";"));

}

void REIXSXESImageInterpolationABEditor::onBinningLevelBoxChanged(int value)
{
		analysisBlock_->setBinningLevel(value);
}
