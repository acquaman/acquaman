#include "BioXASBeamStatusBar.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "ui/BioXAS/BioXASBiStateControlButton.h"
#include "ui/BioXAS/BioXASShuttersView.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASBeamStatusBar::BioXASBeamStatusBar(BioXASBeamStatus *beamStatus, QWidget *parent) :
	BioXASButtonEditorBar(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Current settings.

	setBeamStatus(beamStatus);

	refresh();
}

BioXASBeamStatusBar::~BioXASBeamStatusBar()
{

}

void BioXASBeamStatusBar::refresh()
{
	// Clear the buttons and views.

	clearButtons();

	// Iterate through the beam status components, creating
	// views for each.

	if (beamStatus_) {

		foreach (AMControl *component, beamStatus_->components()) {
			if (component) {
				QAbstractButton *button = createControlButton(component);
				QWidget *view = createControlView(component);

				addButton(button, view);
			}
		}
	}
}

void BioXASBeamStatusBar::setBeamStatus(BioXASBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_)
			connect( beamStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

QAbstractButton* BioXASBeamStatusBar::createControlButton(AMControl *control) const
{
	QAbstractButton *button = 0;
	bool controlFound = false;

	BioXASShutters *shutters = qobject_cast<BioXASShutters*>(control);
	if (!controlFound && shutters) {
		button = new BioXASBiStateControlButton(shutters);
		button->setIcon(QIcon(":/shutterIcon2.png"));
		button->setToolTip("Shutters");

		controlFound = true;
	}

	BioXASValves *valves = qobject_cast<BioXASValves*>(control);
	if (!controlFound && valves) {
		button = new BioXASBiStateControlButton(valves);
		button->setIcon(QIcon(":/valveIcon2.png"));
		button->setToolTip("Valves");

		controlFound = true;
	}

	BioXASM1MirrorMaskState *mirrorMask = qobject_cast<BioXASM1MirrorMaskState*>(control);
	if (!controlFound && mirrorMask) {
		button = new BioXASBiStateControlButton(mirrorMask);
		button->setIcon(QIcon(":/mirror-icon1.png"));
		button->setToolTip("Mirror");

		controlFound = true;
	}

	BioXASSSRLMonochromatorMaskState *monoMask = qobject_cast<BioXASSSRLMonochromatorMaskState*>(control);
	if (!controlFound && monoMask) {
		button = new BioXASBiStateControlButton(monoMask);
		button->setIcon(QIcon(":/mono-icon5.png"));
		button->setToolTip("Monochromator Mask");

		controlFound = true;
	}

	return button;
}

QWidget* BioXASBeamStatusBar::createControlView(AMControl *control) const
{
	QWidget *view = 0;
	bool controlFound = false;

	BioXASShutters *shutters = qobject_cast<BioXASShutters*>(control);
	if (!controlFound && shutters) {
		view = new BioXASShuttersView(shutters);
		controlFound = true;
	}

	BioXASValves *valves = qobject_cast<BioXASValves*>(control);
	if (!controlFound && valves) {
		BioXASControlEditor *editor = new BioXASControlEditor(valves);
		editor->setTitle("Valves");

		view = editor;
		controlFound = true;
	}

	BioXASM1MirrorMaskState *mirrorMask = qobject_cast<BioXASM1MirrorMaskState*>(control);
	if (!controlFound && mirrorMask) {
		BioXASControlEditor *editor = new BioXASControlEditor(mirrorMask);
		editor->setTitle("Mirror mask");

		view = editor;
		controlFound = true;
	}

	BioXASSSRLMonochromatorMaskState *monoMask = qobject_cast<BioXASSSRLMonochromatorMaskState*>(control);
	if (!controlFound && monoMask) {
		BioXASControlEditor *editor = new BioXASControlEditor(monoMask);
		editor->setTitle("Mono mask");

		view = editor;
		controlFound = true;
	}

	return view;
}
