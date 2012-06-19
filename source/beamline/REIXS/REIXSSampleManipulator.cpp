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

	// Nothing currently running. We can run this one action immediately in the queue.
	AMActionRunner::s()->runActionImmediatelyInQueue(new REIXSSampleMoveAction(newPosition));

	return true;
}
