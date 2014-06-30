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


#ifndef REIXSSAMPLEMANIPULATOR_H
#define REIXSSAMPLEMANIPULATOR_H

#include "beamline/AMSampleManipulator.h"

/// Implements the AMSampleManipulator interface for the REIXS beamline, so that AMSamplePlateView can measure and move to sample positions.  For measuring, we grab the sample manipulator positions from the beamline controls. For moving, we add a REIXSSampleMoveAction to the front of the workflow and run it (assuming nothing else is currently running. If something is running, we return that we cannot move right now.)
class REIXSSampleManipulator : public AMSampleManipulator {
public:
	/// Constructor
 	virtual ~REIXSSampleManipulator();
	REIXSSampleManipulator() : AMSampleManipulator() {}

	/// Returns the current position of the sample manipulator, as an AMControlInfoList.
	virtual AMControlInfoList position() const;
	/// Sends a request to move the sample manipulator into position \c newPosition. Returns true if the request is accepted; false if it cannot be started for any reason.
	virtual bool moveToPosition(const AMControlInfoList& newPosition);

	/// Returns the facility ID where this manipulator is found. It would be bad to try to move to sample positions from one facility when at another facility. The REIXS facility id is 5. \todo Make a more robust facility id registration system.
	virtual int facilityId() const { return 5; }

};

#endif // REIXSSAMPLEMANIPULATOR_H
