#include "BioXASBeamStatusBar.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASMasterValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"
#include "ui/beamline/AMControlToolButton.h"
#include "ui/BioXAS/BioXASShuttersButton.h"
#include "ui/BioXAS/BioXASValvesButton.h"
#include "ui/BioXAS/BioXASMonochromatorButton.h"
#include "ui/BioXAS/BioXASMirrorButton.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASBeamStatusBar::BioXASBeamStatusBar(BioXASBeamStatus *beamStatus, QWidget *parent) :
	BioXASButtonEditorBar(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	shuttersButton_ = new BioXASShuttersButton(0);
	shuttersButton_->setToolTip("Shutters");

	shuttersEditor_ = new BioXASControlEditor(0);
	shuttersEditor_->setTitle("Shutters");

	valvesButton_ = new BioXASValvesButton(0);
	valvesButton_->setToolTip("Valves");

	valvesEditor_ = new BioXASControlEditor(0);
	valvesEditor_->setTitle("Valves");

	mirrorButton_ = new BioXASMirrorButton(0);
	mirrorButton_->setToolTip("M1 mirror mask");

	mirrorEditor_ = new BioXASControlEditor(0);
	mirrorEditor_->setTitle("M1 mirror mask");

	monoButton_ = new BioXASMonochromatorButton(0);
	monoButton_->setToolTip("Monochromator mask");

	monoEditor_ = new BioXASControlEditor(0);
	monoEditor_->setTitle("Monochromator mask");

	// Add views.

	addButton(shuttersButton_, shuttersEditor_);
	addButton(valvesButton_, valvesEditor_);
	addButton(mirrorButton_, mirrorEditor_);
	addButton(monoButton_, monoEditor_);

	// Current settings.

	setBeamStatus(beamStatus);

	refresh();
}

BioXASBeamStatusBar::~BioXASBeamStatusBar()
{

}

void BioXASBeamStatusBar::refresh()
{
	updateShuttersViews();
	updateValvesViews();
	updateMirrorViews();
	updateMonoViews();
}

void BioXASBeamStatusBar::setBeamStatus(BioXASBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_) {
			disconnect( beamStatus_, 0, this, 0 );

			if (beamStatus_->shutters())
				disconnect( beamStatus_->shutters(), 0, this, 0 );
		}

		beamStatus_ = newStatus;

		if (beamStatus_) {
			connect( beamStatus_, SIGNAL(shuttersChanged(BioXASShutters*)), this, SLOT(updateShuttersViews()) );
			connect( beamStatus_, SIGNAL(valvesChanged(BioXASValves*)), this, SLOT(updateValvesViews()) );
			connect( beamStatus_, SIGNAL(mirrorMaskStateChanged(BioXASM1MirrorMaskState*)), this, SLOT(updateMirrorViews()) );
			connect( beamStatus_, SIGNAL(monoMaskStateChanged(BioXASSSRLMonochromatorMaskState*)), this, SLOT(updateMonoViews()) );
		}

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASBeamStatusBar::updateShuttersViews()
{
	BioXASShutters *shutters = 0;

	if (beamStatus_)
		shutters = beamStatus_->shutters();

	shuttersButton_->setControl(shutters);
	shuttersEditor_->setControl(shutters);
}

void BioXASBeamStatusBar::updateValvesViews()
{
	BioXASValves *valves = 0;

	if (beamStatus_)
		valves = beamStatus_->valves();

	valvesButton_->setControl(valves);
	valvesEditor_->setControl(valves);
}

void BioXASBeamStatusBar::updateMirrorViews()
{
	AMControl *maskState = 0;

	if (beamStatus_)
		maskState = beamStatus_->mirrorMaskState();

	mirrorButton_->setControl(maskState);
	mirrorEditor_->setControl(maskState);
}

void BioXASBeamStatusBar::updateMonoViews()
{
	AMControl *maskState = 0;

	if (beamStatus_)
		maskState = beamStatus_->monoMaskState();

	monoButton_->setControl(maskState);
	monoEditor_->setControl(maskState);
}
