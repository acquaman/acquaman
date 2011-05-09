#include "VESPERSPersistentView.h"
#include "ui/AMShutterButton.h"
#include "ui/VESPERS/VESPERSSampleStageView.h"
#include "ui/VESPERS/PIDLoopControlView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

VESPERSPersistentView::VESPERSPersistentView(QWidget *parent) :
	QWidget(parent)
{
	// The shutter buttons.
	AMShutterButton *psh1 = new AMShutterButton("PSH1", "PSH1408-B20-01:state", "PSH1408-B20-01:opr:open", "PSH1408-B20-01:opr:close");
	AMShutterButton *psh2 = new AMShutterButton("PSH2", "PSH1408-B20-02:state", "PSH1408-B20-02:opr:open", "PSH1408-B20-02:opr:close");
	AMShutterButton *ssh1 = new AMShutterButton("SSH1", "SSH1408-B20-01:state", "SSH1408-B20-01:opr:open", "SSH1408-B20-01:opr:close");
	AMShutterButton *ssh2 = new AMShutterButton("SSH2", "SSH1607-1-B21-01:state", "SSH1607-1-B21-01:opr:open", "SSH1607-1-B21-01:opr:close");

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

	QLabel *pshShutterLabel = new QLabel("Photon Shutters");
	pshShutterLabel->setFont(font);

	QLabel *sshShutterLabel = new QLabel("Safety Shutters");
	sshShutterLabel->setFont(font);

	QLabel *statusLabel = new QLabel("Beamline Status");
	statusLabel->setFont(font);

	// Shutter layout.
	QHBoxLayout *pshShutters = new QHBoxLayout;
	pshShutters->addWidget(psh1);
	pshShutters->addWidget(psh2);

	QVBoxLayout *pshShutterLayout = new QVBoxLayout;
	pshShutterLayout->setSpacing(1);
	pshShutterLayout->addWidget(pshShutterLabel, 0, Qt::AlignLeft);
	pshShutterLayout->addLayout(pshShutters);

	QHBoxLayout *sshShutters = new QHBoxLayout;
	sshShutters->addWidget(ssh1);
	sshShutters->addWidget(ssh2);

	QVBoxLayout *sshShutterLayout = new QVBoxLayout;
	sshShutterLayout->addWidget(sshShutterLabel);
	sshShutterLayout->addLayout(sshShutters);

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
	persistentLayout->addLayout(pshShutterLayout);
	persistentLayout->addLayout(sshShutterLayout);
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
	vespers->setFlat(true);

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
