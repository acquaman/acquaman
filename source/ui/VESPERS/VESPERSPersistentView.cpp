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


#include "VESPERSPersistentView.h"
#include "ui/VESPERS/VESPERSPIDLoopControlView.h"
#include "ui/VESPERS/VESPERSBeamSelectorView.h"
#include "ui/VESPERS/VESPERSScalerView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QMenu>
#include <QAction>

 VESPERSPersistentView::~VESPERSPersistentView(){}
VESPERSPersistentView::VESPERSPersistentView(QWidget *parent) :
	QWidget(parent)
{
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
	motorGroupView_->setMotorGroupView("Sample Stage - H, V, N");

	// PID control view widgets.
	VESPERSPIDLoopControlView *pidSampleStageView = new VESPERSPIDLoopControlView(VESPERSBeamline::vespers()->sampleStagePID());
	VESPERSPIDLoopControlView *pidWireStageView = new VESPERSPIDLoopControlView(VESPERSBeamline::vespers()->wireStagePID());

	// The valve control.
	valves_ = VESPERSBeamline::vespers()->valveSet();
	connect(valves_, SIGNAL(controlSetValuesChanged()), this, SLOT(onValvesStateChanged()));

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

	energyLabel_ = new QLabel("Energy:");

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

	QHBoxLayout *energySetpointLayout = new QHBoxLayout;
	energySetpointLayout->addWidget(energyLabel_);
	energySetpointLayout->addWidget(energySetpoint_);
	energySetpointLayout->addWidget(energyFeedback_);
	energySetpointLayout->setContentsMargins(15, 11, 11, 11);

	QVBoxLayout *beamSelectionLayout = new QVBoxLayout;
	beamSelectionLayout->addWidget(beamSelectionLabel);
	beamSelectionLayout->addWidget(beamSelectorView, 0, Qt::AlignCenter);
	beamSelectionLayout->addLayout(energySetpointLayout);

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

	valveStatusButton_ = new QToolButton;
	valveStatusButton_->setIcon(QIcon(":/valveIcon.png"));
	valveStatusButton_->setToolTip("Valve Indicator");
	valveStatusButton_->setAutoRaise(true);
	connect(valveStatusButton_, SIGNAL(clicked()), this, SLOT(onValvesStatusClicked()));

	QLabel *valveIcon = new QLabel;
	valveIcon->setPixmap(QIcon(":/valveIcon.png").pixmap(25));
	valveIcon->setToolTip("Valve Indicator");

	// Temp, water, and pressure labels.
	temperatureStatusButton_ = new QToolButton;
	temperatureStatusButton_->setIcon(QIcon(":/ThermometerIcon.png"));
	temperatureStatusButton_->setToolTip("Temperature Indicator");
	temperatureStatusButton_->setAutoRaise(true);
	connect(temperatureStatusButton_, SIGNAL(clicked()), this, SLOT(onTemperatureStatusClicked()));

	pressureStatusButton_ = new QToolButton;
	pressureStatusButton_->setIcon(QIcon(":/PressureIcon.png"));
	pressureStatusButton_->setToolTip("Pressure Indicator");
	pressureStatusButton_->setAutoRaise(true);
	connect(pressureStatusButton_, SIGNAL(clicked()), this, SLOT(onPressureStatusClicked()));

	waterStatusButton_ = new QToolButton;
	waterStatusButton_->setIcon(QIcon(":/FaucetIcon.png"));
	waterStatusButton_->setToolTip("Water Indicator");
	waterStatusButton_->setAutoRaise(true);
	connect(waterStatusButton_, SIGNAL(clicked()), this, SLOT(onWaterTransducerStatusClicked()));

	// Layout.
	QGridLayout *statusLayout = new QGridLayout;
	statusLayout->addWidget(temperatureStatusButton_, 0, 0);
	statusLayout->addWidget(pressureStatusButton_, 0, 1);
	statusLayout->addWidget(waterStatusButton_, 0, 2);
	statusLayout->addWidget(valveStatusButton_, 0, 3);
	statusLayout->addWidget(valvesButton_, 0, 4, 1, 2);
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
	persistentLayout->addWidget(new VESPERSScalerView());
	persistentLayout->addWidget(statusLabel);
	persistentLayout->addLayout(statusLayout);
	persistentLayout->addStretch();

	QGroupBox *vespersBox = new QGroupBox;
	vespersBox->setLayout(persistentLayout);

	QVBoxLayout *vespersLayout = new QVBoxLayout;
	vespersLayout->addWidget(vespersBox);

	setLayout(vespersLayout);
	setFixedWidth(350);
}

void VESPERSPersistentView::onBeamChanged(VESPERS::Beam beam)
{
	switch(beam){

	case VESPERS::NoBeam:
	case VESPERS::Pink:
		energyLabel_->setVisible(false);
		energySetpoint_->setVisible(false);
		energyFeedback_->setVisible(false);
		break;

	case VESPERS::TenPercent:
	case VESPERS::OnePointSixPercent:
	case VESPERS::Si:
		energyLabel_->setVisible(true);
		energySetpoint_->setVisible(true);
		energyFeedback_->setVisible(true);
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
	bool allGood = true;

	for (int i = 0, size = valves_->count(); i < size; i++)
		if (valves_->at(i)->isMoving())
			allGood = false;

	valvesButton_->setText(allGood ? "Close Valves" : "Open Valves");
	valveStatusButton_->setAutoRaise(allGood);
	valveStatusButton_->setStyleSheet(allGood ? "" : "QToolButton { background-color: red }");
}

void VESPERSPersistentView::onPressureStateChanged()
{
	bool allGood = true;

	for (int i = 0, size = pressure_->count(); i < size && allGood; i++)
		if (pressure_->at(i)->isMoving())
			allGood = false;

	pressureStatusButton_->setAutoRaise(allGood);
	pressureStatusButton_->setStyleSheet(allGood ? "" : "QToolButton { background-color: red }");
}

void VESPERSPersistentView::onTemperatureStateChanged()
{
	bool allGood = true;

	for (int i = 0, size = temperature_->count(); i < size && allGood; i++)
		if (temperature_->at(i)->isMoving())
			allGood = false;

	temperatureStatusButton_->setAutoRaise(allGood);
	temperatureStatusButton_->setStyleSheet(allGood ? "" : "QToolButton { background-color: red }");
}

void VESPERSPersistentView::onWaterStateChanged()
{
	bool allGood = true;

	for (int i = 0, size = flowSwitches_->count(); i < size && allGood; i++)
		if (flowSwitches_->at(i)->value() == 0)
			allGood = false;

	for (int i = 0, size = flowTransducers_->count(); i < size && allGood; i++)
		if (flowTransducers_->at(i)->isMoving())
			allGood = false;

	waterStatusButton_->setAutoRaise(allGood);
	waterStatusButton_->setStyleSheet(allGood ? "" : "QToolButton { background-color: red }");
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

void VESPERSPersistentView::onValvesStatusClicked()
{
	emit statusButtonClicked("Valves");
}

void VESPERSPersistentView::onPressureStatusClicked()
{
	emit statusButtonClicked("Pressure");
}

void VESPERSPersistentView::onTemperatureStatusClicked()
{
	emit statusButtonClicked("Temperature");
}

void VESPERSPersistentView::onWaterTransducerStatusClicked()
{
	emit statusButtonClicked("Water Pressure");
}
