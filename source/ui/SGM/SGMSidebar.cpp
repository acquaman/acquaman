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

#include "ui/CLS/CLSSynchronizedDwellTimeView.h"

SGMSidebar::SGMSidebar(QWidget *parent) :
	QWidget(parent)
{
	mainBox_ = new QGroupBox("SGM Beamline");
	mainLayout_ = new QVBoxLayout();
	mainLayout_->addWidget(mainBox_);
	gl_ = new QGridLayout();
	mainBox_->setLayout(gl_);

	setMaximumWidth(350);

	readyLabel_ = new AMControlEditor(SGMBeamline::sgm()->beamlineReady(), NULL, true);
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
	energyNC_ = new AMControlEditor(SGMBeamline::sgm()->energy(), SGMBeamline::sgm()->energyMovingStatus());
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
	gratingNC_ = new AMControlEditor(SGMBeamline::sgm()->grating());
	gratingNC_->overrideTitle("Grating");
	gratingNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	entranceSlitNC_ = new AMControlEditor(SGMBeamline::sgm()->entranceSlitGap());
	entranceSlitNC_->setControlFormat('f', 1);
	entranceSlitNC_->overrideTitle("Entrance Slit");
	entranceSlitNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	exitSlitNC_ = new AMControlEditor(SGMBeamline::sgm()->exitSlitGap());
	exitSlitNC_->setControlFormat('f', 1);
	exitSlitNC_->overrideTitle("Exit Slit");
	exitSlitNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	scanningLabel_ = new AMControlEditor(SGMBeamline::sgm()->beamlineScanning(), NULL, true);
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

	detectorSignalSources_ = new QButtonGroup();
	picoammeterButton_ = new QRadioButton(SGMBeamline::sgm()->sgmDetectorSignalSourceName(SGMBeamline::sourcePicoammeters));
	scalerButton_ = new QRadioButton(SGMBeamline::sgm()->sgmDetectorSignalSourceName(SGMBeamline::sourceScaler));
	detectorSignalSources_->addButton(picoammeterButton_, 0);
	detectorSignalSources_->addButton(scalerButton_, 1);
	QGroupBox *detectorSourceBox = new QGroupBox("Detectors");
	QVBoxLayout *dl = new QVBoxLayout();
	dl->addWidget(picoammeterButton_);
	dl->addWidget(scalerButton_);
	dl->setSpacing(0);
	dl->setContentsMargins(2, 2, 2, 2);
	detectorSourceBox->setLayout(dl);
	connect(SGMBeamline::sgm(), SIGNAL(detectorSignalSourceChanged(SGMBeamline::sgmDetectorSignalSource)), this, SLOT(onDetectorSignalSourceChanged(SGMBeamline::sgmDetectorSignalSource)));
	connect(detectorSignalSources_, SIGNAL(buttonClicked(int)), this, SLOT(onDetectorButtonsClicked(int)));

	endstationsAvailable_ = new QButtonGroup();
	scientaButton_ = new QRadioButton(SGMBeamline::sgm()->sgmEndstationName(SGMBeamline::scienta));
	ssaButton_ = new QRadioButton(SGMBeamline::sgm()->sgmEndstationName(SGMBeamline::ssa));
	endstationsAvailable_->addButton(scientaButton_, 0);
	endstationsAvailable_->addButton(ssaButton_, 1);
	QGroupBox *endstationsBox = new QGroupBox("Endstations");
	QVBoxLayout *sl = new QVBoxLayout();
	sl->addWidget(scientaButton_);
	sl->addWidget(ssaButton_);
	sl->setSpacing(0);
	sl->setContentsMargins(2, 2, 2, 2);
	endstationsBox->setLayout(sl);
	connect(SGMBeamline::sgm(), SIGNAL(currentEndstationChanged(SGMBeamline::sgmEndstation)), this, SLOT(onCurrentEndstationChanged(SGMBeamline::sgmEndstation)));
	connect(endstationsAvailable_, SIGNAL(buttonClicked(int)), this, SLOT(onEndstationButtonsClicked(int)));

	beamlineWarningsLabel_ = new QLabel(SGMBeamline::sgm()->beamlineWarnings());
	connect(SGMBeamline::sgm(), SIGNAL(beamlineWarningsChanged(QString)), beamlineWarningsLabel_, SLOT(setText(QString)));
	connect(SGMBeamline::sgm(), SIGNAL(beamlineWarningsChanged(QString)), this, SLOT(onBeamlineWarnings(QString)));

	// create UI elements
	imageView_ = new MPlotWidget();
	imageView_->setMinimumHeight(200);
	imagePlot_ = new MPlot();

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

	imagePlot_->addItem(i0Series_);
	// debugging:
	// connect(i0Series_->signalSource(), SIGNAL(boundsChanged()), this, SLOT(testingBoundsChanged()));
	//imagePlot_->addItem(teySeries_);
	//imagePlot_->addItem(tfySeries_);
	//imagePlot_->addItem(pdSeries_);

	stripToolCounter_ = 0;
	stripToolTimer_ = new QTimer(this);
	connect(stripToolTimer_, SIGNAL(timeout()), this, SLOT(onStripToolTimerTimeout()));
	stripToolTimer_->start(1000);

	warningAndPlotHL_ = new QHBoxLayout();
	warningAndPlotHL_->addWidget(beamlineWarningsLabel_);

	hvOnButton_ = new QPushButton("HV On");
	hvOffButton_ = new QPushButton("HV Off");
	//connect(hvOnButton_, SIGNAL(clicked()), this, SLOT(onHVOnClicked()));
	//connect(hvOffButton_, SIGNAL(clicked()), this, SLOT(onHVOffClicked()));

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
	gl_->addWidget(entranceSlitNC_,		6, 0, 1, 3, 0);
	gl_->addWidget(exitSlitNC_,		6, 3, 1, 3, 0);
	gl_->addLayout(shl,			7, 0, 1, 3, 0);
	gl_->addWidget(detectorSourceBox,	8, 0, 1, 3, 0);
	gl_->addWidget(endstationsBox,		8, 3, 1, 3, 0);
	//gl_->addWidget(hvOnButton_,		9, 0, 1, 2, 0);
	//gl_->addWidget(hvOffButton_,		9, 2, 1, 2, 0);

	gl_->addLayout(warningAndPlotHL_,	10, 0, 1, 6, 0);

	gl_->setRowStretch(9, 10);

	setLayout(mainLayout_);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

	scanningResetButton_->setContentsMargins(2,2,2,2);

	//CLSSynchronizedDwellTimeView *synchronizedDwellTimeView = new CLSSynchronizedDwellTimeView(SGMBeamline::sgm()->synchronizedDwellTime());
	//synchronizedDwellTimeView->show();
}

SGMSidebar::~SGMSidebar() {
	// must delete series objects first.  If left to ~QObject / QObject::deleteChildren() called from ~MPlot, the ~MPlotAbstractSeries() crashes when trying to disconnect from its data, because we've deleted the data already.
	delete i0Series_;
	delete teySeries_;
	delete tfySeries_;
	delete pdSeries_;
}


void SGMSidebar::showEvent(QShowEvent *se){
	int minWidth = std::max(entranceSlitNC_->size().width()/3, exitSlitNC_->size().width()/3);
	minWidth = std::max(minWidth, trackUndulatorCButton_->size().width()/2);
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
	qDebug() << "Beam on action finished";
	/* NTBA - August 25th, 2011 (David Chevrier)
			Probably need to delete the internals too, list, actions, etc"
	*/
	delete beamOnAction_;
	beamOnAction_ = 0;//NULL
}

void SGMSidebar::onStopMotorsButtonClicked(){
	if(stopMotorsAction_)
		return;
	qDebug() << "Starting the stop motors action";
	stopMotorsAction_ = SGMBeamline::sgm()->createStopMotorsAction();
	connect(stopMotorsAction_, SIGNAL(finished()), this, SLOT(onStopMotorsActionFinished()));
	stopMotorsAction_->start();
}

void SGMSidebar::onStopMotorsActionFinished(){
	qDebug() << "Motor stop SUCCEEDED";
	delete stopMotorsAction_;
	stopMotorsAction_ = 0;//NULL
}

void SGMSidebar::onDetectorSignalSourceChanged(SGMBeamline::sgmDetectorSignalSource newSource){
	if(newSource == SGMBeamline::sourcePicoammeters)
		picoammeterButton_->setChecked(true);
	else if(newSource == SGMBeamline::sourceScaler)
		scalerButton_->setChecked(true);
}

void SGMSidebar::onDetectorButtonsClicked(int buttonIndex){
	SGMBeamline::sgm()->setDetectorSignalSource((SGMBeamline::sgmDetectorSignalSource)buttonIndex);
}

void SGMSidebar::onCurrentEndstationChanged(SGMBeamline::sgmEndstation newEndstation){
	if(newEndstation == SGMBeamline::scienta)
		scientaButton_->setChecked(true);
	else if(newEndstation == SGMBeamline::ssa)
		ssaButton_->setChecked(true);
}

void SGMSidebar::onEndstationButtonsClicked(int buttonIndex){
	SGMBeamline::sgm()->setCurrentEndstation((SGMBeamline::sgmEndstation)buttonIndex);
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

void SGMSidebar::onBeamlineWarnings(const QString &newWarnings){
	if(newWarnings.isEmpty() && warningAndPlotHL_->itemAt(0)->widget() == beamlineWarningsLabel_){
		warningAndPlotHL_->removeWidget(beamlineWarningsLabel_);
		beamlineWarningsLabel_->hide();
		warningAndPlotHL_->addWidget(imageView_);
		imageView_->show();
	}
	else if(warningAndPlotHL_->itemAt(0)->widget() == imageView_){
		warningAndPlotHL_->removeWidget(imageView_);
		imageView_->hide();
		warningAndPlotHL_->addWidget(beamlineWarningsLabel_);
		beamlineWarningsLabel_->show();
	}
}

void SGMSidebar::onHVOnClicked(){
	AMBeamlineActionItem* onAction = SGMBeamline::sgm()->createHV106OnActions();
	connect(onAction, SIGNAL(succeeded()), this, SLOT(onHVOnSucceeded()));
	onAction->start();
}

void SGMSidebar::onHVOffClicked(){
	AMBeamlineActionItem* offAction = SGMBeamline::sgm()->createHV106OffActions();
	connect(offAction, SIGNAL(succeeded()), this, SLOT(onHVOffSucceeded()));
	offAction->start();
}

void SGMSidebar::onHVOnSucceeded(){
	qDebug() << "Heard on action succeeded";
	disconnect(QObject::sender(), 0, this, SLOT(onHVOnSucceeded()));
}

void SGMSidebar::onHVOffSucceeded(){
	qDebug() << "Heard off action succeeded";
	disconnect(QObject::sender(), 0, this, SLOT(onHVOffSucceeded()));
}
