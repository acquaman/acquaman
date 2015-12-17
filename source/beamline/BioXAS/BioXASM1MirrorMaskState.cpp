#include "BioXASM1MirrorMaskState.h"

BioXASM1MirrorMaskState::BioXASM1MirrorMaskState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("M1 Mirror Mask State");
	setAllowsMovesWhileMoving(false);

	// Setup basic value options.

	addValueOption(Open, "Open", 3.2, 0.1, 10); // these are 'pretend' values, for now. should be safe to use them, as this control does not support moving.
	addValueOption(Closed, "Closed", 0);
}

BioXASM1MirrorMaskState::~BioXASM1MirrorMaskState()
{

}

void BioXASM1MirrorMaskState::setUpperSlitBlade(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit upperSlitBladeChanged(newControl);
}
