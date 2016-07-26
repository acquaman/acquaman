#include "CLSBeamStatusValves.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include "util/AMErrorMonitor.h"

CLSBeamStatusValves::CLSBeamStatusValves(const QString &name, QObject *parent) :
	CLSValves(name, parent)
{
}

CLSBeamStatusValves::~CLSBeamStatusValves()
{
	valvesBeamOnOrderMap_.clear();
}

bool CLSBeamStatusValves::addValve(AMControl *newValve, double openStateValue, double closedStateValue, int beamOnOrder)
{
	bool result = CLSValves::addValve(newValve, openStateValue, closedStateValue);
	if (result) {
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

bool CLSBeamStatusValves::removeValve(AMControl *valve)
{
	bool result = CLSValves::removeTriStateControl(valve);
	if (result) {
		valvesBeamOnOrderMap_.remove(valvesBeamOnOrderMap_.key(valve));
	}

	return result;
}

bool CLSBeamStatusValves::clearValves()
{
	bool result = CLSValves::clearValves();
	if (result) {
		valvesBeamOnOrderMap_.clear();
	}

	return result;
}

AMAction3* CLSBeamStatusValves::createMoveToOpenAction()
{
	// create the action list to open the valves (sequentially)
	AMListAction3 *valvesBeamOnActionList = new AMListAction3(new AMListActionInfo3("Open valves action list", "Open Valves"), AMListAction3::Sequential);

	// this is to make sure all the controls are checked
	foreach (int beamOnOrder, valvesBeamOnOrderMap_.keys()) {
		AMControl *valveControl = valvesBeamOnOrderMap_.value(beamOnOrder);
		if (valveControl && isChildState2(valveControl)) { // the valve is closed
			AMListAction3 *openValveActionList = new AMListAction3(new AMListActionInfo3(QString("Open valves %1 action list").arg(valveControl->name()), "Open Valves"), AMListAction3::Parallel);

			AMAction3 *openValveAction = AMActionSupport::buildControlMoveAction(valveControl, 1);
			openValveActionList->addSubAction(openValveAction);

			AMAction3 *openValveWaitAction = AMActionSupport::buildControlWaitAction(valveControl, 1);
			openValveActionList->addSubAction(openValveWaitAction);

			// add the open valve action list to the group list
			valvesBeamOnActionList->addSubAction(openValveActionList);
		}
	}

	// add the open/wait action lists to the beam on action list
	if (valvesBeamOnActionList->subActionCount() == 0 ) {
		valvesBeamOnActionList->deleteLater();
		valvesBeamOnActionList = 0;
	}

	return valvesBeamOnActionList;
}
