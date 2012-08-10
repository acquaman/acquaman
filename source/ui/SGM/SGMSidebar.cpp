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


#include "SGMSidebar.h"

#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>

#include "ui/beamline/AMControlButton.h"

SGMSidebar::SGMSidebar(QWidget *parent) :
	QWidget(parent)
{
	mainBox_ = new QGroupBox("SGM Beamline");
	mainLayout_ = new QVBoxLayout();
	mainLayout_->addWidget(mainBox_);
	gl_ = new QGridLayout();
	mainBox_->setLayout(gl_);

	setMaximumWidth(350);

	readyLabel_ = new AMExtendedControlEditor(SGMBeamline::sgm()->beamlineReady(), NULL, true);
	readyLabel_->setNoUnitsBox(true);
	readyLabel_->overrideTitle("");
	readyLabel_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	beamOnAction_ = 0;//NULL
	beamOnButton_ = new QToolButton();
	beamOnButton_->setText("Beam On");
	beamOnButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnButtonClicked()));
	beamOffCButton_ = new AMControlButton(SGMBeamline::sgm()->fastShutterVoltage());
	beamOffCButton_->overrideText("Beam Off");
	beamOffCButton_->setDownValue(5);
	beamOffCButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	stopMotorsAction_ = 0;//NULL
	stopMotorsButton_ = new QToolButton();
	stopMotorsButton_->setText("Emergency\nMotor Stop");
	stopMotorsButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(stopMotorsButton_, SIGNAL(clicked()), this, SLOT(onStopMotorsButtonClicked()));
	closeVacuumButton_ = new QToolButton();
	closeVacuumButton_->setText("Close Vacuum");
	closeVacuumButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(closeVacuumButton_, SIGNAL(clicked()), this, SLOT(onCloseVacuumButtonClicked()));
	visibleLightButton_ = new QToolButton();
	visibleLightButton_->setText("Visible Light");
	visibleLightButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(visibleLightButton_, SIGNAL(clicked()), this, SLOT(onVisibleLightClicked()));
	connect(SGMBeamline::sgm(), SIGNAL(visibleLightStatusChanged(QString)), this, SLOT(onVisibleLightStatusChanged(QString)));
	energyNC_ = new AMExtendedControlEditor(SGMBeamline::sgm()->energy(), SGMBeamline::sgm()->energyMovingStatus());
	energyNC_->setControlFormat('f', 2);
	energyNC_->overrideTitle("Energy");
	energyNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	trackUndulatorCButton_ = new AMControlButton(SGMBeamline::sgm()->undulatorTracking());
	trackUndulatorCButton_->overrideText("Undulator");
	trackUndulatorCButton_->setCheckable(true);
	trackUndulatorCButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	trackGratingCButton_ = new AMControlButton(SGMBeamline::sgm()->monoTracking());
	trackGratingCButton_->overrideText("Grating");
	trackGratingCButton_->setCheckable(true);
	trackGratingCButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	trackExitSlitCButton_ = new AMControlButton(SGMBeamline::sgm()->exitSlitTracking());
	trackExitSlitCButton_->overrideText("Exit Slit");
	trackExitSlitCButton_->setCheckable(true);
	trackExitSlitCButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	gratingNC_ = new AMExtendedControlEditor(SGMBeamline::sgm()->grating());
	gratingNC_->overrideTitle("Grating");
	gratingNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	exitSlitNC_ = new AMExtendedControlEditor(SGMBeamline::sgm()->exitSlitGap());
	exitSlitNC_->setControlFormat('f', 1);
	exitSlitNC_->overrideTitle("Exit Slit");
	exitSlitNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	scanningLabel_ = new AMExtendedControlEditor(SGMBeamline::sgm()->beamlineScanning(), NULL, true);
	scanningLabel_->setNoUnitsBox(true);
	scanningLabel_->overrideTitle("");
	scanningLabel_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	scanningResetButton_ = new QToolButton();
	scanningResetButton_->setText("Reset");
	scanningResetButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(scanningResetButton_, SIGNAL(clicked()), this, SLOT(onScanningResetButtonClicked()));
	QHBoxLayout *shl = new QHBoxLayout();
	shl->addWidget(scanningLabel_);
	shl->addWidget(scanningResetButton_);
	shl->setSpacing(0);
	shl->setContentsMargins(2, 2, 2, 2);

	beamlineWarningsLabel_ = new QLabel(SGMBeamline::sgm()->beamlineWarnings());
	connect(SGMBeamline::sgm(), SIGNAL(beamlineWarningsChanged(QString)), beamlineWarningsLabel_, SLOT(setText(QString)));
	connect(SGMBeamline::sgm(), SIGNAL(beamlineWarningsChanged(QString)), this, SLOT(onBeamlineWarnings(QString)));

	i0CheckBox_ = new QCheckBox("I0");
	teyCheckBox_ = new QCheckBox("TEY");
	tfyCheckBox_ = new QCheckBox("TFY");
	pdCheckBox_ = new QCheckBox("PD");
	fpd1CheckBox_ = new QCheckBox("FPD1");
	fpd2CheckBox_ = new QCheckBox("FPD2");
	fpd3CheckBox_ = new QCheckBox("FPD3");
	fpd4CheckBox_ = new QCheckBox("FPD4");

	// create UI elements
	imageView_ = new MPlotWidget();
	imageView_->setMinimumHeight(200);
	imagePlot_ = new MPlot();

	QHBoxLayout *checkBoxHL1 = new QHBoxLayout();
	checkBoxHL1->addWidget(i0CheckBox_);
	checkBoxHL1->addWidget(teyCheckBox_);
	checkBoxHL1->addWidget(tfyCheckBox_);
	checkBoxHL1->addWidget(pdCheckBox_);
	checkBoxHL1->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout *checkBoxHL2 = new QHBoxLayout();
	checkBoxHL2->addWidget(fpd1CheckBox_);
	checkBoxHL2->addWidget(fpd2CheckBox_);
	checkBoxHL2->addWidget(fpd3CheckBox_);
	checkBoxHL2->addWidget(fpd4CheckBox_);
	checkBoxHL2->setContentsMargins(0, 0, 0, 0);

	QVBoxLayout *checkBoxVL = new QVBoxLayout();
	checkBoxVL->addLayout(checkBoxHL1);
	checkBoxVL->addLayout(checkBoxHL2);
	checkBoxVL->setContentsMargins(1, 1, 1, 1);

	plotLayout_ = new QVBoxLayout();
	plotLayout_->addWidget(imageView_);
	plotLayout_->addLayout(checkBoxVL);
	plotLayout_->setContentsMargins(1, 1, 1, 1);

	// ATTENTION DAVE: New special axis scale on bottom of strip tool plot:
	stripToolSpecialAxisScale_ = new MPlotAxisScale(Qt::Horizontal);
	stripToolSpecialAxisScale_->setPadding(2);
	imagePlot_->addAxisScale(stripToolSpecialAxisScale_);
	imagePlot_->axisBottom()->setAxisScale(stripToolSpecialAxisScale_);	// have the bottom (visible) axis display this axisScale instead
	/////////////////

	imageView_->setPlot(imagePlot_);
	imagePlot_->axisScaleLeft()->setAutoScaleEnabled();
	imagePlot_->axisScaleLeft()->setPadding(2);
	imagePlot_->axisScaleBottom()->setAutoScaleEnabled();
	imagePlot_->axisScaleBottom()->setPadding(2);
	imagePlot_->setMarginBottom(10);
	imagePlot_->setMarginLeft(10);
	imagePlot_->setMarginRight(5);
	imagePlot_->setMarginTop(5);
	imagePlot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	imagePlot_->axisRight()->setTicks(4);
	imagePlot_->axisBottom()->setTicks(4);
	imagePlot_->axisLeft()->showGrid(false);
	imagePlot_->axisBottom()->showAxisName(false);
	imagePlot_->axisLeft()->showAxisName(false);

	i0Model_ = new MPlotRealtimeModel(this);
	teyModel_ = new MPlotRealtimeModel(this);
	tfyModel_ = new MPlotRealtimeModel(this);
	pdModel_ = new MPlotRealtimeModel(this);
	fpd1Model_ = new MPlotRealtimeModel(this);
	fpd2Model_ = new MPlotRealtimeModel(this);
	fpd3Model_ = new MPlotRealtimeModel(this);
	fpd4Model_ = new MPlotRealtimeModel(this);

	i0Series_ = new MPlotSeriesBasic(i0Model_);
	i0Series_->setDescription("I0");
	i0Series_->setLinePen(QPen(QColor(255, 0, 0)));
	i0Series_->setMarker(MPlotMarkerShape::None);
	teySeries_ = new MPlotSeriesBasic(teyModel_);
	teySeries_->setDescription("TEY");
	teySeries_->setLinePen(QPen(QColor(0, 255, 0)));
	teySeries_->setMarker(MPlotMarkerShape::None);
	tfySeries_ = new MPlotSeriesBasic(tfyModel_);
	tfySeries_->setDescription("TFY");
	tfySeries_->setLinePen(QPen(QColor(0, 0, 255)));
	tfySeries_->setMarker(MPlotMarkerShape::None);
	pdSeries_ = new MPlotSeriesBasic(pdModel_);
	pdSeries_->setDescription("PD");
	pdSeries_->setLinePen(QPen(QColor(255, 0, 255)));
	pdSeries_->setMarker(MPlotMarkerShape::None);

	fpd1Series_ = new MPlotSeriesBasic(fpd1Model_);
	fpd1Series_->setDescription("FPD1");
	fpd1Series_->setLinePen(QPen(QColor(255, 255, 0)));
	fpd1Series_->setMarker(MPlotMarkerShape::None);
	fpd2Series_ = new MPlotSeriesBasic(fpd2Model_);
	fpd2Series_->setDescription("FPD2");
	fpd2Series_->setLinePen(QPen(QColor(0, 255, 255)));
	fpd2Series_->setMarker(MPlotMarkerShape::None);
	fpd3Series_ = new MPlotSeriesBasic(fpd3Model_);
	fpd3Series_->setDescription("FPD3");
	fpd3Series_->setLinePen(QPen(QColor(255, 127, 127)));
	fpd3Series_->setMarker(MPlotMarkerShape::None);
	fpd4Series_ = new MPlotSeriesBasic(fpd4Model_);
	fpd4Series_->setDescription("FPD4");
	fpd4Series_->setLinePen(QPen(QColor(127, 255, 127)));
	fpd4Series_->setMarker(MPlotMarkerShape::None);

	imagePlot_->addItem(i0Series_);
	i0CheckBox_->setChecked(true);

	connect(i0CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onI0CheckBoxToggled(bool)));
	connect(teyCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onTEYCheckBoxToggled(bool)));
	connect(tfyCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onTFYCheckBoxToggled(bool)));
	connect(pdCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onPDCheckBoxToggled(bool)));
	connect(fpd1CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onFPD1CheckBoxToggled(bool)));
	connect(fpd2CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onFPD2CheckBoxToggled(bool)));
	connect(fpd3CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onFPD3CheckBoxToggled(bool)));
	connect(fpd4CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onFPD4CheckBoxToggled(bool)));

	stripToolCounter_ = 0;
	stripToolTimer_ = new QTimer(this);
	connect(stripToolTimer_, SIGNAL(timeout()), this, SLOT(onStripToolTimerTimeout()));
	stripToolTimer_->start(1000);

	warningAndPlotHL_ = new QHBoxLayout();
	warningAndPlotHL_->addWidget(beamlineWarningsLabel_);

	gl_->addWidget(readyLabel_,		0, 0, 1, 6, 0);
	gl_->addWidget(beamOnButton_,		1, 0, 1, 2, 0);
	gl_->addWidget(beamOffCButton_,		1, 2, 1, 2, 0);
	gl_->addWidget(stopMotorsButton_,	1, 4, 1, 2, 0);
	gl_->addWidget(closeVacuumButton_,	2, 0, 1, 3, 0);
	gl_->addWidget(visibleLightButton_,	2, 3, 1, 3, 0);
	gl_->addWidget(energyNC_,		3, 0, 1, 6, 0);
	gl_->addWidget(trackUndulatorCButton_,	4, 0, 1, 2, 0);
	gl_->addWidget(trackGratingCButton_,	4, 2, 1, 2, 0);
	gl_->addWidget(trackExitSlitCButton_,	4, 4, 1, 2, 0);
	gl_->addWidget(gratingNC_,		5, 0, 1, 6, 0);
	gl_->addWidget(exitSlitNC_,		6, 0, 1, 3, 0);
	gl_->addLayout(shl,			6, 3, 1, 3, 0);
	gl_->addLayout(warningAndPlotHL_,	10, 0, 1, 6, 0);

	gl_->setRowStretch(9, 10);

	setLayout(mainLayout_);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

	scanningResetButton_->setContentsMargins(2,2,2,2);
	onBeamlineWarnings("");
}

SGMSidebar::~SGMSidebar() {
	// must delete series objects first.  If left to ~QObject / QObject::deleteChildren() called from ~MPlot, the ~MPlotAbstractSeries() crashes when trying to disconnect from its data, because we've deleted the data already.
	delete i0Series_;
	delete teySeries_;
	delete tfySeries_;
	delete pdSeries_;
}


void SGMSidebar::showEvent(QShowEvent *se){
	int minWidth = std::max(exitSlitNC_->size().width()/3, trackUndulatorCButton_->size().width()/2);
	minWidth = std::max(minWidth, trackGratingCButton_->size().width()/2);
	minWidth = std::max(minWidth, trackExitSlitCButton_->size().width()/2);
	gl_->setColumnMinimumWidth(0, minWidth);
	gl_->setColumnMinimumWidth(1, minWidth);
	gl_->setColumnMinimumWidth(2, minWidth);
	gl_->setColumnMinimumWidth(3, minWidth);
	gl_->setColumnMinimumWidth(4, minWidth);
	gl_->setColumnMinimumWidth(5, minWidth);
	QWidget::showEvent(se);
}

void SGMSidebar::onVisibleLightStatusChanged(const QString &status){
	if(visibleLightButton_)
		visibleLightButton_->setText(status);
}

void SGMSidebar::onVisibleLightClicked(){
	if(SGMBeamline::sgm()->isVisibleLightOn())
		SGMBeamline::sgm()->visibleLightOff();
	else
		SGMBeamline::sgm()->visibleLightOn();
}

void SGMSidebar::onCloseVacuumButtonClicked(){
	SGMBeamline::sgm()->closeVacuum();
}

void SGMSidebar::onBeamOnButtonClicked(){
	if(beamOnAction_)
		return;
	beamOnAction_ = SGMBeamline::sgm()->createBeamOnActions();
	connect(beamOnAction_, SIGNAL(finished()), this, SLOT(onBeamOnActionFinished()));
	beamOnAction_->start();
}

void SGMSidebar::onBeamOnActionFinished(){
	/* NTBA - August 25th, 2011 (David Chevrier)
			Probably need to delete the internals too, list, actions, etc"
	*/
	delete beamOnAction_;
	beamOnAction_ = 0;//NULL
}

void SGMSidebar::onStopMotorsButtonClicked(){
	if(stopMotorsAction_)
		return;
	stopMotorsAction_ = SGMBeamline::sgm()->createStopMotorsAction();
	connect(stopMotorsAction_, SIGNAL(finished()), this, SLOT(onStopMotorsActionFinished()));
	stopMotorsAction_->start();
}

void SGMSidebar::onStopMotorsActionFinished(){
	delete stopMotorsAction_;
	stopMotorsAction_ = 0;//NULL
}

void SGMSidebar::onScanningResetButtonClicked(){
	SGMBeamline::sgm()->beamlineScanning()->move(0);
}

void SGMSidebar::onStripToolTimerTimeout(){
	if(i0Model_->count() <= 50) {
		stripToolSpecialAxisScale_->setDataRange(MPlotAxisRange(-i0Model_->count(),0));	// would need to correct this if not doing exactly one model point per second
	}
	else {
		i0Model_->removePointFront();
		teyModel_->removePointFront();
		tfyModel_->removePointFront();
		pdModel_->removePointFront();
	}

	// inserted to prevent crashes before SGM detectors connected
	double i0Reading = 0, teyReading = 0, tfyReading = 0, pdReading = 0;
	if(SGMBeamline::sgm()->i0Detector())
		i0Reading = SGMBeamline::sgm()->i0Detector()->reading();
	if(SGMBeamline::sgm()->teyDetector())
		teyReading = SGMBeamline::sgm()->teyDetector()->reading();
	if(SGMBeamline::sgm()->tfyDetector())
		tfyReading = SGMBeamline::sgm()->tfyDetector()->reading();
	if(SGMBeamline::sgm()->photodiodeDetector())
		pdReading = SGMBeamline::sgm()->photodiodeDetector()->reading();
	///////////////

	i0Series_->setDescription(QString("I0 %1").arg(i0Reading, 0, 'e', 2));
	i0Model_->insertPointBack(stripToolCounter_, i0Reading);
	teySeries_->setDescription(QString("TEY %1").arg(teyReading, 0, 'e', 2));
	teyModel_->insertPointBack(stripToolCounter_, teyReading);
	tfySeries_->setDescription(QString("TFY %1").arg(tfyReading, 0, 'e', 2));
	tfyModel_->insertPointBack(stripToolCounter_, tfyReading);
	pdSeries_->setDescription(QString("PD %1").arg(pdReading, 0, 'e', 2));
	pdModel_->insertPointBack(stripToolCounter_, pdReading);
	stripToolCounter_++;
}

void SGMSidebar::onI0CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(i0Series_);
	else
		imagePlot_->removeItem(i0Series_);
}

void SGMSidebar::onTEYCheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(teySeries_);
	else
		imagePlot_->removeItem(teySeries_);
}

void SGMSidebar::onTFYCheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(tfySeries_);
	else
		imagePlot_->removeItem(tfySeries_);
}

void SGMSidebar::onPDCheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(pdSeries_);
	else
		imagePlot_->removeItem(pdSeries_);
}

void SGMSidebar::onFPD1CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(fpd1Series_);
	else
		imagePlot_->removeItem(fpd1Series_);
}

void SGMSidebar::onFPD2CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(fpd2Series_);
	else
		imagePlot_->removeItem(fpd2Series_);
}

void SGMSidebar::onFPD3CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(fpd3Series_);
	else
		imagePlot_->removeItem(fpd3Series_);
}

void SGMSidebar::onFPD4CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(fpd4Series_);
	else
		imagePlot_->removeItem(fpd4Series_);
}

void SGMSidebar::onBeamlineWarnings(const QString &newWarnings){
	if(newWarnings.isEmpty() && warningAndPlotHL_->itemAt(0)->widget() == beamlineWarningsLabel_){
		warningAndPlotHL_->removeWidget(beamlineWarningsLabel_);
		beamlineWarningsLabel_->hide();
		warningAndPlotHL_->addLayout(plotLayout_);
		imageView_->show();
		i0CheckBox_->show();
		teyCheckBox_->show();
		tfyCheckBox_->show();
		pdCheckBox_->show();
		fpd1CheckBox_->show();
		fpd2CheckBox_->show();
		fpd3CheckBox_->show();
		fpd4CheckBox_->show();
	}
	else if(!newWarnings.isEmpty() && warningAndPlotHL_->itemAt(0)->layout() == plotLayout_){
		warningAndPlotHL_->removeItem(plotLayout_);
		imageView_->hide();
		i0CheckBox_->hide();
		teyCheckBox_->hide();
		tfyCheckBox_->hide();
		pdCheckBox_->hide();
		fpd1CheckBox_->hide();
		fpd2CheckBox_->hide();
		fpd3CheckBox_->hide();
		fpd4CheckBox_->hide();
		warningAndPlotHL_->addWidget(beamlineWarningsLabel_);
		beamlineWarningsLabel_->show();
	}
}
