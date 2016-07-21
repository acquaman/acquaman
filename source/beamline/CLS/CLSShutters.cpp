#include "CLSShutters.h"

#include "beamline/AMControl.h"

CLSShutters::CLSShutters(const QString &name, QObject *parent) :
	CLSTriStateGroup(name, parent)
{
	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed", true);
}

CLSShutters::~CLSShutters()
{
}

bool CLSShutters::isOpen() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState1())
		result = true;

	return result;
}

bool CLSShutters::isClosed() const
{
	bool result = false;

	if (isConnected() && areAnyChildrenState2())
		result = true;

	return result;
}

bool CLSShutters::hasShutter(AMControl *control) const
{
	return hasChildControl(control);
}

bool CLSShutters::addShutter(AMControl *newShutter, double openStateValue, double closedStateValue)
{
	bool result = addTriStateControl(newShutter, openStateValue, closedStateValue);

	if (result) {
		emit shuttersChanged();
	}

	return result;
}

bool CLSShutters::removeShutter(AMControl *shutter)
{
	bool result = removeTriStateControl(shutter);

	if (result) {
		emit shuttersChanged();
	}

	return result;
}

bool CLSShutters::clearShutters()
{
	bool result = clearTriStateControls();

	if (result) {
		emit shuttersChanged();
	}

	return result;
}

AMAction3* CLSShutters::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	switch (int(setpoint)) {
	case Open:
		result = createMoveToOpenAction();
		break;
	case Closed:
		result = createMoveToClosedAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* CLSShutters::createMoveToOpenAction()
{
	AMAction3 *action = createMoveChildrenToState1Action();

	if (action) {
		action->info()->setShortDescription("Opening shutters.");
		action->info()->setLongDescription("Opening shutters.");
	}

	return action;
}

AMAction3* CLSShutters::createMoveToClosedAction()
{
	AMAction3 *action = createMoveChildrenToState2Action();

	if (action) {
		action->info()->setShortDescription("Closing shutters.");
		action->info()->setLongDescription("Closing shutters.");
	}

	return action;
}

AMAction3* CLSShutters::createMoveChildToOpenAction(AMControl *child)
{
	return createMoveChildToState1Action(child);
}

AMAction3* CLSShutters::createWaitChildMoveToOpenAction(AMControl *child)
{
	return createWaitChildToMoveState1Action(child);
}

AMAction3* CLSShutters::createMoveChildToClosedAction(AMControl *child)
{
	return createMoveChildToState2Action(child);
}

AMAction3* CLSShutters::createWaitChildMoveToClosedAction(AMControl *child)
{
	return createWaitChildMoveToState2Action(child);
}

AMAction3* CLSShutters::createCheckChildIsOpenAction(AMControl *child, double timeoutSec)
{
	return createCheckChildAtState1Action(child, timeoutSec);
}

AMAction3* CLSShutters::createCheckChildIsClosedAction(AMControl *child, double timeoutSec)
{
	return createCheckChildAtState2Action(child, timeoutSec);
}

int CLSShutters::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOpen())
		result = Open;
	else if (isClosed())
		result = Closed;

	return result;
}
