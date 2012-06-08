#include "AMChangeRunAction.h"

#include "dataman/AMRun.h"
#include "dataman/AMUser.h"

void AMChangeRunAction::startImplementation() {
	AMChangeRunActionInfo* changeInfo = qobject_cast<AMChangeRunActionInfo*>(info());

	if(changeInfo) {
		if(changeInfo->runId() < 1) {
			// create new run:
			// Look up facility from current run:
			AMRun currentRun(AMUser::user()->currentRunId(), AMDatabase::database("user"));

			AMRun newRun(changeInfo->newRunName(), currentRun.facilityId());

			newRun.storeToDb(AMDatabase::database("user"));
			AMUser::user()->setCurrentRunId(newRun.id());
		}

		else {
			// switch to existing run:
			AMUser::user()->setCurrentRunId(changeInfo->runId());
		}
	}

	notifyStarted();
	notifySucceeded();
}
