/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "REIXSSampleMoveAction.h"
#include "util/AMErrorMonitor.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "actions2/actions/AMInternalControlMoveAction.h"
#include "dataman/AMSamplePlatePre2013.h"

REIXSSampleMoveAction::REIXSSampleMoveAction(REIXSSampleMoveActionInfo *info, QObject *parent) :
	AMListAction(info, AMListAction::ParallelMode, parent)
{

}

REIXSSampleMoveAction::REIXSSampleMoveAction(const AMControlInfoList &targetPositions, QObject *parent) :
	AMListAction(new REIXSSampleMoveActionInfo(targetPositions), AMListAction::ParallelMode, parent)
{

}

void REIXSSampleMoveAction::startImplementation()
{
	double x, y, z, theta;

	// Mode 1: moving to sample on sample plate:
	if(sampleMoveInfo()->samplePlateId() > 0) {

		int infoPlateId = sampleMoveInfo()->samplePlateId();
		int currentPlateId = REIXSBeamline::bl()->samplePlate()->id();
		int sampleIndex = sampleMoveInfo()->sampleIndex();

		if(infoPlateId != currentPlateId) {
			AMErrorMon::alert(this, -2, "Could not move to the sample because the sample plate is not the current sample plate. Use the 'Sample Positions' page to change the current sample plate.");
			notifyFailed();
			return;
		}

		if(sampleIndex >= REIXSBeamline::bl()->samplePlate()->count()) {
			AMErrorMon::alert(this, -201, QString("Could not move to the sample because the sample chosen (# %1) is not on the plate (%2 samples)").arg(sampleIndex).arg(REIXSBeamline::bl()->samplePlate()->count()));
			notifyFailed();
			return;
		}

		if(sampleIndex < 0) {
			AMErrorMon::alert(this, -202, QString("Could not move to the sample because an invalid sample was chosen (# %1)").arg(sampleIndex));
			notifyFailed();
			return;
		}

		const AMControlInfoList& pos = REIXSBeamline::bl()->samplePlate()->at(sampleIndex).position();
		try {
		int index;
		if((index = pos.indexOf("sampleX")) != -1)
			x = pos.at(index).value();
		else
			throw 0;
		if((index = pos.indexOf("sampleY")) != -1)
			y = pos.at(index).value();
		else
			throw 1;
		if((index = pos.indexOf("sampleZ")) != -1)
			z = pos.at(index).value();
		else
			throw 2;
		if((index = pos.indexOf("sampleTheta")) != -1)
			theta = pos.at(index).value();
		else
			throw 3;
		}
		catch(int) {
			AMErrorMon::alert(this, -3, "Could not move to the sample because the sample position has not been marked properly. Please report this problem to the REIXS software developers.");
			notifyFailed();
			return;
		}
	}
	// Mode 2: manual fixed position:
	else {
		x = sampleMoveInfo()->x();
		y = sampleMoveInfo()->y();
		z = sampleMoveInfo()->z();
		theta = sampleMoveInfo()->theta();
	}

	REIXSSampleChamber* sampleControls = REIXSBeamline::bl()->sampleChamber();

	// check that all the sample manipulator controls are connected
	if(!sampleControls->isConnected()) {
		AMErrorMon::alert(this, -4, "Could not move to the sample position because not all the sample manipulator controls are connected. Please report this problem to the beamline staff.");
		notifyFailed();
		return;
	}


	// This is the old version, that did a complicated sequence of moves to move safely between positions in the vicinity of the entrance slit. Now that we are slitless, we can change this to a Parallel action, and just do the X,Y,Z,Theta moves immediately.
	////////////////////////////////

//	// Add the move actions in order. If we have a negative X move, do that. Then, if there's a negative Y move, do that. This will take us as far as possible away from the slit.  Then do rotation, then Z move. Then do any positive Y move, and finally any positive X move.

//	// Move away as far as possible
//	if(x < sampleControls->x()->value()) {
//		addSubAction(new AMInternalControlMoveAction(sampleControls->x(), x));
//	}
//	if(y < sampleControls->y()->value()) {
//		addSubAction(new AMInternalControlMoveAction(sampleControls->y(), y));
//	}

//	// Now rotation
//	addSubAction(new AMInternalControlMoveAction(sampleControls->r(), theta));
//	// And height change
//	addSubAction(new AMInternalControlMoveAction(sampleControls->z(), z));

//	// Now re-approach
//	if(y > sampleControls->y()->value()) {
//		addSubAction(new AMInternalControlMoveAction(sampleControls->y(), y));
//	}
//	if(x > sampleControls->x()->value()) {
//		addSubAction(new AMInternalControlMoveAction(sampleControls->x(), x));
//	}
	////////////////////////////

	addSubAction(new AMInternalControlMoveAction(sampleControls->x(), x));
	addSubAction(new AMInternalControlMoveAction(sampleControls->y(), y));
	addSubAction(new AMInternalControlMoveAction(sampleControls->z(), z));
	addSubAction(new AMInternalControlMoveAction(sampleControls->r(), theta));

	// Now that those sub-actions are setup, just let AMListAction do its job to run them.
	AMListAction::startImplementation();
}
