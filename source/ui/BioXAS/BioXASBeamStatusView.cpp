#include "BioXASBeamStatusView.h"

BioXASBeamStatusView::BioXASBeamStatusView(BioXASBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	beamStatus_ = 0;

	// Create UI elements.

	wigglerButton_ = new AMControlToolButton(0);

	shutterButton_ = new AMControlToolButton(0);

	valvesButton_ = new AMControlToolButton(0);

	mirrorMaskButton_ = new AMControlToolButton(0);

	monoMaskButton_ = new AMControlToolButton(0);

	// Create and set main layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(wigglerButton_);
	layout->addWidget(shutterButton_);
	layout->addWidget(valvesButton_);
	layout->addWidget(mirrorMaskButton_);
	layout->addWidget(monoMaskButton_);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
}

BioXASBeamStatusView::~BioXASBeamStatusView()
{

}

void BioXASBeamStatusView::setBeamStatus(BioXASBeamStatus *newBeamStatus)
{
	if (beamStatus_ != newBeamStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newBeamStatus;

		if (beamStatus_)
			connect( beamStatus_, SIGNAL(componentsChanged()), this, SLOT(updateBeamStatusView()) );

		emit beamStatusChanged(beamStatus_);
	}

	updateBeamStatusView();
}

void BioXASBeamStatusView::updateBeamStatusView()
{
	updateWigglerButton();
	updateShutterButton();
	updateValvesButton();
	updateMirrorMaskButton();
	updateMonoMaskButton();
}

void BioXASBeamStatusView::updateWigglerButton()
{

}

void BioXASBeamStatusView::updateShutterButton()
{

}

void BioXASBeamStatusView::updateValvesButton()
{

}

void BioXASBeamStatusView::updateMirrorMaskButton()
{

}

void BioXASBeamStatusView::updateMonoMaskButton()
{

}
