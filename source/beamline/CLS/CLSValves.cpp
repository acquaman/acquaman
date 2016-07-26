#include "CLSValves.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
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
	valvesBeamOnOrderMap_.clear();
}

AMListAction3* CLSValves::createBeamOnActionList()
{
	// create the action list to move the valves (sequentially) and wait for the move done
	AMListAction3 *openValvesActionList = new AMListAction3(new AMListActionInfo3("Open valves action list", "Open Valves"), AMListAction3::Sequential);
	AMListAction3 *waitValvesOpenActionList = new AMListAction3(new AMListActionInfo3("Wait valves Open action list", "Wait Valves Open"), AMListAction3::Parallel);

	// this is to make sure all the controls are checked
	int currentBeamOnOrder = 1;
	int checkedControlCount = 0;
	while (checkedControlCount < valvesBeamOnOrderMap_.count()) {
		AMControl *valveControl = valvesBeamOnOrderMap_.value(currentBeamOnOrder);
		if (valveControl) {
			if (isChildState2(valveControl)) { // the valve is closed
				AMAction3 *openValveAction = AMActionSupport::buildControlMoveAction(valveControl, 1);
				openValvesActionList->addSubAction(openValveAction);

				AMAction3 *openValveWaitAction = AMActionSupport::buildControlWaitAction(valveControl, 1);
				waitValvesOpenActionList->addSubAction(openValveWaitAction);
			}

			checkedControlCount ++;
		}

		currentBeamOnOrder++;
	}

	// add the open/wait action lists to the beam on action list
	AMListAction3 *openValvesActionsList = 0;
	if (openValvesActionList->subActionCount() > 0) {
		openValvesActionsList = new AMListAction3(new AMListActionInfo3("Beam On - Valves", "Beam On: open valves"), AMListAction3::Parallel);
		openValvesActionsList->addSubAction(openValvesActionList);
		openValvesActionsList->addSubAction(waitValvesOpenActionList);
	} else {
		openValvesActionList->deleteLater();
		waitValvesOpenActionList->deleteLater();
	}

	return openValvesActionsList;
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

bool CLSValves::addValve(AMControl *newValve, double openStateValue, double closedStateValue, int beamOnOrder)
{
	bool result = addTriStateControl(newValve, openStateValue, closedStateValue);

	if (result) {
		emit valvesChanged();

		if (beamOnOrder > 0) {
			AMControl * control = valvesBeamOnOrderMap_.value(beamOnOrder);
			if (control) {
				AMErrorMon::alert(this, CLSVALVES_BEAM_ONOFF_LIST_CONFLICTION, QString("Confliction on valves beam on/off list: (%1, %2) -- (%3, %4)")
								  .arg(beamOnOrder).arg(control->name()).arg(beamOnOrder).arg(newValve->name()));
			} else {
				valvesBeamOnOrderMap_.insert(beamOnOrder, newValve);
			}
		}
	}

	return result;
}

bool CLSValves::removeValve(AMControl *valve)
{
	bool result = removeTriStateControl(valve);

	if (result) {
		emit valvesChanged();

		valvesBeamOnOrderMap_.remove(valvesBeamOnOrderMap_.key(valve));
	}

	return result;
}

bool CLSValves::clearValves()
{
	bool result = clearTriStateControls();

	if (result) {
		emit valvesChanged();

		valvesBeamOnOrderMap_.clear();
	}

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
