#include "BioXASMirrorButton.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"

BioXASMirrorButton::BioXASMirrorButton(AMControl *control, QWidget *parent) :
	AMControlToolButton(control, parent)
{
	// Initialize inherited variables.

	setIcon(QIcon(":/mirror-icon1.png"));
	setToolTip("Mirror");
}

BioXASMirrorButton::~BioXASMirrorButton()
{

}

QColor BioXASMirrorButton::currentColor() const
{
	QColor color = QColor(Qt::yellow);

	BioXASM1MirrorMaskState *state = qobject_cast<BioXASM1MirrorMaskState*>(control_);

	if (state && state->canMeasure()) {
		if (state->value() == BioXASM1MirrorMaskState::Open)
			color = QColor(Qt::green);
		else
			color = QColor(Qt::red);
	}

	return color;
}
