/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSPersistentView.h"
#include "ui/VESPERS/VESPERSSampleStageView.h"
#include "ui/VESPERS/PIDLoopControlView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

VESPERSPersistentView::VESPERSPersistentView(QWidget *parent) :
	QWidget(parent)
{
	// The shutter buttons.
	psh1_ = new AMShutterButton("PSH", "PSH1408-B20-01:state", "PSH1408-B20-01:opr:open", "PSH1408-B20-01:opr:close");
	connect(psh1_, SIGNAL(clicked()), this, SLOT(onPSH1Clicked()));
	psh2_ = new AMShutterButton("Optic", "PSH1408-B20-02:state", "PSH1408-B20-02:opr:open", "PSH1408-B20-02:opr:close");
	connect(psh2_, SIGNAL(clicked()), this, SLOT(onPSH2Clicked()));
	ssh1_ = new AMShutterButton("SSH", "SSH1408-B20-01:state", "SSH1408-B20-01:opr:open", "SSH1408-B20-01:opr:close");
	connect(ssh1_, SIGNAL(clicked()), this, SLOT(onSSH1Clicked()));
	ssh2_ = new AMShutterButton("Exp.", "SSH1607-1-B21-01:state", "SSH1607-1-B21-01:opr:open", "SSH1607-1-B21-01:opr:close");
	connect(ssh2_, SIGNAL(clicked()), ssh2_, SLOT(changeState()));

	// Sample stage widget.
	VESPERSSampleStageView *motors = new VESPERSSampleStageView;

	// PID control view widget.
	PIDLoopControlView *pidView = new PIDLoopControlView(VESPERSBeamline::vespers()->sampleStagePID());
	connect(pidView->pid(), SIGNAL(stateChanged(bool)), motors, SLOT(setEnabled(bool)));

	// Valve group.
	valves_ = VESPERSBeamline::vespers()->valves();
	// The temperature control.
	temperature_ = VESPERSBeamline::vespers()->temperatureSet();
	connect(temperature_, SIGNAL(controlSetValuesChanged()), this, SLOT(onTemperatureStateChanged()));

	// The pressure control.
	pressure_ = VESPERSBeamline::vespers()->pressureSet();
	connect(pressure_, SIGNAL(controlSetValuesChanged()), this, SLOT(onPressureStateChanged()));

	// The flow switches.
	flowSwitches_ = VESPERSBeamline::vespers()->flowSwitchSet();
	connect(flowSwitches_, SIGNAL(controlSetValuesChanged()), this, SLOT(onWaterStateChanged()));

	// The flow transducers.
	flowTransducers_ = VESPERSBeamline::vespers()->flowTransducerSet();
	connect(flowTransducers_, SIGNAL(controlSetValuesChanged()), this, SLOT(onWaterStateChanged()));

	QFont font(this->font());
	font.setBold(true);

	QLabel *sampleStageLabel = new QLabel("Sample Stage Control");
	sampleStageLabel->setFont(font);
	QLabel *pshShutterLabel = new QLabel("Front End Shutters");
	pshShutterLabel->setFont(font);
	QLabel *sshShutterLabel = new QLabel("Beamline Shutters");
	sshShutterLabel->setFont(font);
	QLabel *endstationShutterLabel = new QLabel("Endstation Shutter");
	endstationShutterLabel->setFont(font);
	QLabel *statusLabel = new QLabel("Beamline Status");
	statusLabel->setFont(font);

	// Shutter layout.
	QHBoxLayout *frontEndShutters = new QHBoxLayout;
	frontEndShutters->addWidget(psh1_);
	frontEndShutters->addWidget(ssh1_);

	QVBoxLayout *frontEndShutterLayout = new QVBoxLayout;
	frontEndShutterLayout->setSpacing(1);
	frontEndShutterLayout->addWidget(pshShutterLabel, 0, Qt::AlignLeft);
	frontEndShutterLayout->addLayout(frontEndShutters);

	QHBoxLayout *beamlineShutters = new QHBoxLayout;
	beamlineShutters->addWidget(psh2_);
	beamlineShutters->addWidget(ssh2_);

	QVBoxLayout *beamlineShutterLayout = new QVBoxLayout;
	beamlineShutterLayout->addWidget(sshShutterLabel);
	beamlineShutterLayout->addLayout(beamlineShutters);

	// Endstation shutter control.
	filterLowerButton_ = new QPushButton("Open");
	filterLowerButton_->setCheckable(true);
	connect(filterLowerButton_, SIGNAL(clicked()), this, SLOT(onLowerFilterUpdate()));

	filterLabel_ = new QLabel;
        filterLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));
	connect(VESPERSBeamline::vespers()->filterShutterLower(), SIGNAL(valueChanged(double)), this, SLOT(onFilterStatusChanged()));

	QFormLayout *filterLayout = new QFormLayout;
	filterLayout->addRow(filterLabel_, filterLowerButton_);
	filterLayout->setHorizontalSpacing(20);

	QHBoxLayout *adjustedFilterLayout = new QHBoxLayout;
	adjustedFilterLayout->addSpacing(30);
	adjustedFilterLayout->addLayout(filterLayout);

	// The valve control.
	valvesButton_ = new QPushButton("Open Valves");
	connect(valvesButton_, SIGNAL(clicked()), this, SLOT(onValvesButtonPushed()));

	valvesStatus_ = new QLabel;
	valvesStatus_->setPixmap(QIcon(":/RED.png").pixmap(30));
	connect(valves_, SIGNAL(statusChanged(bool)), this, SLOT(onValvesStateChanged()));

	// Temp, water, and pressure labels.
	tempLabel_ = new QLabel;
	tempLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));

	pressureLabel_ = new QLabel;
	pressureLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));

	waterLabel_ = new QLabel;
	waterLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));

	QFormLayout *statusLayout = new QFormLayout;
	statusLayout->addRow(valvesStatus_, valvesButton_);
	statusLayout->addRow(tempLabel_, new QLabel("Temperature"));
	statusLayout->addRow(pressureLabel_, new QLabel("Vacuum"));
	statusLayout->addRow(waterLabel_, new QLabel("Water"));
	statusLayout->setHorizontalSpacing(20);

	QHBoxLayout *adjustedStatusLayout = new QHBoxLayout;
	adjustedStatusLayout->addSpacing(30);
	adjustedStatusLayout->addLayout(statusLayout);

	QVBoxLayout *persistentLayout = new QVBoxLayout;
	persistentLayout->addLayout(frontEndShutterLayout);
	persistentLayout->addLayout(beamlineShutterLayout);
	persistentLayout->addWidget(sampleStageLabel);
	persistentLayout->addWidget(motors);
	persistentLayout->addWidget(pidView);
	persistentLayout->addWidget(endstationShutterLabel);
	persistentLayout->addLayout(adjustedFilterLayout);
	persistentLayout->addWidget(statusLabel);
	persistentLayout->addLayout(adjustedStatusLayout);
	persistentLayout->addStretch();

	QGroupBox *vespers = new QGroupBox("VESPERS Beamline");
	vespers->setLayout(persistentLayout);
	vespers->setStyleSheet("QGroupBox { font: bold 12px; } ");

	QVBoxLayout *vespersLayout = new QVBoxLayout;
	vespersLayout->addWidget(vespers);

	setLayout(vespersLayout);
	setFixedWidth(200);
}

void VESPERSPersistentView::onFilterStatusChanged()
{
	if (((int)VESPERSBeamline::vespers()->filterShutterLower()->value()) == 1){

		filterLabel_->setPixmap(QIcon(":/ON.png").pixmap(30));
		filterLowerButton_->setText("Close");
	}
	else{

		filterLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));
		filterLowerButton_->setText("Open");
	}
}

void VESPERSPersistentView::onLowerFilterUpdate()
{
	// 0 = OUT.  For this to work properly, the upper shutter is to remain fixed at the out position and the lower shutter changes.  Therefore if upper is IN, put it out.
	if (((int)VESPERSBeamline::vespers()->filterShutterUpper()->value()) == 1)
		toggleFilter(VESPERSBeamline::vespers()->filterShutterUpper());

	toggleFilter(VESPERSBeamline::vespers()->filterShutterLower());
}

void VESPERSPersistentView::toggleFilter(AMControl *filter)
{
	AMPVControl *temp = qobject_cast<AMPVControl *>(filter);

	if (!temp)
		return;

	temp->move(1);
	temp->move(0);
}

void VESPERSPersistentView::onValvesButtonPushed()
{
	if (valves_->allValvesOpen())
		valves_->closeAllValves();
	else
		valves_->openAllValves();
}

void VESPERSPersistentView::onValvesStateChanged()
{
	if (valves_->allValvesOpen()){

		valvesButton_->setText("Close Valves");
		valvesStatus_->setPixmap(QIcon(":/ON.png").pixmap(30));
	}
	else{

		valvesButton_->setText("Open Valves");
		valvesStatus_->setPixmap(QIcon(":/RED.png").pixmap(30));
	}
}

void VESPERSPersistentView::onPressureStateChanged()
{
	bool allGood = true;
	AMReadOnlyPVwStatusControl *temp;

	for (int i = 0; i < pressure_->count(); i++){

		temp = qobject_cast<AMReadOnlyPVwStatusControl *>(pressure_->at(i));
		if (temp && temp->isMoving())
			allGood = false;
	}

	if (allGood)
		pressureLabel_->setPixmap(QIcon(":/ON.png").pixmap(30));
	else
		pressureLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));
}

void VESPERSPersistentView::onTemperatureStateChanged()
{
	bool allGood = true;
	AMReadOnlyPVControl *temp;

	for (int i = 0; i < temperature_->count(); i++){

		temp = qobject_cast<AMReadOnlyPVControl *>(temperature_->at(i));
		if (temp && temp->value() == 0)
			allGood = false;
	}

	if (allGood)
		tempLabel_->setPixmap(QIcon(":/ON.png").pixmap(30));
	else
		tempLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));
}

void VESPERSPersistentView::onWaterStateChanged()
{
	bool allGood = true;
	AMReadOnlyPVControl *temp;

	for (int i = 0; i < flowSwitches_->count(); i++){

		temp = qobject_cast<AMReadOnlyPVControl *>(flowSwitches_->at(i));
		if (temp && temp->value() == 0)
			allGood = false;
	}

	for (int i = 0; i < flowTransducers_->count(); i++){

		temp = qobject_cast<AMReadOnlyPVControl *>(flowTransducers_->at(i));
		if (temp && temp->value() == 0)
			allGood = false;
	}

	if (allGood)
		waterLabel_->setPixmap(QIcon(":/ON.png").pixmap(30));
	else
		waterLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));
}

void VESPERSPersistentView::onPSH1Clicked()
{
	// If currently open, simply close.
	if (psh1_->state() == AMShutterButton::Open)
		psh1_->close();

	// Need to check if opening is okay before opening.  Emits a message if not successful.
	else {
		if (ssh1_->state() == AMShutterButton::Open || (ssh1_->state() == AMShutterButton::Closed && psh2_->state() == AMShutterButton::Closed))
			psh1_->open();

		else
			QMessageBox::information(this, "Beamline Instructions", QString("You must open the %1 shutter before opening %2 shutter.").arg(ssh1_->title()).arg(psh1_->title()));
	}
}

void VESPERSPersistentView::onPSH2Clicked()
{
	// If currently open, simply close.
	if (psh2_->state() == AMShutterButton::Open)
		psh2_->close();

	// Need to check if opening is okay before opening.  Emits a message if not successful.
	else {
		if (ssh1_->state() == AMShutterButton::Open || (ssh1_->state() == AMShutterButton::Closed && psh1_->state() == AMShutterButton::Closed))
			psh2_->open();

		else
			QMessageBox::information(this, "Beamline Instructions", QString("You must open the %1 shutter before opening %2 shutter.").arg(ssh1_->title()).arg(psh2_->title()));
	}
}

void VESPERSPersistentView::onSSH1Clicked()
{
	// If currently closed, simply open.
	if (ssh1_->state() == AMShutterButton::Closed)
		ssh1_->open();

	// Need to check if closing is okay before closing.  Emits a message if not successful.
	else{

		if ((psh1_->state() == AMShutterButton::Open && psh2_->state() == AMShutterButton::Closed) || (psh1_->state() == AMShutterButton::Closed && psh2_->state() == AMShutterButton::Open))
			ssh1_->close();

		else
			QMessageBox::information(this, "Beamline Instructions", QString("You must close either the %1 shutter or the %2 shutter before closing the %3 shutter.").arg(psh1_->title()).arg(psh2_->title()).arg(ssh1_->title()));
	}
}
