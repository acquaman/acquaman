#include "REIXSMoveToSampleTransferPositionAction.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "actions2/actions/AMInternalControlMoveAction.h"
#include "util/AMErrorMonitor.h"

REIXSMoveToSampleTransferPositionAction::REIXSMoveToSampleTransferPositionAction(REIXSMoveToSampleTransferPositionActionInfo *info, QObject *parent) :
	AMListAction(info, AMListAction::SequentialMode, parent)
{

}


void REIXSMoveToSampleTransferPositionAction::startImplementation()
{
	REIXSSampleChamber* sampleControls = REIXSBeamline::bl()->sampleChamber();

	// check that all the sample manipulator controls are connected
	if(!sampleControls->isConnected()) {
		AMErrorMon::alert(this, -4, "Could not move to the sample position because not all the sample manipulator controls are connected. Please report this problem to the beamline staff.");
		notifyFailed();
		return;
	}

	const AMControlInfoList& positions = qobject_cast<REIXSMoveToSampleTransferPositionActionInfo*>(info())->positions();
	const AMControlInfo& xTarget = positions.controlNamed("sampleX");
	const AMControlInfo& yTarget = positions.controlNamed("sampleY");
	const AMControlInfo& zTarget = positions.controlNamed("sampleZ");
	const AMControlInfo& thetaTarget = positions.controlNamed("sampleTheta");


	// build up the list actions: First the y move, then the x move, then the z move, then theta
	addSubAction(new AMInternalControlMoveAction(sampleControls->x(), xTarget.value()));
	addSubAction(new AMInternalControlMoveAction(sampleControls->y(), yTarget.value()));
	addSubAction(new AMInternalControlMoveAction(sampleControls->z(), zTarget.value()));
	addSubAction(new AMInternalControlMoveAction(sampleControls->r(), thetaTarget.value()));


}
