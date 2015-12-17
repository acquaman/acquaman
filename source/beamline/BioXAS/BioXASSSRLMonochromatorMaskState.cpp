#include "BioXASSSRLMonochromatorMaskState.h"

BioXASSSRLMonochromatorMaskState::BioXASSSRLMonochromatorMaskState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("SSRL Monochromator Mask State");
	setAllowsMovesWhileMoving(false);

	// Setup value options.

	addValueOption(Open, "Open", 0);
	addValueOption(Closed, "Closed", 1);
}

BioXASSSRLMonochromatorMaskState::~BioXASSSRLMonochromatorMaskState()
{

}

void BioXASSSRLMonochromatorMaskState::setState(AMReadOnlyPVControl *newControl)
{
	if (setBaseControl(newControl))
		emit stateChanged(newControl);
}
