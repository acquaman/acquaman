#include "BioXASShutters.h"

BioXASShutters::BioXASShutters(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed");
}

BioXASShutters::~BioXASShutters()
{

}

bool BioXASShutters::isOpen() const
{
	return areChildrenState1();
}

void BioXASShutters::addShutter(AMControl *newShutter, double openValue, double closedValue)
{
	if (addBiStateControl(newShutter, openValue, closedValue))
		emit shuttersChanged();
}

void BioXASShutters::removeShutter(AMControl *shutter)
{
	if (removeBiStateControl(shutter))
		emit shuttersChanged();
}

void BioXASShutters::clearShutters()
{
	if (clearBiStateControls())
		emit shuttersChanged();
}

AMAction3* BioXASShutters::createMoveAction(double setpoint)
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

AMAction3* BioXASShutters::createMoveToOpenAction()
{
	AMAction3 *action = createMoveChildrenToState1Action();

	if (action) {
		action->info()->setShortDescription("Opening shutters.");
		action->info()->setLongDescription("Opening shutters.");
	}

	return action;
}

AMAction3* BioXASShutters::createMoveToClosedAction()
{
	AMAction3 *action = createMoveChildrenToState2Action();

	if (action) {
		action->info()->setShortDescription("Closing shutters.");
		action->info()->setLongDescription("Closing shutters.");
	}

	return action;
}

AMAction3* BioXASShutters::createMoveChildToOpen(AMControl *child)
{
	return createMoveChildToState1Action(child);
}

AMAction3* BioXASShutters::createMoveChildToClosed(AMControl *child)
{
	return createMoveChildToState2Action(child);
}

AMAction3* BioXASShutters::createCheckChildIsOpen(AMControl *child, double timeoutSec)
{
	return createCheckChildAtState1Action(child, timeoutSec);
}

AMAction3* BioXASShutters::createCheckChildIsClosed(AMControl *child, double timeoutSec)
{
	return createCheckChildAtState2Action(child, timeoutSec);
}

int BioXASShutters::currentIndex() const
{
	int result = indicesNamed("Unknown").first();

	if (isOpen())
		result = Open;
	else
		result = Closed;

	return result;
}

