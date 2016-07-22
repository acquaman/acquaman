#include "BioXASMirrorMaskState.h"

#include "actions3/AMActionSupport.h"

BioXASMirrorMaskState::BioXASMirrorMaskState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("M1 Mirror Mask State");
	setAllowsMovesWhileMoving(false);

	// Setup basic value options.

	addValueOption(Closed, "Closed", BIOXASMIRRORMASKSTATE_UPPER_BLADE_CLOSED_POSITION_SETPOINT, BIOXASMIRRORMASKSTATE_UPPER_BLADE_CLOSED_POSITION_MINIMUM, BIOXASMIRRORMASKSTATE_UPPER_BLADE_CLOSED_POSITION_MAXIMUM);
	addValueOption(Open, "Open", BIOXASMIRRORMASKSTATE_UPPER_BLADE_OPEN_POSITION_MINIMUM, BIOXASMIRRORMASKSTATE_UPPER_BLADE_OPEN_POSITION_MAXIMUM);
}

BioXASMirrorMaskState::~BioXASMirrorMaskState()
{

}

void BioXASMirrorMaskState::setUpperBlade(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit upperBladeChanged(newControl);
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
	return AMActionSupport::buildControlMoveAction(control_, BIOXASMIRRORMASKSTATE_UPPER_BLADE_CLOSED_POSITION_SETPOINT);
}
