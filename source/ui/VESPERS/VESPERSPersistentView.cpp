#include "VESPERSPersistentView.h"
#include "ui/AMShutterButton.h"
#include "ui/VESPERS/VESPERSSampleStageView.h"

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

	// Valve group.
	valves_ = VESPERSBeamline::vespers()->valves();

	QFont font(this->font());
	font.setBold(true);

	QLabel *pshShutterLabel = new QLabel("Photon Shutters");
	pshShutterLabel->setFont(font);

	QLabel *sshShutterLabel = new QLabel("Safter Shutters");
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
	valvesButton_ = new QPushButton("Open All Valves");
	connect(valvesButton_, SIGNAL(clicked()), this, SLOT(onValvesButtonPushed()));

	valvesStatus_ = new QLabel;
	valvesStatus_->setPixmap(QIcon(":/RED.png").pixmap(30));
	connect(valves_, SIGNAL(statusChanged(bool)), this, SLOT(onValvesStateChanged()));

	QHBoxLayout *valvesLayout = new QHBoxLayout;
	valvesLayout->addWidget(valvesStatus_, 0, Qt::AlignRight);
	valvesLayout->addWidget(valvesButton_, 0, Qt::AlignLeft);

	QVBoxLayout *persistentLayout = new QVBoxLayout;
	persistentLayout->addLayout(pshShutterLayout);
	persistentLayout->addLayout(sshShutterLayout);
	persistentLayout->addWidget(motors);
	persistentLayout->addWidget(statusLabel);
	persistentLayout->addLayout(valvesLayout);
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

		valvesButton_->setText("Close All Valves");
		valvesStatus_->setPixmap(QIcon(":/ON.png").pixmap(30));
	}
	else{

		valvesButton_->setText("Open All Valves");
		valvesStatus_->setPixmap(QIcon(":/RED.png").pixmap(30));
	}
}
