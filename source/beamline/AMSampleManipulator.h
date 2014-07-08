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


#ifndef AMSAMPLEMANIPULATOR_H
#define AMSAMPLEMANIPULATOR_H

#include "dataman/info/AMControlInfoList.h"

/// This specifies an interface for moving and marking the position of samples. AMSamplePlateView uses an instance of an AMSampleManipulator to grab the current manipulator position, and request for a sample to be moved into position.
/*! To implement this, all you need is to provide a position() function to read the current position, and a moveToPosition() function to initiate a move to that position. You can do that however you want: using actions, actions in the workflow, etc.

One implementation of this is provided (AMControlSetSampleManipulator), which simply sets and reads an AMControlSet. More advanced implementations might want to use actions in conjunction with the workflow, so that sample switches appear in the user's history log, can be copied to run again, etc.*/
class AMSampleManipulator
{

public:
	/// Empty constructor
	AMSampleManipulator() {}
	virtual ~AMSampleManipulator() {}

	/// Returns the current position of the sample manipulator, as an AMControlInfoList.
	virtual AMControlInfoList position() const = 0;
	/// Sends a request to move the sample manipulator into position \c newPosition. Returns true if the request is accepted; false if it cannot be started for any reason.
	virtual bool moveToPosition(const AMControlInfoList& newPosition) = 0;

	/// \todo Something to indicate valid/connected? Should we have signals for this too?
	/// \todo Something to indicate the reason for failure? (ie: out of range, etc.?)

	/// Returns the facility ID where this manipulator is found. It would be bad to try to move to sample positions from one facility when at another facility. [This facility ID should correspond to an AMFacility object in the user database.]  The base implementation return 0 [invalid facility].
	virtual int facilityId() const { return 0; }
};

#endif // AMSAMPLEMANIPULATOR_H
