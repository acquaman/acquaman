#include "BioXASBeamStatusButtonBar.h"

#include "beamline/CLS/CLSBeamlineStatus.h"

#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASBeamStatusButtonBar::BioXASBeamStatusButtonBar(CLSBeamlineStatus *beamlineStatus, QWidget *parent) :
	BioXASControlButtonBar(parent)
{
	// Initialize class variables.

	beamlineStatus_ = 0;

	// Current settings.

	setBeamlineStatus(beamlineStatus);
}

BioXASBeamStatusButtonBar::~BioXASBeamStatusButtonBar()
{

}

void BioXASBeamStatusButtonBar::refresh()
{
	// Clear the controls.

	clearControls();

	// Add control for each component in the beam status.

	if (beamlineStatus_) {

		foreach (AMControl *control, beamlineStatus_->components())
			addControl(control, beamlineStatus_->componentBeamOnValue(control));
	}
}

void BioXASBeamStatusButtonBar::setBeamlineStatus(CLSBeamlineStatus *newControl)
{
	if (beamlineStatus_ != newControl) {

		if (beamlineStatus_)
			disconnect( beamlineStatus_, 0, this, 0 );

		beamlineStatus_ = newControl;

		if (beamlineStatus_)
			connect( beamlineStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );

		refresh();

		emit beamStatusChanged(beamlineStatus_);
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
