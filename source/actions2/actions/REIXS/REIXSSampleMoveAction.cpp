#include "REIXSSampleMoveAction.h"
#include "util/AMErrorMonitor.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "actions2/actions/AMInternalControlMoveAction.h"

REIXSSampleMoveAction::REIXSSampleMoveAction(REIXSSampleMoveActionInfo *info, QObject *parent) :
	AMListAction(info, AMListAction::SequentialMode, parent)
{

}

REIXSSampleMoveAction::REIXSSampleMoveAction(const AMControlInfoList &targetPositions, QObject *parent) :
	AMListAction(new REIXSSampleMoveActionInfo(targetPositions), AMListAction::SequentialMode, parent)
{

}

void REIXSSampleMoveAction::startImplementation()
{
	// check that the info is valid -- we know where we're supposed to be going
	if(!sampleMoveInfo()->isValid()) {
		AMErrorMon::alert(this, -3, "Could not move to the sample position because a valid position wasn't specified. Please report this problem to the Acquaman developers.");
		notifyFailed();
		return;
	}

	REIXSSampleChamber* sampleControls = REIXSBeamline::bl()->sampleChamber();

	// check that all the sample manipulator controls are connected
	if(!sampleControls->isConnected()) {
		AMErrorMon::alert(this, -4, "Could not move to the sample position because not all the sample manipulator controls are connected. Please report this problem to the beamline staff.");
		notifyFailed();
		return;
	}

	const AMControlInfo& xTarget = sampleMoveInfo()->targetPositions().controlNamed("sampleX");
	const AMControlInfo& yTarget = sampleMoveInfo()->targetPositions().controlNamed("sampleY");
	const AMControlInfo& zTarget = sampleMoveInfo()->targetPositions().controlNamed("sampleZ");
	const AMControlInfo& thetaTarget = sampleMoveInfo()->targetPositions().controlNamed("sampleTheta");

	// Add the move actions in order. If we have a negative X move, do that. Then, if there's a negative Y move, do that. This will take us as far as possible away from the slit.  Then do rotation, then Z move. Then do any positive Y move, and finally any positive X move.

	// Move away as far as possible
	if(xTarget.value() < sampleControls->x()->value()) {
		addSubAction(new AMInternalControlMoveAction(sampleControls->x(), xTarget.value()));
	}
	if(yTarget.value() < sampleControls->y()->value()) {
		addSubAction(new AMInternalControlMoveAction(sampleControls->y(), yTarget.value()));
	}

	// Now rotation
	addSubAction(new AMInternalControlMoveAction(sampleControls->r(), thetaTarget.value()));
	// And height change
	addSubAction(new AMInternalControlMoveAction(sampleControls->z(), zTarget.value()));

	// Now re-approach
	if(yTarget.value() > sampleControls->y()->value()) {
		addSubAction(new AMInternalControlMoveAction(sampleControls->y(), yTarget.value()));
	}
	if(xTarget.value() > sampleControls->x()->value()) {
		addSubAction(new AMInternalControlMoveAction(sampleControls->x(), xTarget.value()));
	}


	// Now that those sub-actions are setup, just let AMListAction do its job to run them.
	AMListAction::startImplementation();
}
