#include "CLSBeamStatusShutters.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

#include "util/AMErrorMonitor.h"

CLSBeamStatusShutters::CLSBeamStatusShutters(const QString &name, QObject *parent) :
	CLSShutters(name, parent)
{
	safetyShutter_ = 0;
}


CLSBeamStatusShutters::~CLSBeamStatusShutters()
{
	shuttersBeamOnOrderMap_.clear();
}

bool CLSBeamStatusShutters::isSafetyShutterOpen() const
{
	if (safetyShutter_) {
		return safetyShutter_->isConnected() && isChildState1(safetyShutter_);
	}

	return false;
}

bool CLSBeamStatusShutters::isSafeToOpenShutters() const
{
	if (safetyShutter_) { // when the safety shutter is open or the safety shutter is in the beam on shutters list (then we can open it programatically)
		return safetyShutter_->isConnected()
			&& (isChildState1(safetyShutter_) || shuttersBeamOnOrderMap_.values().contains(safetyShutter_));
	}

	return true;
}

bool CLSBeamStatusShutters::addShutter(AMControl *newShutter, double openStateValue, double closedStateValue, int beamOnOrder)
{
	bool result = CLSShutters::addShutter(newShutter, openStateValue, closedStateValue);

	if (result) {
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

bool CLSBeamStatusShutters::removeShutter(AMControl *shutter)
{
	bool result = CLSShutters::removeShutter(shutter);
	if ( result )
		shuttersBeamOnOrderMap_.remove(shuttersBeamOnOrderMap_.key(shutter));

	return result;
}

bool CLSBeamStatusShutters::clearShutters()
{
	bool result = CLSShutters::clearShutters();

	if (result)
		shuttersBeamOnOrderMap_.clear();

	return result;
}

void CLSBeamStatusShutters::setSafetyShutter(AMControl *safetyShutter, double openStateValue, double closedStateValue, int beamOnOrder)
{
	if (safetyShutter_ !=  safetyShutter) {
		removeShutter(safetyShutter_);

		safetyShutter_ = safetyShutter;

		if (safetyShutter_) {
			addShutter(safetyShutter_, openStateValue, closedStateValue, beamOnOrder);
		}

		emit safetyShutterChanged(safetyShutter_);
	}
}

AMAction3* CLSBeamStatusShutters::createMoveToOpenAction()
{
	// create the action list to move the Shutters (sequentially)
	AMListAction3 *shuttersBeamOnActionList = new AMListAction3(new AMListActionInfo3("Open shutters action list", "Open Shutters"), AMListAction3::Sequential);

	// create open/wait actions for all the closed shutters
	foreach (int beamOnOrder, shuttersBeamOnOrderMap_.keys()) {
		AMControl *shutterControl = shuttersBeamOnOrderMap_.value(beamOnOrder);
		if (shutterControl) {
			AMListAction3 *openShutterActionList = new AMListAction3(new AMListActionInfo3(QString("Open shutter %1 action list").arg(shutterControl->name()), "Open Shutter"), AMListAction3::Parallel);
			openShutterActionList->addSubAction(createMoveChildToOpenAction(shutterControl));
			openShutterActionList->addSubAction(createWaitChildMoveToOpenAction(shutterControl));

			// add to the beam on action list
			if (openShutterActionList->subActionCount() != 0)
				shuttersBeamOnActionList->addSubAction(openShutterActionList);
			else
				openShutterActionList->deleteLater();
		}
	}

	// check whether there is valid actions in the action list
	if (shuttersBeamOnActionList->subActionCount() == 0) {
		shuttersBeamOnActionList->deleteLater();
		shuttersBeamOnActionList = 0;
	}

	return shuttersBeamOnActionList;
}

AMAction3* CLSBeamStatusShutters::createMoveToClosedAction()
{
	// create the action list to move the Shutters (sequentially)
	AMListAction3 *shuttersBeamOffActionList = new AMListAction3(new AMListActionInfo3("Close shutters action list", "Close Shutters"), AMListAction3::Sequential);

	// create open/wait actions for all the opened shutters
	foreach (int beamOnOrder, shuttersBeamOnOrderMap_.keys()) {
		AMControl *shutterControl = shuttersBeamOnOrderMap_.value(beamOnOrder);
		if (shutterControl) {
			AMListAction3 *closeShutterActionList = new AMListAction3(new AMListActionInfo3(QString("Close shutter %1 action list").arg(shutterControl->name()), "Open Shutter"), AMListAction3::Parallel);
			closeShutterActionList->addSubAction(createMoveChildToClosedAction(shutterControl));
			closeShutterActionList->addSubAction(createWaitChildMoveToClosedAction(shutterControl));

			// insert to the beginning of the beam off action list -- close the shutters in the reverse of the beam on order
			if (closeShutterActionList->subActionCount() != 0)
				shuttersBeamOffActionList->insertSubAction(closeShutterActionList, 0);
			else
				closeShutterActionList->deleteLater();
		}
	}

	// check whether there is valid actions in the action list
	if (shuttersBeamOffActionList->subActionCount() == 0) {
		shuttersBeamOffActionList->deleteLater();
		shuttersBeamOffActionList = 0;
	}

	return shuttersBeamOffActionList;
}
