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


#include "VESPERSPersistentView.h"
#include "ui/VESPERS/VESPERSPIDLoopControlView.h"
#include "ui/VESPERS/VESPERSBeamSelectorView.h"
#include "ui/CLS/CLSIonChamberView.h"
#include "ui/CLS/CLSSplitIonChamberView.h"
#include "ui/beamline/AMIonChamberView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QMenu>
#include <QAction>

VESPERSPersistentView::VESPERSPersistentView(QWidget *parent) :
	QWidget(parent)
{
	// Temporary hack.  This is to allow some users to use the beamline while the mono application is shitting itself.
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(hackForEnergyTolerance(QPoint)));

	// The shutter buttons.
	psh1_ = new CLSStopLightButton(qobject_cast<CLSBiStateControl *>(VESPERSBeamline::vespers()->photonShutter1()));
	connect(psh1_, SIGNAL(clicked()), this, SLOT(onPSH1Clicked()));
	psh2_ = new CLSStopLightButton(qobject_cast<CLSBiStateControl *>(VESPERSBeamline::vespers()->photonShutter2()));
	connect(psh2_, SIGNAL(clicked()), this, SLOT(onPSH2Clicked()));
	ssh1_ = new CLSStopLightButton(qobject_cast<CLSBiStateControl *>(VESPERSBeamline::vespers()->safetyShutter1()));
	connect(ssh1_, SIGNAL(clicked()), this, SLOT(onSSH1Clicked()));
	ssh2_ = new CLSStopLightButton(qobject_cast<CLSBiStateControl *>(VESPERSBeamline::vespers()->safetyShutter2()));
	connect(ssh2_, SIGNAL(clicked()), this, SLOT(onSSH2Clicked()));

	motorGroupView_ = new CLSPseudoMotorGroupView(VESPERSBeamline::vespers()->motorGroup(), AMMotorGroupView::Exclusive);
	connect(motorGroupView_, SIGNAL(currentMotorGroupObjectViewChanged(QString)), this, SIGNAL(currentSampleStageChanged(QString)));

	QList<int> ids = AMDatabase::database("user")->objectsWhere("VESPERSScanConfigurationDbObject_table");

	if (!ids.isEmpty()){

		VESPERS::Motor motor = VESPERS::Motor(AMDatabase::database("user")->retrieve(ids.last(), "VESPERSScanConfigurationDbObject_table", "motor").toInt());

		if ((motor & VESPERS::H) || (motor & VESPERS::V))
			motorGroupView_->setMotorGroupView("Sample Stage - H, V, N");

		else if ((motor & VESPERS::X) || (motor & VESPERS::Z))
			motorGroupView_->setMotorGroupView("Sample Stage - X, Z, Y");

		else if ((motor & VESPERS::AttoH) || (motor & VESPERS::AttoV))
			motorGroupView_->setMotorGroupView("Attocube Stage - H, V, N");

		else if ((motor & VESPERS::AttoX) || (motor & VESPERS::AttoZ))
			motorGroupView_->setMotorGroupView("Attocube Stage - X, Z, Y");

		else if (motor & VESPERS::AttoRx)
			motorGroupView_->setMotorGroupView("Attocube Stage - Rx");

		else if (motor & VESPERS::AttoRy)
			motorGroupView_->setMotorGroupView("Attocube Stage - Ry");

		else if (motor & VESPERS::AttoRz)
			motorGroupView_->setMotorGroupView("Attocube Stage - Rz");

		else if ((motor & VESPERS::BigBeamX) || (motor & VESPERS::BigBeamZ))
			motorGroupView_->setMotorGroupView("Big Beam - X, Z");
	}


	// PID control view widgets.
	VESPERSPIDLoopControlView *pidSampleStageView = new VESPERSPIDLoopControlView(VESPERSBeamline::vespers()->sampleStagePID());
	VESPERSPIDLoopControlView *pidWireStageView = new VESPERSPIDLoopControlView(VESPERSBeamline::vespers()->wireStagePID());

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

	QLabel *pshShutterLabel = new QLabel("Front End Shutters");
	pshShutterLabel->setFont(font);
	QLabel *sshShutterLabel = new QLabel("Beamline Shutters");
	sshShutterLabel->setFont(font);
	QLabel *beamSelectionLabel = new QLabel("Beam Selection");
	beamSelectionLabel->setFont(font);
	QLabel *slitsLabel = new QLabel("Intermediate Slit Gaps");
	slitsLabel->setFont(font);
	QLabel *endstationShutterLabel = new QLabel("Endstation");
	endstationShutterLabel->setFont(font);
	QLabel *statusLabel = new QLabel("Beamline Status");
	statusLabel->setFont(font);
	QLabel *experimentReadyLabel = new QLabel("Experiment Ready Status");
	experimentReadyLabel->setFont(font);
	experimentReadyLabel->setContextMenuPolicy(Qt::CustomContextMenu);
	QLabel *ionChamberLabel = new QLabel("Ion Chamber Gain Setup");
	ionChamberLabel->setFont(font);

	// Shutter layout.
	QGridLayout *shutterLayout = new QGridLayout;
	shutterLayout->addWidget(pshShutterLabel, 0, 0, 1, 2);
	shutterLayout->addWidget(sshShutterLabel, 0, 2, 1, 2);
	shutterLayout->addWidget(psh1_);
	shutterLayout->addWidget(ssh1_);
	shutterLayout->addWidget(psh2_);
	shutterLayout->addWidget(ssh2_);

	// Beam selection and mono energy setting.
	VESPERSBeamSelectorView *beamSelectorView = new VESPERSBeamSelectorView;
	connect(VESPERSBeamline::vespers(), SIGNAL(currentBeamChanged(VESPERS::Beam)), this, SLOT(onBeamChanged(VESPERS::Beam)));

	// Energy (Eo) selection
	energySetpoint_ = new QDoubleSpinBox;
	energySetpoint_->setSuffix(" eV");
	energySetpoint_->setMinimum(0);
	energySetpoint_->setMaximum(30000);
	energySetpoint_->setAlignment(Qt::AlignCenter);
	connect(energySetpoint_, SIGNAL(editingFinished()), this, SLOT(setEnergy()));
	connect(VESPERSBeamline::vespers()->mono(), SIGNAL(EoChanged(double)), this, SLOT(onEnergyChanged(double)));

	energyFeedback_ = new QLabel;
	energyFeedback_->setAlignment(Qt::AlignCenter);
	connect(VESPERSBeamline::vespers()->mono(), SIGNAL(energyChanged(double)), this, SLOT(onEnergyFeedbackChanged(double)));

	energyTolerance_ = new VESPERSChangeEnergyToleranceHackView;
	energyTolerance_->hide();

	QHBoxLayout *energySetpointLayout = new QHBoxLayout;
	energySetpointLayout->addWidget(new QLabel("Energy:"));
	energySetpointLayout->addWidget(energySetpoint_);
	energySetpointLayout->addWidget(energyFeedback_);
	energySetpointLayout->setContentsMargins(15, 11, 11, 11);

	QVBoxLayout *beamSelectionLayout = new QVBoxLayout;
	beamSelectionLayout->addWidget(beamSelectionLabel);
	beamSelectionLayout->addWidget(beamSelectorView, 0, Qt::AlignCenter);
	beamSelectionLayout->addLayout(energySetpointLayout);
	beamSelectionLayout->addWidget(energyTolerance_);

	// The intermediate slits.
	slits_ = VESPERSBeamline::vespers()->intermediateSlits();

	xSlit_ = new QDoubleSpinBox;
	xSlit_->setSuffix(" mm");
	xSlit_->setDecimals(3);
	xSlit_->setSingleStep(0.001);
	connect(slits_, SIGNAL(gapXChanged(double)), xSlit_, SLOT(setValue(double)));
	connect(xSlit_, SIGNAL(editingFinished()), this, SLOT(setXGap()));

	zSlit_ = new QDoubleSpinBox;
	zSlit_->setSuffix(" mm");
	zSlit_->setDecimals(3);
	zSlit_->setSingleStep(0.001);
	connect(slits_, SIGNAL(gapZChanged(double)), zSlit_, SLOT(setValue(double)));
	connect(zSlit_, SIGNAL(editingFinished()), this, SLOT(setZGap()));

	QHBoxLayout *slitsLayout = new QHBoxLayout;
	slitsLayout->addWidget(new QLabel("H:"), 0, Qt::AlignRight);
	slitsLayout->addWidget(xSlit_);
	slitsLayout->addWidget(new QLabel("V:"), 0, Qt::AlignRight);
	slitsLayout->addWidget(zSlit_);
	slitsLayout->setContentsMargins(15, 11, 11, 11);

	// The Experiment Ready Status
	experimentReady_ = new QLabel;
	experimentReady_->setPixmap(QIcon(":/RED.png").pixmap(25));
	experimentReady_->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(experimentReady_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onPOEStatusEnablePopupMenuRequested(QPoint)));
	connect(experimentReadyLabel, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onPOEStatusEnablePopupMenuRequested(QPoint)));
	connect(VESPERSBeamline::vespers(), SIGNAL(poeStatusChanged(bool)), this, SLOT(onExperimentStatusChanged()));
	connect(VESPERSBeamline::vespers(), SIGNAL(poeStatusEnableChanged()), this, SLOT(onExperimentStatusChanged()));

	QHBoxLayout *experimentReadyLayout = new QHBoxLayout;
	experimentReadyLayout->addWidget(experimentReady_);
	experimentReadyLayout->addWidget(experimentReadyLabel);
	experimentReadyLayout->setSpacing(10);
	experimentReadyLayout->setContentsMargins(15, 0, 11, 0);
	experimentReadyLayout->addStretch();

	// Endstation shutter control.
	filterLowerButton_ = new QPushButton("Open Shutter");
	filterLowerButton_->setCheckable(true);
	connect(filterLowerButton_, SIGNAL(clicked()), this, SLOT(toggleShutterState()));

	filterLabel_ = new QLabel;
	filterLabel_->setPixmap(QIcon(":/RED.png").pixmap(25));
	connect(VESPERSBeamline::vespers()->endstation(), SIGNAL(shutterChanged(bool)), this, SLOT(onShutterStateChanged(bool)));

	// Setup the filters.
	filterComboBox_ = new QComboBox;
	filterComboBox_->addItem("None");
	filterComboBox_->addItem(QString::fromUtf8("50 μm"));
	filterComboBox_->addItem(QString::fromUtf8("100 μm"));
	filterComboBox_->addItem(QString::fromUtf8("150 μm"));
	filterComboBox_->addItem(QString::fromUtf8("200 μm"));
	filterComboBox_->addItem(QString::fromUtf8("250 μm"));
	filterComboBox_->addItem(QString::fromUtf8("300 μm"));
	filterComboBox_->addItem(QString::fromUtf8("350 μm"));
	filterComboBox_->addItem(QString::fromUtf8("400 μm"));
	filterComboBox_->addItem(QString::fromUtf8("450 μm"));
	filterComboBox_->addItem(QString::fromUtf8("500 μm"));
	filterComboBox_->addItem(QString::fromUtf8("550 μm"));
	filterComboBox_->addItem(QString::fromUtf8("600 μm"));
	filterComboBox_->addItem(QString::fromUtf8("650 μm"));
	filterComboBox_->addItem(QString::fromUtf8("700 μm"));
	filterComboBox_->addItem(QString::fromUtf8("750 μm"));
	filterComboBox_->addItem(QString::fromUtf8("800 μm"));
	connect(filterComboBox_, SIGNAL(currentIndexChanged(int)), VESPERSBeamline::vespers()->endstation(), SLOT(setFilterThickness(int)));
	connect(VESPERSBeamline::vespers()->endstation(), SIGNAL(filterThicknessChanged(int)), this, SLOT(onFiltersChanged(int)));

	QHBoxLayout *filterLayout = new QHBoxLayout;
	filterLayout->addWidget(filterLabel_);
	filterLayout->addWidget(filterLowerButton_);
	filterLayout->addWidget(new QLabel("Filters:"));
	filterLayout->addWidget(filterComboBox_);
	filterLayout->setSpacing(5);
	filterLayout->setContentsMargins(15, 11, 11, 11);

	// The valve control.
	valvesButton_ = new QPushButton("Open Valves");
	connect(valvesButton_, SIGNAL(clicked()), this, SLOT(onValvesButtonPushed()));

	valvesStatus_ = new QLabel;
	valvesStatus_->setPixmap(QIcon(":/RED.png").pixmap(25));
	connect(VESPERSBeamline::vespers()->valveSet(), SIGNAL(controlSetValuesChanged()), this, SLOT(onValvesStateChanged()));

	QLabel *valveIcon = new QLabel;
	valveIcon->setPixmap(QIcon(":/valveIcon.png").pixmap(25));
	valveIcon->setToolTip("Valve Indicator");

	// Temp, water, and pressure labels.
	tempLabel_ = new QLabel;
	tempLabel_->setPixmap(QIcon(":/RED.png").pixmap(25));
	QLabel *temperatureIcon = new QLabel;
	temperatureIcon->setPixmap(QIcon(":/ThermometerIcon.png").pixmap(25));
	temperatureIcon->setToolTip("Temperature Indicator");

	pressureLabel_ = new QLabel;
	pressureLabel_->setPixmap(QIcon(":/RED.png").pixmap(25));
	QLabel *pressureIcon = new QLabel;
	pressureIcon->setPixmap(QIcon(":/PressureIcon.png").pixmap(25));
	pressureIcon->setToolTip("Pressure Indicator");

	waterLabel_ = new QLabel;
	waterLabel_->setPixmap(QIcon(":/RED.png").pixmap(25));
	QLabel *waterIcon = new QLabel;
	waterIcon->setPixmap(QIcon(":/FaucetIcon.png").pixmap(25));
	waterIcon->setToolTip("Water Indicator");

	// Ion chambers.
	QVBoxLayout *ionChamberLayout = new QVBoxLayout;
	ionChamberLayout->addWidget(new CLSSplitIonChamberView(VESPERSBeamline::vespers()->iSplit()));
	ionChamberLayout->addWidget(new CLSIonChamberView(VESPERSBeamline::vespers()->iPreKB()));
	ionChamberLayout->addWidget(new CLSIonChamberView(VESPERSBeamline::vespers()->iMini()));
	ionChamberLayout->addWidget(new CLSIonChamberView(VESPERSBeamline::vespers()->iPost()));
	ionChamberLayout->setContentsMargins(10, 0, 10, 0);

	// Layout.
	QGridLayout *statusLayout = new QGridLayout;
	statusLayout->addWidget(temperatureIcon, 0, 0);
	statusLayout->addWidget(pressureIcon, 0, 1);
	statusLayout->addWidget(waterIcon, 0, 2);
	statusLayout->addWidget(valveIcon, 0, 3);
	statusLayout->addWidget(tempLabel_, 1, 0);
	statusLayout->addWidget(pressureLabel_, 1, 1);
	statusLayout->addWidget(waterLabel_, 1, 2);
	statusLayout->addWidget(valvesStatus_, 1, 3);
	statusLayout->addWidget(valvesButton_, 1, 4, 1, 2);
	statusLayout->setContentsMargins(15, 0, 11, 0);

	QVBoxLayout *persistentLayout = new QVBoxLayout;
	persistentLayout->addLayout(shutterLayout);
	persistentLayout->addLayout(beamSelectionLayout);
	persistentLayout->addWidget(slitsLabel);
	persistentLayout->addLayout(slitsLayout);
	persistentLayout->addWidget(motorGroupView_, 0, Qt::AlignLeft);
	persistentLayout->addWidget(pidSampleStageView);
	persistentLayout->addWidget(pidWireStageView);
	persistentLayout->addLayout(experimentReadyLayout);
	persistentLayout->addWidget(endstationShutterLabel);
	persistentLayout->addLayout(filterLayout);
	persistentLayout->addWidget(ionChamberLabel);
	persistentLayout->addLayout(ionChamberLayout);
	persistentLayout->addWidget(statusLabel);
	persistentLayout->addLayout(statusLayout);
	persistentLayout->addStretch();

	QGroupBox *vespersBox = new QGroupBox;
	vespersBox->setLayout(persistentLayout);

	QVBoxLayout *vespersLayout = new QVBoxLayout;
	vespersLayout->addWidget(vespersBox);

	setLayout(vespersLayout);
	setFixedSize(325, 900);
}

#include <QMenu>
void VESPERSPersistentView::hackForEnergyTolerance(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Change energy tolerance");

	temp = popup.exec(mapToGlobal(pos));

	if (temp && temp->text() == "Change energy tolerance"){

		energyTolerance_->setVisible(!energyTolerance_->isVisible());
	}
}

void VESPERSPersistentView::onBeamChanged(VESPERS::Beam beam)
{
	switch(beam){

	case VESPERS::NoBeam:
	case VESPERS::Pink:
		energySetpoint_->setEnabled(false);
		break;

	case VESPERS::TenPercent:
	case VESPERS::OnePointSixPercent:
	case VESPERS::Si:
		energySetpoint_->setEnabled(true);
		break;
	}
}

void VESPERSPersistentView::onShutterStateChanged(bool state)
{
	if (state){

		filterLabel_->setPixmap(QIcon(":/ON.png").pixmap(25));
		filterLowerButton_->setText("Close Shutter");
	}

	else{

		filterLabel_->setPixmap(QIcon(":/RED.png").pixmap(25));
		filterLowerButton_->setText("Open Shutter");
	}
}

void VESPERSPersistentView::onExperimentStatusChanged()
{
	if (VESPERSBeamline::vespers()->poeStatusEnable())
		experimentReady_->setPixmap(QIcon(VESPERSBeamline::vespers()->poeStatus() ? ":/ON.png" : ":/RED.png").pixmap(25));

	else
		experimentReady_->setPixmap(QIcon(":/Yellow.png").pixmap(25));
}

void VESPERSPersistentView::onPOEStatusEnablePopupMenuRequested(const QPoint &point)
{
	QMenu popup(experimentReady_);
	QAction *temp = popup.addAction("Enable POE Status");
	temp->setCheckable(true);
	temp->setChecked(VESPERSBeamline::vespers()->poeStatusEnable());

	temp = popup.exec(experimentReady_->mapToGlobal(point));

	if (temp)
		VESPERSBeamline::vespers()->setPOEStatusEnable(!VESPERSBeamline::vespers()->poeStatusEnable());
}

void VESPERSPersistentView::toggleShutterState()
{
	VESPERSBeamline::vespers()->endstation()->setShutterState(!VESPERSBeamline::vespers()->endstation()->shutterState());
}

void VESPERSPersistentView::onValvesButtonPushed()
{
	if (VESPERSBeamline::vespers()->allValvesOpen())
		VESPERSBeamline::vespers()->closeAllValves();
	else
		VESPERSBeamline::vespers()->openAllValves();
}

void VESPERSPersistentView::onValvesStateChanged()
{
	if (VESPERSBeamline::vespers()->allValvesOpen()){

		valvesButton_->setText("Close Valves");
		valvesStatus_->setPixmap(QIcon(":/ON.png").pixmap(25));
	}
	else{

		valvesButton_->setText("Open Valves");
		valvesStatus_->setPixmap(QIcon(":/RED.png").pixmap(25));
	}
}

void VESPERSPersistentView::onPressureStateChanged()
{
	bool allGood = true;

	for (int i = 0; i < pressure_->count(); i++){

		if (pressure_->at(i)->isMoving())
			allGood = false;
	}

	pressureLabel_->setPixmap(QIcon(allGood ? ":/ON.png" : ":/RED.png").pixmap(25));
}

void VESPERSPersistentView::onTemperatureStateChanged()
{
	bool allGood = true;

	for (int i = 0; i < temperature_->count(); i++){

		if (temperature_->at(i)->isMoving())
			allGood = false;
	}

	tempLabel_->setPixmap(QIcon(allGood ? ":/ON.png" : ":/RED.png").pixmap(25));
}

void VESPERSPersistentView::onWaterStateChanged()
{
	bool allGood = true;

	for (int i = 0; i < flowSwitches_->count(); i++){

		if (flowSwitches_->at(i)->value() == 0)
			allGood = false;
	}

	for (int i = 0; i < flowTransducers_->count(); i++){

		if (flowTransducers_->at(i)->isMoving())
			allGood = false;
	}

	waterLabel_->setPixmap(QIcon(allGood ? ":/ON.png" : ":/RED.png").pixmap(25));
}

void VESPERSPersistentView::onPSH1Clicked()
{
	// If currently open, simply close.
	if (VESPERSBeamline::vespers()->photonShutter1()->value() == 1)
		VESPERSBeamline::vespers()->closePhotonShutter1();

	// Need to check if opening is okay before opening.  Emits a message if not successful.
	else if (VESPERSBeamline::vespers()->photonShutter1()->value() == 0 && !VESPERSBeamline::vespers()->openPhotonShutter1())
		QMessageBox::information(this, "Beamline Instructions", QString("You must open the %1 shutter before opening %2 shutter.").arg(VESPERSBeamline::vespers()->safetyShutter1()->name()).arg(VESPERSBeamline::vespers()->photonShutter1()->name()));
}

void VESPERSPersistentView::onPSH2Clicked()
{
	// If currently open, simply close.
	if (VESPERSBeamline::vespers()->photonShutter2()->value() == 1)
		VESPERSBeamline::vespers()->closePhotonShutter2();

	// Need to check if opening is okay before opening.  Emits a message if not successful.
	else if (VESPERSBeamline::vespers()->photonShutter2()->value() == 0 && !VESPERSBeamline::vespers()->openPhotonShutter2())
		QMessageBox::information(this, "Beamline Instructions", QString("You must open the %1 shutter before opening %2 shutter.").arg(VESPERSBeamline::vespers()->safetyShutter1()->name()).arg(VESPERSBeamline::vespers()->photonShutter2()->name()));
}

void VESPERSPersistentView::onSSH1Clicked()
{
	// If currently closed, simply open.
	if (VESPERSBeamline::vespers()->safetyShutter1()->value() == 0)
		VESPERSBeamline::vespers()->openSafetyShutter1();

	// Need to check if closing is okay before closing.  Emits a message if not successful.
	else if (VESPERSBeamline::vespers()->safetyShutter1()->value() == 1 && !VESPERSBeamline::vespers()->closeSafetyShutter1())
		QMessageBox::information(this, "Beamline Instructions", QString("You must close either the %1 shutter or the %2 shutter before closing the %3 shutter.").arg(VESPERSBeamline::vespers()->photonShutter1()->name()).arg(VESPERSBeamline::vespers()->photonShutter2()->name()).arg(VESPERSBeamline::vespers()->safetyShutter1()->name()));
}

void VESPERSPersistentView::onSSH2Clicked()
{
	if (VESPERSBeamline::vespers()->safetyShutter2()->value() == 0)
		VESPERSBeamline::vespers()->openSafetyShutter2();

	else if (VESPERSBeamline::vespers()->safetyShutter2()->value() == 1)
		VESPERSBeamline::vespers()->closeSafetyShutter2();
}
