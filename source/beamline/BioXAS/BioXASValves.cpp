#include "BioXASValves.h"
#include "beamline/BioXAS/BioXASBiStateGroup.h"

BioXASValves::BioXASValves(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed");
}

BioXASValves::~BioXASValves()
{

}

bool BioXASValves::isOpen() const
{
	return areChildrenState1();
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
	int result = indicesNamed("Unknown").first();

	if (isOpen())
		result = Open;
	else
		result = Closed;

	return result;
}
