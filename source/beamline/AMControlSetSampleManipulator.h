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


#ifndef AMCONTROLSETSAMPLEMANIPULATOR_H
#define AMCONTROLSETSAMPLEMANIPULATOR_H

#include "beamline/AMSampleManipulator.h"

class AMControlSet;

/// This is a simple implementation of the AMSampleManipulator interface that uses an AMControlSet to directly read and move the sample position. More advanced implementations might want to use actions in conjunction with the workflow, so that sample switches appear in the user's history log, can be copied to run again, etc.
class AMControlSetSampleManipulator : public AMSampleManipulator
{
public:
	/// Construct a sample manipulator based on this \c controlSet.  The \c controlSet should exist as long as we are around and being used, we do not take ownership of it.
 	virtual ~AMControlSetSampleManipulator();
	explicit AMControlSetSampleManipulator(AMControlSet* controlSet);

	/// Returns the control set
	AMControlSet* controlSet() { return controlSet_; }

	/// Reimplemented to read the position from the control set
	virtual AMControlInfoList position() const;
	/// Reimplemented to start a move to \c newPosition. Returns false if the control set is not connected.
	virtual bool moveToPosition(const AMControlInfoList &newPosition);

protected:
	/// The control set
	AMControlSet* controlSet_;
};

#endif // AMCONTROLSETSAMPLEMANIPULATOR_H
