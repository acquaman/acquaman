#include "BioXASValves.h"
#include "beamline/BioXAS/BioXASBiStateGroup.h"

BioXASValves::BioXASValves(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed", true);
}

BioXASValves::~BioXASValves()
{

}

bool BioXASValves::isOpen() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState1())
		result = true;

	return result;
}

bool BioXASValves::isClosed() const
{
	bool result = false;

	if (isConnected() && areAnyChildrenState2())
		result = true;

	return result;
}

void BioXASValves::addValve(AMControl *newValve, double openValue, double closedValue)
{
	if (addBiStateControl(newValve, openValue, closedValue))
		emit valvesChanged();
}

void BioXASValves::removeValve(AMControl *valve)
{
	if (removeBiStateControl(valve))
		emit valvesChanged();
}

void BioXASValves::clearValves()
{
	if (clearBiStateControls())
		emit valvesChanged();
}

AMAction3* BioXASValves::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (setpoint == Open)
		result = createMoveToOpenAction();

	// Closing valves is not supported at this time.
	// This is because a user accidentally closing valves
	// could trigger machine protection protocols.

	return result;
}

AMAction3* BioXASValves::createMoveToOpenAction()
{
	AMAction3 *action = createMoveChildrenToState1Action();

	if (action) {
		action->info()->setShortDescription("Opening valves.");
		action->info()->setLongDescription("Opening valves.");
	}

	return action;
}

int BioXASValves::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOpen())
		result = Open;
	else if (isClosed())
		result = Closed;

	return result;
}
