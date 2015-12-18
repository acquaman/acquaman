#include "BioXASSSRLMonochromatorMaskState.h"
#include "actions3/AMActionSupport.h"

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

AMAction3 BioXASSSRLMonochromatorMaskState::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (int(setpoint) == Closed)
		result = AMActionSupport::buildControlMoveAction(control_, 0);

	return result;
}
