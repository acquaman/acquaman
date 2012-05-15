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
