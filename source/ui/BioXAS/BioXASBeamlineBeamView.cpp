#include "BioXASBeamlineBeamView.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASBeamlineBeamView::BioXASBeamlineBeamView(QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

	QLabel *beamStatusPrompt = new QLabel("Beam status:");

	beamStatus_ = new QLabel();

	beamOnButton_ = new QPushButton("Beam On");

	beamOffButton_ = new QPushButton("Beam Off");

	// Create and set layouts.

	QHBoxLayout *statusLayout = new QHBoxLayout();
	statusLayout->addWidget(beamStatusPrompt);
	statusLayout->addWidget(beamStatus_);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(beamOnButton_);
	buttonLayout->addWidget(beamOffButton_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(statusLayout);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	// Make connections.

//	connect( BioXASBeamline::bioXAS(), SIGNAL(beamStatusChanged()), this, SLOT(updateBeamStatus()) );

	// Current settings.

	updateBeamStatus();
}

BioXASBeamlineBeamView::~BioXASBeamlineBeamView()
{

}

void BioXASBeamlineBeamView::updateBeamStatus()
{
	bool beamOff = BioXASBeamline::bioXAS()->beamOff();
	bool beamOn = BioXASBeamline::bioXAS()->beamOn();

	if (beamOff && !beamOn) {
		showBeamOff();
	} else if (!beamOff && beamOn) {
		showBeamOn();
	} else {
		showBeamBetween();
	}
}

void BioXASBeamlineBeamView::showBeamOff()
{
	beamStatus_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

	beamOnButton_->setEnabled(true);
	beamOffButton_->setEnabled(false);
}

void BioXASBeamlineBeamView::showBeamOn()
{
	beamStatus_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));

	beamOnButton_->setEnabled(false);
	beamOffButton_->setEnabled(true);
}

void BioXASBeamlineBeamView::showBeamBetween()
{
	beamStatus_->setPixmap(QPixmap(":/22x22/yellowLEDOn.png"));

	beamOnButton_->setEnabled(true);
	beamOffButton_->setEnabled(true);
}
