#include "BioXASM1MirrorMaskState.h"
#include "actions3/AMActionSupport.h"

BioXASM1MirrorMaskState::BioXASM1MirrorMaskState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("M1 Mirror Mask State");
	setAllowsMovesWhileMoving(false);

	// Setup basic value options.

	addValueOption(Closed, "Closed", 0.0, 0.0, 0.01, false);
	addValueOption(Open, "Open", 0.01, 10.0);
}

BioXASM1MirrorMaskState::~BioXASM1MirrorMaskState()
{

}

void BioXASM1MirrorMaskState::setUpperSlitBlade(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit upperSlitBladeChanged(newControl);
}

AMAction3* BioXASM1MirrorMaskState::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (int(setpoint) == Closed)
		result = createMoveToClosedAction();

	return result;
}

AMAction3* BioXASM1MirrorMaskState::createMoveToClosedAction()
{
	return AMActionSupport::buildControlMoveAction(control_, 0);
}
