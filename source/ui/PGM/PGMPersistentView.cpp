#include "PGMPersistentView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QMenu>
#include <QAction>


PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
	QLayout *shutterLayout = createAndLayoutBeamlingShutters();



    valveAButton_ = new QPushButton("Open Branch A");
    connect(valveAButton_, SIGNAL(clicked()), this, SLOT(onValveAButtonPushed()));

    valveBButton_ = new QPushButton("Open Branch B");
    connect(valveBButton_, SIGNAL(clicked()), this, SLOT(onValveBButtonPushed()));

    // Set the valve status on startup
    onValveStatusChanged();

    // Valve Layout
    QGridLayout *valveLayout = new QGridLayout;
    valveLayout->addWidget(valveAButton_, 0, 0);
    valveLayout->addWidget(valveBButton_, 0 ,1);

    QVBoxLayout *persistentLayout = new QVBoxLayout;
    persistentLayout->addLayout(shutterLayout);
    persistentLayout->addLayout(valveLayout);

    persistentLayout->addStretch();

    QGroupBox *pgmBox = new QGroupBox("VLS-PGM");
    pgmBox->setLayout(persistentLayout);

    QVBoxLayout *pgmLayout = new QVBoxLayout;
    pgmLayout->addWidget(pgmBox);

    setLayout(pgmLayout);
    setFixedWidth(350);

}

// Sets the current UI elements based on the start-up status of the valves.
void PGMPersistentView::onValveStatusChanged(){

    if(PGMBeamline::pgm()->branchAOpen()){
        onValveAButtonPushed();
    }
    else if (PGMBeamline::pgm()->branchBOpen()){
        onValveBButtonPushed();
    }
}


void PGMPersistentView::onValveAButtonPushed(){
    // Change UI elements
    valveAButton_->setText("Branch A Open");
    valveAButton_->setStyleSheet("background-color: rgb(0, 170, 0)");

    valveBButton_->setText("Branch B Close");
    valveBButton_->setStyleSheet("background-color: rgb(170, 0, 0)");


    // Change valve status on machine.
    if(!PGMBeamline::pgm()->branchAOpen()){
        // Close Branch B first.
        PGMBeamline::pgm()->closeValve(1);
        // Open Branch A.
        PGMBeamline::pgm()->openValve(0);
    }
}

void PGMPersistentView::onValveBButtonPushed(){
    // Change UI elements
    valveBButton_->setText("Branch B Open");
    valveBButton_->setStyleSheet("background-color: rgb(0, 170, 0)");

    valveAButton_->setText("Branch A Closed");
    valveAButton_->setStyleSheet("background-color: rgb(170,0 , 0)");

    // Change valve status on machine.
    if(!PGMBeamline::pgm()->branchBOpen()){
        // Close Branch A first.
        PGMBeamline::pgm()->closeValve(0);
        // Open Branch B.
        PGMBeamline::pgm()->openValve(1);
    }
}


void PGMPersistentView::onPhotonShutter3ValueChanged(double state){
    // Open
    if(state == 1){
        photonShutter3Button_->setIcon(QIcon(":/32x32/greenLEDOn.png").pixmap(25));
    }
    // Between
    else if (state == 2){
        photonShutter3Button_->setIcon(QIcon(":/32x32/yellowLEDOn.png").pixmap(25));

    }
    // Closed
    else if (state == 4){
        photonShutter3Button_->setIcon(QIcon(":/32x32/redLEDOn.png").pixmap(25));
    }


}

void PGMPersistentView::onPhotonShutter2ValueChanged(double state){
    // Open
    if(state == 1){
        photonShutter2Button_->setIcon(QIcon(":/32x32/greenLEDOn.png").pixmap(25));
    }
    // Between
    else if (state == 2){
        photonShutter2Button_->setIcon(QIcon(":/32x32/yellowLEDOn.png").pixmap(25));

    }
    // Closed
    else if (state == 4){
        photonShutter2Button_->setIcon(QIcon(":/32x32/redLEDOn.png").pixmap(25));
    }


}

void PGMPersistentView::onPhotonShutter1ValueChanged(double state){
    // Open
    if(state == 1){
        photonShutter1Button_->setIcon(QIcon(":/32x32/greenLEDOn.png").pixmap(25));
    }
    // Between
    else if (state == 2){
        photonShutter1Button_->setIcon(QIcon(":/32x32/yellowLEDOn.png").pixmap(25));

    }
    // Closed
    else if (state == 4){
        photonShutter1Button_->setIcon(QIcon(":/32x32/redLEDOn.png").pixmap(25));
    }

}

void PGMPersistentView::onSafetyShutter1ValueChanged(double state){
    // Open
    if(state == 1){
        safetyShutter1Button_->setIcon(QIcon(":/32x32/greenLEDOn.png").pixmap(25));
    }
    // Between
    else if (state == 2){
        safetyShutter1Button_->setIcon(QIcon(":/32x32/yellowLEDOn.png").pixmap(25));

    }
    // Closed
    else if (state == 4){
        safetyShutter1Button_->setIcon(QIcon(":/32x32/redLEDOn.png").pixmap(25));
    }


}

QLayout * PGMPersistentView::createAndLayoutBeamlingShutters()
{
	PGMBeamline *pgmBeamline = PGMBeamline::pgm();

	// Read only shutter status
	photonShutter1Button_ = new QToolButton;
	photonShutter1Button_->setIcon(QIcon(":/32x32/redLEDOn.png").pixmap(25));
	photonShutter1Button_->setToolTip("Photon Shutter 1");
	connect(pgmBeamline->photonShutter1(), SIGNAL(valueChanged(double)), this, SLOT(onPhotonShutter1ValueChanged(double)));

	// Photon shutter 2
	photonShutter2Button_ = new QToolButton;
	photonShutter2Button_->setIcon(QIcon(":/32x32/redLEDOn.png").pixmap(25));
	photonShutter2Button_->setToolTip("Photon Shutter 2");
	//TODO: usually we won't allow the user the control the shutter directly. we will have the beam on/off action list, which will turn the valves and shutters automatically
	connect(photonShutter2Button_, SIGNAL(clicked()), pgmBeamline, SLOT(togglePhotonShutter2()));
	connect(pgmBeamline->photonShutter2(), SIGNAL(valueChanged(double)), this, SLOT(onPhotonShutter2ValueChanged(double)));


	// Photon shutter 3
	photonShutter3Button_ = new QToolButton;
	photonShutter3Button_->setIcon(QIcon(":/32x32/redLEDOn.png").pixmap(25));
	photonShutter3Button_->setToolTip("Photon Shutter 3");
	//TODO: usually we won't allow the user the control the shutter directly. we will have the beam on/off action list, which will turn the valves and shutters automatically
	connect(photonShutter3Button_, SIGNAL(clicked()), pgmBeamline, SLOT(togglePhotonShutter3()));
	connect(pgmBeamline->photonShutter3(), SIGNAL(valueChanged(double)), this, SLOT(onPhotonShutter3ValueChanged(double)));

	// Safety Shutter 1
	safetyShutter1Button_ = new QToolButton;
	safetyShutter1Button_->setIcon(QIcon(":/32x32/redLEDOn.png").pixmap(25));
	safetyShutter1Button_->setToolTip("Safety Shutter 1");
	//TODO: usually we won't allow the user the control the shutter directly. we will have the beam on/off action list, which will turn the valves and shutters automatically
	connect(safetyShutter1Button_, SIGNAL(clicked()), pgmBeamline, SLOT(toggleSafetyShutter1()));
	connect(pgmBeamline->safetyShutter1(), SIGNAL(valueChanged(double)), this, SLOT(onSafetyShutter1ValueChanged(double)));

	QFont font(this->font());
	font.setBold(true);

	// Shutter Layout
	QLabel *psh1Label = new QLabel("PSH1");
	psh1Label->setFont(font);
	QLabel *psh2Label = new QLabel("PSH2");
	psh2Label->setFont(font);
	QLabel *psh3Label = new QLabel("PSH3");
	psh3Label->setFont(font);
	QLabel *ssh1Label = new QLabel("SSH1");
	ssh1Label->setFont(font);

	QGridLayout *shutterLayout = new QGridLayout;
	shutterLayout->addWidget(psh1Label, 0, 0, 1, 2);
	shutterLayout->addWidget(psh2Label, 0, 2, 1, 2);
	shutterLayout->addWidget(psh3Label, 0, 4, 1, 2);
	shutterLayout->addWidget(ssh1Label, 0, 6, 1, 2);
	shutterLayout->addWidget(photonShutter1Button_, 1, 0, 1, 2);
	shutterLayout->addWidget(photonShutter2Button_, 1, 2, 1, 2);
	shutterLayout->addWidget(photonShutter3Button_, 1, 4, 1, 2);
	shutterLayout->addWidget(safetyShutter1Button_, 1, 6, 1, 2);

	return shutterLayout;
}

void PGMPersistentView::initializationForStartup()
{
	PGMBeamline *pgmBeamline = PGMBeamline::pgm();

	if (pgmBeamline->isConnected()) {
		onPhotonShutter1ValueChanged(pgmBeamline->photonShutter1()->value());
		onPhotonShutter2ValueChanged(pgmBeamline->photonShutter2()->value());
		onPhotonShutter3ValueChanged(pgmBeamline->photonShutter3()->value());
		onSafetyShutter1ValueChanged(pgmBeamline->safetyShutter1()->value());
	}
}

