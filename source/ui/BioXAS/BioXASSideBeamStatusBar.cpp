#include "BioXASSideBeamStatusBar.h"
#include "beamline/BioXAS/BioXASSideBeamStatus.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASMasterValves.h"
#include "ui/beamline/AMControlToolButton.h"
#include "ui/BioXAS/BioXASShuttersButton.h"
#include "ui/BioXAS/BioXASValvesButton.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASSideBeamStatusBar::BioXASSideBeamStatusBar(BioXASSideBeamStatus *beamStatus, QWidget *parent) :
    BioXASBeamlineStatusBar(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	frontEndShuttersButton_ = new BioXASShuttersButton(0);
	frontEndShuttersButton_->setToolTip("Front-end shutters");

	frontEndShuttersEditor_ = new BioXASControlEditor(0);
	frontEndShuttersEditor_->setTitle("Front-end shutters");

	valvesButton_ = new BioXASValvesButton(0);
	valvesButton_->setToolTip("Valves");

	valvesEditor_ = new BioXASControlEditor(0);
	valvesEditor_->setTitle("Valves");

	// Add views.

	addButton(frontEndShuttersButton_, frontEndShuttersEditor_);
	addButton(valvesButton_, valvesEditor_);

	// Current settings.

	setBeamStatus(beamStatus);

	refresh();
}

BioXASSideBeamStatusBar::~BioXASSideBeamStatusBar()
{

}

void BioXASSideBeamStatusBar::refresh()
{
	updateFrontEndShuttersViews();
	updateValvesViews();
}

void BioXASSideBeamStatusBar::setBeamStatus(BioXASSideBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_) {
			connect( beamStatus_, SIGNAL(frontEndShuttersChanged(BioXASFrontEndShutters*)), this, SLOT(updateFrontEndShuttersViews()) );
			connect( beamStatus_, SIGNAL(valvesChanged(BioXASMasterValves*)), this, SLOT(updateValvesViews()) );
		}

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASSideBeamStatusBar::updateFrontEndShuttersViews()
{
	BioXASShutters *shutters = 0;

	if (beamStatus_)
		shutters = beamStatus_->frontEndShutters();

	frontEndShuttersButton_->setControl(shutters);
	frontEndShuttersEditor_->setControl(shutters);
}

void BioXASSideBeamStatusBar::updateValvesViews()
{
	BioXASValves *valves = 0;

	if (beamStatus_)
		valves = beamStatus_->valves();

	valvesButton_->setControl(valves);
	valvesEditor_->setControl(valves);
}
