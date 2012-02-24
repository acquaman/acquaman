#include "beamline/REIXS/REIXSSampleManipulator.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "actions2/AMActionRunner.h"
#include "actions2/actions/REIXS/REIXSSampleMoveAction.h"
#include "util/AMErrorMonitor.h"

AMControlInfoList REIXSSampleManipulator::position() const {
	return REIXSBeamline::bl()->sampleManipulatorSet()->toInfoList();
}


bool REIXSSampleManipulator::moveToPosition(const AMControlInfoList &newPosition) {

	// Note: we could also do this with AMActionRunner's immediate action mode, if we didn't care about letting the user move the sample while other actions were running. Instead, we use the regular queue.

	// anything currently running in the workflow? If there is, probably don't want to let the user move the sample.
	if(AMActionRunner::s()->actionRunning()) {
		AMErrorMon::alert(0, -1, "REIXSSampleManipulator: Could not move the sample position, because actions are still running. Please wait until they are finished, or pause the workflow, and try again.");
		return false;
	}

	// Nothing currently running. Throw an action into the front of the workflow, start it, and then restore the queuePaused state to whatever it used to be.
	bool queuePaused = AMActionRunner::s()->queuePaused();
	AMActionRunner::s()->insertActionInQueue(new REIXSSampleMoveAction(newPosition), 0);
	AMActionRunner::s()->setQueuePaused(false);
	AMActionRunner::s()->setQueuePaused(queuePaused);

	return true;
}
