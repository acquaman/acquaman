/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
