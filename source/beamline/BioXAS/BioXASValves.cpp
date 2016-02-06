#include "BioXASValves.h"
#include "util/AMErrorMonitor.h"

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

bool BioXASValves::hasValve(AMControl *control) const
{
	return hasChildControl(control);
}

QList<AMControl*> BioXASValves::openValvesList() const
{
	return childrenInState1();
}

QList<AMControl*> BioXASValves::closedValvesList() const
{
	return childrenInState2();
}

bool BioXASValves::addValve(AMControl *newValve, double openValue, double closedValue)
{
	bool result = addBiStateControl(newValve, openValue, closedValue);

	if (result)
		emit valvesChanged();

	return result;
}

bool BioXASValves::removeValve(AMControl *valve)
{
	bool result = removeBiStateControl(valve);

	if (result)
		emit valvesChanged();

	return result;
}

bool BioXASValves::clearValves()
{
	bool result = clearBiStateControls();

	if (result)
		emit valvesChanged();

	return result;
}

void BioXASValves::updateValue()
{
	BioXASBiStateGroup::updateValue();

	// Display AMErrorMon if any child controls
	// are in a Closed state.

	QList<AMControl*> closedValves = closedValvesList();

	if (!closedValves.isEmpty()) {
		QString error("The following valves are closed:\n");

		foreach (AMControl *control, closedValves) {
			if (control)
				error += tr("%1\n").arg(control->name());
		}

		AMErrorMon::error(this, BIOXASVALVES_CLOSED_STATE, error);
	}
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
