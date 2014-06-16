/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

	correlationSmoothingBox_ = new QComboBox();
	correlationSmoothingBox_->addItem("None");
	correlationSmoothingBox_->addItem("Quadratic");
	correlationSmoothingBox_->addItem("Cubic");
	correlationSmoothingBox_->addItem("Quartic");

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
	plot_->axisBottom()->setTicks(3);
	plot_->axisLeft()->setTicks(3);

	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());


	// Build Layout:

	QVBoxLayout* vl = new QVBoxLayout();
	QFormLayout* fl = new QFormLayout();
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(rangeMinControl_);
	hl->addWidget(new QLabel("To"));
	hl->addWidget(rangeMaxControl_);
	fl->addRow("From:", hl);
	QHBoxLayout* hl4 = new QHBoxLayout();
	hl4->addWidget(energyCalibrationOffsetBox_);
	hl4->addWidget(tiltCalibrationOffsetBox_);
	fl->addRow("Offset:", hl4);
	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(correlationCenterBox_);
	hl2->addWidget(new QLabel("#"));
	hl2->addWidget(correlationPointsBox_);
	fl->addRow("C. center:", hl2);
	fl->addRow("C. smooth:", correlationSmoothingBox_);
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
//	onShiftValuesChanged();

	// make connections:
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onAnalysisBlockInputDataSourcesChanged()));
//	connect(analysisBlock_, SIGNAL(shiftValuesChanged()), this, SLOT(onShiftValuesChanged()));

	connect(rangeMinControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMinControlChanged(int)));
	connect(rangeMaxControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMaxControlChanged(int)));
	connect(correlationCenterBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelationCenterBoxChanged(int)));
	connect(correlationPointsBox_, SIGNAL(valueChanged(int)), this, SLOT(onCorrelationPointsBoxChanged(int)));

	connect(shiftDisplayOffsetSlider_, SIGNAL(valueChanged(int)), shiftData_, SLOT(setDisplayXOffset(int)));

	connect(manualShiftEntryButton_, SIGNAL(clicked()), this, SLOT(onManualShiftEntryButtonClicked()));

	// direct connections:
	connect(correlateNowButton_, SIGNAL(clicked()), analysisBlock_, SLOT(correlateNow()));
	connect(liveCorrelationCheckBox_, SIGNAL(toggled(bool)), analysisBlock_, SLOT(enableLiveCorrelation(bool)));
	connect(energyCalibrationOffsetBox_, SIGNAL(valueChanged(double)), analysisBlock_, SLOT(setEnergyCalibrationOffset(double)));
	connect(tiltCalibrationOffsetBox_, SIGNAL(valueChanged(double)), analysisBlock_, SLOT(setTiltCalibrationOffset(double)));
	connect(correlationSmoothingBox_, SIGNAL(currentIndexChanged(int)), analysisBlock_, SLOT(setCorrelationSmoothing(int)));

	connect(applyToOtherScansButton_, SIGNAL(clicked()), this, SLOT(onApplyToOtherScansMenuClicked()));
}

REIXSXESImageABEditor::~REIXSXESImageABEditor() {
	delete colorMap_;
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
	corrRegionLeft_->setValue(QPointF(analysisBlock_->correlationCenterPixel()-analysisBlock_->correlationHalfWidth(), 0));
	corrRegionRight_->setValue(QPointF(analysisBlock_->correlationCenterPixel()+analysisBlock_->correlationHalfWidth(), 0));
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

		rangeMinControl_->setEnabled(true);
		rangeMaxControl_->setEnabled(true);
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

		correlationSmoothingBox_->blockSignals(true);
		correlationSmoothingBox_->setCurrentIndex(analysisBlock_->correlationSmoothing());
		correlationSmoothingBox_->blockSignals(false);

		liveCorrelationCheckBox_->blockSignals(true);
		liveCorrelationCheckBox_->setChecked(analysisBlock_->liveCorrelation());
		liveCorrelationCheckBox_->blockSignals(false);

		shiftDisplayOffsetSlider_->blockSignals(true);
		shiftDisplayOffsetSlider_->setRange(0, analysisBlock_->size(0)-1);
		shiftDisplayOffsetSlider_->setValue(analysisBlock_->size(0)/2);
		shiftDisplayOffsetSlider_->blockSignals(false);

		image_ = new MPlotImageBasic();
		image_->setColorMap(*colorMap_);
		image_->setModel(new AMDataSourceImageData(inputSource), true);
		plot_->addItem(image_);

		corrRegionLeft_->setValue(QPointF(analysisBlock_->correlationCenterPixel()-analysisBlock_->correlationHalfWidth(), 0));
		corrRegionRight_->setValue(QPointF(analysisBlock_->correlationCenterPixel()+analysisBlock_->correlationHalfWidth(), 0));
	}

	else {
		// no input source. Not much we can do.
		rangeMinControl_->setEnabled(false);
		rangeMaxControl_->setEnabled(false);
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
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Choose scans...", "Choose the scans you want to apply these analysis parameters to.", true, this);
		// chooseScanDialog_->dataView_->setOrganizeMode(AMDataViews::OrganizeScanTypes);
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

#include <QDebug>
void REIXSXESImageABEditor::onApplyToOtherScansChosen()
{
	disconnect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onApplyToOtherScansChosen()));

	qDebug() << "Calling applyToOtherScansChosen.";
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
					xesAB->setSumRangeMax(analysisBlock_->sumRangeMax());
					xesAB->setSumRangeMin(analysisBlock_->sumRangeMin());
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

		scan->release();	// delete scan.
	}

	progressDialog->setValue(scans.count());
	delete progressDialog;

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

