#include "BioXASBeamStatusButtonBar.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASBeamStatusButtonBar::BioXASBeamStatusButtonBar(BioXASBeamStatus *beamStatus, QWidget *parent) :
	BioXASControlButtonBar(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Current settings.

	setBeamStatus(beamStatus);
}

BioXASBeamStatusButtonBar::~BioXASBeamStatusButtonBar()
{

}

void BioXASBeamStatusButtonBar::refresh()
{
	// Clear the controls.

	clearControls();

	// Add control for each component in the beam status.

	if (beamStatus_) {

		foreach (AMControl *control, beamStatus_->components())
			addControl(control, beamStatus_->componentBeamOnValue(control));
	}
}

void BioXASBeamStatusButtonBar::setBeamStatus(BioXASBeamStatus *newControl)
{
	if (beamStatus_ != newControl) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newControl;

		if (beamStatus_)
			connect( beamStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

QAbstractButton* BioXASBeamStatusButtonBar::createButton(AMControl *control, double greenValue)
{
	QAbstractButton *button = BioXASControlButtonBar::createButton(control, greenValue);

	if (button) {

		bool controlFound = false;

		BioXASShutters *shutters = qobject_cast<BioXASShutters*>(control);
		if (!controlFound && shutters) {
			button->setIcon(QIcon(":/shutterIcon2.png"));
			button->setToolTip("Shutters");

			controlFound = true;
		}

		BioXASValves *valves = qobject_cast<BioXASValves*>(control);
		if (!controlFound && valves) {
			button->setIcon(QIcon(":/valveIcon2.png"));
			button->setToolTip("Valves");

			controlFound = true;
		}

		BioXASM1MirrorMaskState *mirrorMask = qobject_cast<BioXASM1MirrorMaskState*>(control);
		if (!controlFound && mirrorMask) {
			button->setIcon(QIcon(":/mirror-icon1.png"));
			button->setToolTip("Mirror Mask");

			controlFound = true;
		}

		BioXASSSRLMonochromatorMaskState *monoMask = qobject_cast<BioXASSSRLMonochromatorMaskState*>(control);
		if (!controlFound && monoMask) {
			button->setIcon(QIcon(":/mono-icon5.png"));
			button->setToolTip("Monochromator Mask");

			controlFound = true;
		}
	}

	return button;
}
