#include "BioXASShuttersGroup.h"
#include "beamline/AMControl.h"

BioXASShuttersGroup::BioXASShuttersGroup(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed", true);
}

BioXASShuttersGroup::~BioXASShuttersGroup()
{

}

bool BioXASShuttersGroup::isOpen() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState1())
		result = true;

	return result;
}

bool BioXASShuttersGroup::isClosed() const
{
	bool result = false;

	if (isConnected() && areAnyChildrenState2())
		result = true;

	return result;
}

void BioXASShuttersGroup::addShutter(AMControl *newShutter, double openValue, double closedValue)
{
	if (addBiStateControl(newShutter, openValue, closedValue))
		emit shuttersChanged();
}

void BioXASShuttersGroup::removeShutter(AMControl *shutter)
{
	if (removeBiStateControl(shutter))
		emit shuttersChanged();
}

void BioXASShuttersGroup::clearShutters()
{
	if (clearBiStateControls())
		emit shuttersChanged();
}

AMAction3* BioXASShuttersGroup::createMoveAction(double setpoint)
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

AMAction3* BioXASShuttersGroup::createMoveToOpenAction()
{
	AMAction3 *action = createMoveChildrenToState1Action();

	if (action) {
		action->info()->setShortDescription("Opening shutters.");
		action->info()->setLongDescription("Opening shutters.");
	}

	return action;
}

AMAction3* BioXASShuttersGroup::createMoveToClosedAction()
{
	AMAction3 *action = createMoveChildrenToState2Action();

	if (action) {
		action->info()->setShortDescription("Closing shutters.");
		action->info()->setLongDescription("Closing shutters.");
	}

	return action;
}

AMAction3* BioXASShuttersGroup::createMoveChildToOpen(AMControl *child)
{
	return createMoveChildToState1Action(child);
}

AMAction3* BioXASShuttersGroup::createMoveChildToClosed(AMControl *child)
{
	return createMoveChildToState2Action(child);
}

AMAction3* BioXASShuttersGroup::createCheckChildIsOpen(AMControl *child, double timeoutSec)
{
	return createCheckChildAtState1Action(child, timeoutSec);
}

AMAction3* BioXASShuttersGroup::createCheckChildIsClosed(AMControl *child, double timeoutSec)
{
	return createCheckChildAtState2Action(child, timeoutSec);
}

int BioXASShuttersGroup::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOpen())
		result = Open;
	else if (isClosed())
		result = Closed;

	return result;
}

