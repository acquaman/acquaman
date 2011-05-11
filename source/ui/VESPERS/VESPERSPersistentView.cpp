#include "VESPERSPersistentView.h"
#include "ui/VESPERS/VESPERSSampleStageView.h"
#include "ui/VESPERS/PIDLoopControlView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
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

	// The valve control.
	valvesButton_ = new QPushButton("Open Valves");
	connect(valvesButton_, SIGNAL(clicked()), this, SLOT(onValvesButtonPushed()));

	valvesStatus_ = new QLabel;
	valvesStatus_->setPixmap(QIcon(":/RED.png").pixmap(30));
	connect(valves_, SIGNAL(statusChanged(bool)), this, SLOT(onValvesStateChanged()));

	QHBoxLayout *valvesLayout = new QHBoxLayout;
	valvesLayout->addWidget(valvesStatus_, 0, Qt::AlignCenter);
	valvesLayout->addWidget(valvesButton_, 0, Qt::AlignLeft);

	// Temp, water, and pressure labels.
	tempLabel_ = new QLabel;
	tempLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));
	QHBoxLayout *tempLayout = new QHBoxLayout;
	tempLayout->addWidget(tempLabel_, 0, Qt::AlignCenter);
	tempLayout->addWidget(new QLabel("Temperature"), 0, Qt::AlignLeft);

	pressureLabel_ = new QLabel;
	pressureLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));
	QHBoxLayout *pressureLayout = new QHBoxLayout;
	pressureLayout->addWidget(pressureLabel_, 0, Qt::AlignCenter);
	pressureLayout->addWidget(new QLabel("Vacuum"), 0, Qt::AlignLeft);

	waterLabel_ = new QLabel;
	waterLabel_->setPixmap(QIcon(":/RED.png").pixmap(30));
	QHBoxLayout *waterLayout = new QHBoxLayout;
	waterLayout->addWidget(waterLabel_, 0, Qt::AlignCenter);
	waterLayout->addWidget(new QLabel("Water"), 0, Qt::AlignLeft);

	QVBoxLayout *persistentLayout = new QVBoxLayout;
	persistentLayout->addLayout(frontEndShutterLayout);
	persistentLayout->addLayout(beamlineShutterLayout);
	persistentLayout->addWidget(sampleStageLabel);
	persistentLayout->addWidget(motors);
	persistentLayout->addWidget(pidView);
	persistentLayout->addWidget(statusLabel);
	persistentLayout->addLayout(valvesLayout);
	persistentLayout->addLayout(tempLayout);
	persistentLayout->addLayout(pressureLayout);
	persistentLayout->addLayout(waterLayout);
	persistentLayout->addStretch();

	QGroupBox *vespers = new QGroupBox("VESPERS Beamline");
	vespers->setLayout(persistentLayout);
	vespers->setStyleSheet("QGroupBox::title { font: bold 14px; } ");

	QVBoxLayout *vespersLayout = new QVBoxLayout;
	vespersLayout->addWidget(vespers);

	setLayout(vespersLayout);
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
		psh1_->changeState();

	// Need to check if opening is okay before opening.  Emits a message if not successful.
	else {
		if (ssh1_->state() == AMShutterButton::Open || (ssh1_->state() == AMShutterButton::Closed && psh2_->state() == AMShutterButton::Closed))
			psh1_->changeState();

		else
			QMessageBox::information(this, "Beamline Instructions", QString("You must open the %1 shutter before opening %2 shutter.").arg(ssh1_->title()).arg(psh1_->title()));
	}
}

void VESPERSPersistentView::onPSH2Clicked()
{
	// If currently open, simply close.
	if (psh2_->state() == AMShutterButton::Open)
		psh2_->changeState();

	// Need to check if opening is okay before opening.  Emits a message if not successful.
	else {
		if (ssh1_->state() == AMShutterButton::Open || (ssh1_->state() == AMShutterButton::Closed && psh1_->state() == AMShutterButton::Closed))
			psh2_->changeState();

		else
			QMessageBox::information(this, "Beamline Instructions", QString("You must open the %1 shutter before opening %2 shutter.").arg(ssh1_->title()).arg(psh2_->title()));
	}
}

void VESPERSPersistentView::onSSH1Clicked()
{
	// If currently closed, simply open.
	if (ssh1_->state() == AMShutterButton::Closed)
		ssh1_->changeState();

	// Need to check if closing is okay before closing.  Emits a message if not successful.
	else{

		if ((psh1_->state() == AMShutterButton::Open && psh2_->state() == AMShutterButton::Closed) || (psh1_->state() == AMShutterButton::Closed && psh2_->state() == AMShutterButton::Open))
			ssh1_->changeState();

		else
			QMessageBox::information(this, "Beamline Instructions", QString("You must close either the %1 shutter or the %2 shutter before closing the %3 shutter.").arg(psh1_->title()).arg(psh2_->title()).arg(ssh1_->title()));
	}
}
