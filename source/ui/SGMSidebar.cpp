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

SGMSidebar::SGMSidebar(QWidget *parent) :
    QWidget(parent)
{
	mainBox_ = new QGroupBox("SGM Beamline");
	mainLayout_ = new QVBoxLayout();
	mainLayout_->addWidget(mainBox_);
	//vl_ = new QVBoxLayout();
	gl_ = new QGridLayout();
	//mainBox_->setLayout(vl_);
	mainBox_->setLayout(gl_);

	readyLabel_ = new AMControlEdit(SGMBeamline::sgm()->beamlineReady(), NULL, true);
	readyLabel_->setNoUnitsBox(true);
	readyLabel_->overrideTitle("");
//	beamOnCButton_ = new AMControlButton(SGMBeamline::sgm()->beamOn());
//	beamOnCButton_->overrideText("Beam On");
//	beamOnCButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	beamOnBALButton_ = new AMBeamlineActionsListButton(SGMBeamline::sgm()->beamOnActionsList());
	beamOnBALButton_->overrideText("Beam On");
	beamOnBALButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	beamOffCButton_ = new AMControlButton(SGMBeamline::sgm()->fastShutterVoltage());
	beamOffCButton_->overrideText("Beam Off");
	beamOffCButton_->setDownValue(5);
	beamOffCButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	stopMotorsButton_ = new QToolButton();
	stopMotorsButton_->setText("Emergency\nMotor Stop");
	stopMotorsButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	connect(stopMotorsButton_, SIGNAL(clicked()), this, SLOT(onStopMotorsButtonClicked()));
	closeVacuumButton_ = new QToolButton();
	closeVacuumButton_->setText("Close Vacuum");
	closeVacuumButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	connect(closeVacuumButton_, SIGNAL(clicked()), this, SLOT(onCloseVacuumButtonClicked()));
	visibleLightButton_ = new QToolButton();
	visibleLightButton_->setText("Visible Light");
	visibleLightButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	connect(visibleLightButton_, SIGNAL(clicked()), this, SLOT(onVisibleLightClicked()));
	connect(SGMBeamline::sgm(), SIGNAL(visibleLightStatusChanged(QString)), this, SLOT(onVisibleLightStatusChanged(QString)));
	energyNC_ = new AMControlEdit(SGMBeamline::sgm()->energy(), SGMBeamline::sgm()->energyMovingStatus());
	energyNC_->overrideTitle("Energy");
	trackUndulatorCButton_ = new AMControlButton(SGMBeamline::sgm()->undulatorTracking());
	trackUndulatorCButton_->overrideText("Undulator");
	trackUndulatorCButton_->setCheckable(true);
	trackUndulatorCButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	trackGratingCButton_ = new AMControlButton(SGMBeamline::sgm()->monoTracking());
	trackGratingCButton_->overrideText("Grating");
	trackGratingCButton_->setCheckable(true);
	trackGratingCButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	trackExitSlitCButton_ = new AMControlButton(SGMBeamline::sgm()->exitSlitTracking());
	trackExitSlitCButton_->overrideText("Exit Slit");
	trackExitSlitCButton_->setCheckable(true);
	trackExitSlitCButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	gratingNC_ = new AMControlEdit(SGMBeamline::sgm()->grating());
	gratingNC_->overrideTitle("Grating");
	entranceSlitNC_ = new AMControlEdit(SGMBeamline::sgm()->entranceSlitGap());
	entranceSlitNC_->overrideTitle("Entrance Slit");
	exitSlitNC_ = new AMControlEdit(SGMBeamline::sgm()->exitSlitGap());
	exitSlitNC_->overrideTitle("Exit Slit");

	gl_->addWidget(readyLabel_,		0, 0, 1, 6, 0);
	gl_->addWidget(beamOnBALButton_,	1, 0, 1, 2, 0);
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

	setLayout(mainLayout_);

	qDebug() << "\n\n\nPLAYING WITH PARALLEL LIST";
	AMBeamlineControlMoveAction *tmpAction;
	al = new AMBeamlineParallelActionsList(this);
	al->appendStage(new QList<AMBeamlineActionItem*>());
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy(),this);
	tmpAction->setSetpoint(500);
	al->appendAction(0, tmpAction);
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

void SGMSidebar::onStopMotorsButtonClicked(){
	qDebug() << "Starting the parallel actions list";
	al->start();
}
