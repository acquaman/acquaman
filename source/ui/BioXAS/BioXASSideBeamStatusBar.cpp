#include "BioXASSideBeamStatusBar.h"
#include "beamline/BioXAS/BioXASSideBeamStatus.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASMasterValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "ui/beamline/AMControlToolButton.h"
#include "ui/BioXAS/BioXASShuttersButton.h"
#include "ui/BioXAS/BioXASValvesButton.h"
#include "ui/BioXAS/BioXASMonochromatorButton.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASShuttersView.h"

BioXASSideBeamStatusBar::BioXASSideBeamStatusBar(BioXASSideBeamStatus *beamStatus, QWidget *parent) :
    BioXASBeamlineStatusBar(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	shuttersButton_ = new BioXASShuttersButton(0);
	shuttersButton_->setToolTip("Front-end shutters");

	shuttersView_ = new BioXASShuttersView(0);

	valvesButton_ = new BioXASValvesButton(0);
	valvesButton_->setToolTip("Valves");

	valvesEditor_ = new BioXASControlEditor(0);
	valvesEditor_->setTitle("Valves");

	mirrorButton_ = new AMControlToolButton(0);
	mirrorButton_->setToolTip("M1 mirror mask");

	mirrorEditor_ = new BioXASControlEditor(0);
	mirrorEditor_->setTitle("M1 mirror mask");

	monoButton_ = new BioXASMonochromatorButton(0);
	monoButton_->setToolTip("Monochromator mask");

	monoEditor_ = new BioXASControlEditor(0);
	monoEditor_->setTitle("Monochromator mask");

	// Add views.

	addButton(shuttersButton_, shuttersView_);
	addButton(valvesButton_, valvesEditor_);
	addButton(mirrorButton_, mirrorEditor_);
	addButton(monoButton_, monoEditor_);

	// Current settings.

	setBeamStatus(beamStatus);

	refresh();
}

BioXASSideBeamStatusBar::~BioXASSideBeamStatusBar()
{

}

void BioXASSideBeamStatusBar::refresh()
{
	updateShuttersViews();
	updateValvesViews();
	updateMirrorViews();
	updateMonoViews();
}

void BioXASSideBeamStatusBar::setBeamStatus(BioXASSideBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_) {
			connect( beamStatus_, SIGNAL(shuttersChanged(BioXASShutters*)), this, SLOT(updateShuttersViews()) );
			connect( beamStatus_, SIGNAL(valvesChanged(BioXASMasterValves*)), this, SLOT(updateValvesViews()) );
			connect( beamStatus_, SIGNAL(mirrorMaskStateChanged(BioXASM1MirrorMaskState*)), this, SLOT(updateMirrorViews()) );
			connect( beamStatus_, SIGNAL(monoMaskStateChanged(BioXASSSRLMonochromatorMaskState*)), this, SLOT(updateMonoViews()) );
		}

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASSideBeamStatusBar::updateShuttersViews()
{
	BioXASShutters *shutters = 0;

	if (beamStatus_)
		shutters = beamStatus_->shutters();

	shuttersButton_->setControl(shutters);
	shuttersView_->setShutters(shutters);
}

void BioXASSideBeamStatusBar::updateValvesViews()
{
	BioXASValves *valves = 0;

	if (beamStatus_)
		valves = beamStatus_->valves();

	valvesButton_->setControl(valves);
	valvesEditor_->setControl(valves);
}

void BioXASSideBeamStatusBar::updateMirrorViews()
{
	AMControl *maskState = 0;

	if (beamStatus_)
		maskState = beamStatus_->mirrorMaskState();

	mirrorButton_->setControl(maskState);
	mirrorEditor_->setControl(maskState);
}

void BioXASSideBeamStatusBar::updateMonoViews()
{
	AMControl *maskState = 0;

	if (beamStatus_)
		maskState = beamStatus_->monoMaskState();

	monoButton_->setControl(maskState);
	monoEditor_->setControl(maskState);
}
