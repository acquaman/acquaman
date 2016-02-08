#include "BioXASMonochromatorButton.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASMonochromatorButton::BioXASMonochromatorButton(AMControl *control, QWidget *parent) :
	AMControlToolButton(control, parent)
{
	// Initialize inherited variables.

	setIcon(QIcon(":/mono-icon5.png"));
	setToolTip("Monochromator Mask");
}

BioXASMonochromatorButton::~BioXASMonochromatorButton()
{

}

QColor BioXASMonochromatorButton::currentColor() const
{
	QColor color = QColor(Qt::yellow);

	BioXASSSRLMonochromatorMaskState *state = qobject_cast<BioXASSSRLMonochromatorMaskState*>(control_);

	if (state && state->canMeasure()) {
		if (state->value() == BioXASSSRLMonochromatorMaskState::Open)
			color = QColor(Qt::green);
		else
			color = QColor(Qt::red);
	}

	return color;
}
