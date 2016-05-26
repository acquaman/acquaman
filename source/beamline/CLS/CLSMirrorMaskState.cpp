#include "CLSMirrorMaskState.h"

#include "actions3/AMActionSupport.h"

CLSMirrorMaskState::CLSMirrorMaskState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("M1 Mirror Mask State");
	setAllowsMovesWhileMoving(false);

	// Setup basic value options.

	addValueOption(Closed, "Closed", 0.0, 0.0, 0.01);
	addValueOption(Open, "Open", 0.01, 10.0);
}

CLSMirrorMaskState::~CLSMirrorMaskState()
{

}

void CLSMirrorMaskState::setUpperSlitBlade(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit upperSlitBladeChanged(newControl);
}

AMAction3* CLSMirrorMaskState::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (int(setpoint) == Closed)
		result = createMoveToClosedAction();

	return result;
}

AMAction3* CLSMirrorMaskState::createMoveToClosedAction()
{
	return AMActionSupport::buildControlMoveAction(control_, 0);
}
