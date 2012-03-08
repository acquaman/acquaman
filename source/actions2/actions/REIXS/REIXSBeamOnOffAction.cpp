#include "REIXSBeamOnOffAction.h"
#include "beamline/REIXS/REIXSBeamline.h"


#include "actions2/actions/AMInternalControlMoveAction.h"

void REIXSBeamOnOffAction::startImplementation()
{
	REIXSBeamOnOffActionInfo* beamInfo = qobject_cast<REIXSBeamOnOffActionInfo*>(info());

	if(beamInfo && beamInfo->beamOn()) {
		addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->valvesAndShutters()->psh2(), 1.0));
		addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->valvesAndShutters()->psh4(), 1.0));
		//addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->valvesAndShutters()->endstationValve(), 1.0));
	}

	else {
		addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->valvesAndShutters()->psh4(), 0.0));
	}

	AMListAction::startImplementation();
}
