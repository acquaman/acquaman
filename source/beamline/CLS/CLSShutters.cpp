#include "CLSShutters.h"

#include "beamline/AMControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

#include "util/AMErrorMonitor.h"

CLSShutters::CLSShutters(const QString &name, QObject *parent) :
	CLSTriStateGroup(name, parent)
{
	safetyShutter_ = 0;

	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed", true);
}

CLSShutters::~CLSShutters()
{
	shuttersBeamOnOrderMap_.clear();
}

AMListAction3* CLSShutters::createBeamOnActionList()
{
	// create the action list to move the Shutters (sequentially) and wait for the move done
	AMListAction3 *openShuttersActionList = new AMListAction3(new AMListActionInfo3("Open shutters action list", "Open Shutters"), AMListAction3::Sequential);
	AMListAction3 *waitShuttersOpenActionList = new AMListAction3(new AMListActionInfo3("Wait shutters open action list", "Wait Shutters Open"), AMListAction3::Parallel);

	// this is to make sure all the controls are checked
	int currentBeamOnOrder = 1;
	int checkedControlCount = 0;
	while (checkedControlCount < shuttersBeamOnOrderMap_.count()) {
		AMControl *shutterControl = shuttersBeamOnOrderMap_.value(currentBeamOnOrder);
		if (shutterControl) {
			if (isChildState2(shutterControl)) { // the Shutter is closed
				double valveOpenValue = controlState1ValueMap_.value(shutterControl);
				AMAction3 *openShutterAction = AMActionSupport::buildControlMoveAction(shutterControl, valveOpenValue);
				openShuttersActionList->addSubAction(openShutterAction);

				AMAction3 *waitShutterOpenAction = AMActionSupport::buildControlWaitAction(shutterControl, valveOpenValue);
				waitShuttersOpenActionList->addSubAction(waitShutterOpenAction);
			}

			checkedControlCount ++;
		}

		currentBeamOnOrder++;
	}

	// add the open/wait action lists to the beam on action list
	AMListAction3 *openShuttersActionsList = 0;
	if (openShuttersActionList->subActionCount() > 0) {
		AMListAction3 *openShuttersActionsList = new AMListAction3(new AMListActionInfo3("Beam On - Shutter", "Beam On: Open shutters"), AMListAction3::Parallel);
		openShuttersActionsList->addSubAction(openShuttersActionList);
		openShuttersActionsList->addSubAction(waitShuttersOpenActionList);
	} else {
		openShuttersActionList->deleteLater();
		waitShuttersOpenActionList->deleteLater();
	}

	return openShuttersActionsList;
}

AMListAction3* CLSShutters::createBeamOffActionList()
{
	// create the action list to move the Shutters (sequentially) and wait for the move done
	AMListAction3 *closeShuttersActionList = new AMListAction3(new AMListActionInfo3("Close shutters action list", "Close Shutters"), AMListAction3::Sequential);
	AMListAction3 *waitShuttersCloseActionList = new AMListAction3(new AMListActionInfo3("Wait shutters closed action list", "Wait Shutters Closed"), AMListAction3::Parallel);

	// this is to make sure all the controls are checked
	int currentBeamOffOrder = 1;
	int checkedControlCount = 0;
	while (checkedControlCount < shuttersBeamOnOrderMap_.count()) {
		AMControl *shutterControl = shuttersBeamOnOrderMap_.value(currentBeamOffOrder);
		if (shutterControl) {
			double valveCloseValue = controlState2ValueMap_.value(shutterControl);

			AMAction3 *closeShutterAction = AMActionSupport::buildControlMoveAction(shutterControl, valveCloseValue);
			closeShuttersActionList->addSubAction(closeShutterAction);

			AMAction3 *waitShutterClosedAction = AMActionSupport::buildControlWaitAction(shutterControl, valveCloseValue);
			waitShuttersCloseActionList->addSubAction(waitShutterClosedAction);

			checkedControlCount ++;
		}

		currentBeamOffOrder++;
	}

	// add the close/wait action lists to the beam on action list
	AMListAction3 *closeShuttersActionsList = 0;
	if (closeShuttersActionList->subActionCount() > 0) {
		AMListAction3 *openShuttersActionsList = new AMListAction3(new AMListActionInfo3("Beam Off - shutters", "Beam Off: close shutters"), AMListAction3::Parallel);
		openShuttersActionsList->addSubAction(closeShuttersActionList);
		openShuttersActionsList->addSubAction(waitShuttersCloseActionList);
	} else {
		closeShuttersActionList->deleteLater();
		waitShuttersCloseActionList->deleteLater();
	}

	return closeShuttersActionsList;
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

bool CLSShutters::setSafetyShutter(AMControl *safetyShutter)
{
	if (safetyShutter_ !=  safetyShutter) {
		disconnect(safetyShutter_, 0, this, 0);
		safetyShutter_ = safetyShutter;
	}
}

bool CLSShutters::addShutter(AMControl *newShutter, double openValue, double closedValue, int beamOnOrder)
{
	bool result = addTriStateControl(newShutter, openValue, closedValue);

	if (result) {
		emit shuttersChanged();

		if (beamOnOrder > 0) {
			AMControl * control = shuttersBeamOnOrderMap_.value(beamOnOrder);
			if (control) {
				AMErrorMon::alert(this, CLSSHUTTERS_BEAM_ONOFF_LIST_CONFLICTION, QString("Confliction on shutters beam on/off list: (%1, %2) -- (%3, %4)")
								  .arg(beamOnOrder).arg(control->name()).arg(beamOnOrder).arg(newShutter->name()));
			} else {
				shuttersBeamOnOrderMap_.insert(beamOnOrder, newShutter);
			}
		}
	}

	return result;
}

bool CLSShutters::removeShutter(AMControl *shutter)
{
	bool result = removeTriStateControl(shutter);

	if (result)
		emit shuttersChanged();

	return result;
}

bool CLSShutters::clearShutters()
{
	bool result = clearTriStateControls();

	if (result)
		emit shuttersChanged();

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

AMAction3* CLSShutters::createMoveChildToOpen(AMControl *child)
{
	return createMoveChildToState1Action(child);
}

AMAction3* CLSShutters::createMoveChildToClosed(AMControl *child)
{
	return createMoveChildToState2Action(child);
}

AMAction3* CLSShutters::createCheckChildIsOpen(AMControl *child, double timeoutSec)
{
	return createCheckChildAtState1Action(child, timeoutSec);
}

AMAction3* CLSShutters::createCheckChildIsClosed(AMControl *child, double timeoutSec)
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
