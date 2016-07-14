#include "BioXASMirrorMaskState.h"

#include "actions3/AMActionSupport.h"

BioXASMirrorMaskState::BioXASMirrorMaskState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("M1 Mirror Mask State");
	setAllowsMovesWhileMoving(false);

	// Setup basic value options.

	addValueOption(Closed, "Closed", 0.0, 0.0, 0.01);
	addValueOption(Open, "Open", 0.01, 10.0);
}

BioXASMirrorMaskState::~BioXASMirrorMaskState()
{

}

void BioXASMirrorMaskState::setUpperSlitBlade(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit upperSlitBladeChanged(newControl);
}

AMAction3* BioXASMirrorMaskState::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (int(setpoint) == Closed)
		result = createMoveToClosedAction();

	return result;
}

AMAction3* BioXASMirrorMaskState::createMoveToClosedAction()
{
	return AMActionSupport::buildControlMoveAction(control_, 0);
}
