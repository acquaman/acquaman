#include "BioXASBeamStatusBar.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
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

	frontEndShuttersEditor_ = new BioXASControlEditor(0);
	frontEndShuttersEditor_->setTitle("Front-end shutters");

	endstationShutterEditor_ = new BioXASControlEditor(0);
	endstationShutterEditor_->setTitle("Endstation shutter");

	QVBoxLayout *shuttersViewLayout = new QVBoxLayout();
	shuttersViewLayout->setMargin(0);
	shuttersViewLayout->addWidget(frontEndShuttersEditor_);
	shuttersViewLayout->addWidget(endstationShutterEditor_);

	QWidget *shuttersView = new QWidget();
	shuttersView->setLayout(shuttersViewLayout);

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

	addButton(shuttersButton_, shuttersView);
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
			connect( beamStatus_, SIGNAL(valvesChanged(BioXASMasterValves*)), this, SLOT(updateValvesViews()) );
			connect( beamStatus_, SIGNAL(mirrorMaskStateChanged(BioXASM1MirrorMaskState*)), this, SLOT(updateMirrorViews()) );
			connect( beamStatus_, SIGNAL(monoMaskStateChanged(BioXASSSRLMonochromatorMaskState*)), this, SLOT(updateMonoViews()) );

			if (beamStatus_->shutters())
				connect( beamStatus_->shutters(), SIGNAL(shuttersChanged()), this, SLOT(updateShuttersViews()) );
		}

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASBeamStatusBar::updateShuttersViews()
{
	// Update shutters button.

	BioXASShutters *shutters = 0;

	if (beamStatus_)
		shutters = beamStatus_->shutters();

	shuttersButton_->setControl(shutters);

	// Update the front-end shutters editor.

	AMControl *frontEndShutters = 0;

	if (shutters)
		frontEndShutters = shutters->frontEndShutters();

	frontEndShuttersEditor_->setControl(frontEndShutters);

	// Update the endstation shutters editor.

	AMControl *endstationShutter = 0;

	if (shutters)
		endstationShutter = shutters->endstationShutter();

	endstationShutterEditor_->setControl(endstationShutter);
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
