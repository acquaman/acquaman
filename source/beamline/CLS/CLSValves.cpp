#include "CLSValves.h"
#include "util/AMErrorMonitor.h"

CLSValves::CLSValves(const QString &name, QObject *parent) :
	CLSTriStateGroup(name, parent)
{
	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed", true);
}

CLSValves::~CLSValves()
{

}

bool CLSValves::isOpen() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState1())
		result = true;

	return result;
}

bool CLSValves::isClosed() const
{
	bool result = false;

	if (isConnected() && areAnyChildrenState2())
		result = true;

	return result;
}

bool CLSValves::hasValve(AMControl *control) const
{
	return hasChildControl(control);
}

QList<AMControl*> CLSValves::openValvesList() const
{
	return childrenInState1();
}

QList<AMControl*> CLSValves::closedValvesList() const
{
	return childrenInState2();
}

bool CLSValves::addValve(AMControl *newValve, double openValue, double closedValue)
{
	bool result = addTriStateControl(newValve, openValue, closedValue);

	if (result)
		emit valvesChanged();

	return result;
}

bool CLSValves::removeValve(AMControl *valve)
{
	bool result = removeTriStateControl(valve);

	if (result)
		emit valvesChanged();

	return result;
}

bool CLSValves::clearValves()
{
	bool result = clearTriStateControls();

	if (result)
		emit valvesChanged();

	return result;
}

AMAction3* CLSValves::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (setpoint == Open)
		result = createMoveToOpenAction();

	// Closing valves is not supported at this time.
	// This is because a user accidentally closing valves
	// could trigger machine protection protocols.

	return result;
}

AMAction3* CLSValves::createMoveToOpenAction()
{
	AMAction3 *action = createMoveChildrenToState1Action();

	if (action) {
		action->info()->setShortDescription("Opening valves.");
		action->info()->setLongDescription("Opening valves.");
	}

	return action;
}

int CLSValves::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOpen())
		result = Open;
	else if (isClosed())
		result = Closed;

	return result;
}
