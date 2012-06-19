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
