/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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

SGMSidebar::SGMSidebar(QWidget *parent) :
    QWidget(parent)
{
	mainBox_ = new QGroupBox("SGM Beamline");
	mainLayout_ = new QVBoxLayout();
	mainLayout_->addWidget(mainBox_);
	gl_ = new QGridLayout();
	mainBox_->setLayout(gl_);

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

	beamlineWarningsLabel_ = new QLabel(SGMBeamline::sgm()->beamlineWarnings());
	connect(SGMBeamline::sgm(), SIGNAL(beamlineWarningsChanged(QString)), beamlineWarningsLabel_, SLOT(setText(QString)));

	// create UI elements
	imageView_ = new MPlotWidget();
	imageView_->setMinimumHeight(200);
	imagePlot_ = new MPlot();
	imageView_->setPlot(imagePlot_);
	imagePlot_->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
	imagePlot_->setScalePadding(1);
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
	//imagePlot_->addItem(teySeries_);
	//imagePlot_->addItem(tfySeries_);
	//imagePlot_->addItem(pdSeries_);

	stripToolCounter_ = 0;
	stripToolTimer_ = new QTimer(this);
	connect(stripToolTimer_, SIGNAL(timeout()), this, SLOT(onStripToolTimerTimeout()));
	stripToolTimer_->start(1000);

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
	//gl_->addWidget(beamlineWarningsLabel_,	8, 0, 1, 6, 0);
	gl_->addWidget(imageView_,		8, 0, 1, 6, 0);

	gl_->setRowStretch(7, 10);

	setLayout(mainLayout_);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
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
#warning "David, probably need to delete the internals too, list, actions, etc"
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

void SGMSidebar::onStripToolTimerTimeout(){
	if(i0Model_->count() > 50){
		i0Model_->removePointFront();
		teyModel_->removePointFront();
		tfyModel_->removePointFront();
		pdModel_->removePointFront();
	}
	i0Series_->setDescription(QString("I0 %1").arg(SGMBeamline::sgm()->i0Detector()->reading(), 0, 'e', 2));
	i0Model_->insertPointBack(stripToolCounter_, SGMBeamline::sgm()->i0Detector()->reading());
	teySeries_->setDescription(QString("TEY %1").arg(SGMBeamline::sgm()->teyDetector()->reading(), 0, 'e', 2));
	teyModel_->insertPointBack(stripToolCounter_, SGMBeamline::sgm()->teyDetector()->reading());
	tfySeries_->setDescription(QString("TFY %1").arg(SGMBeamline::sgm()->tfyDetector()->reading(), 0, 'e', 2));
	tfyModel_->insertPointBack(stripToolCounter_, SGMBeamline::sgm()->tfyDetector()->reading());
	pdSeries_->setDescription(QString("PD %1").arg(SGMBeamline::sgm()->photodiodeDetector()->reading(), 0, 'e', 2));
	pdModel_->insertPointBack(stripToolCounter_, SGMBeamline::sgm()->photodiodeDetector()->reading());
	stripToolCounter_++;
}
