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


#ifndef REIXSCONTROLMOVEACTIONINFO_H
#define REIXSCONTROLMOVEACTIONINFO_H

#include "actions2/actions/AMControlMoveActionInfo.h"

/// This trivial subclass of AMControlMoveActionInfo is used to verify that (when looking at a stored move action in the database) we know that it came from and can be executed on the REIXS beamline. It provides no new functionality; it simply distinguishes instances from the general AMControlMoveActionInfo.
class REIXSControlMoveActionInfo : public AMControlMoveActionInfo
{
    Q_OBJECT
public:
	/// Constructor. You should always specify a valid \c setpoint, but we provide the default argument because we need a default constructor for database loading.
	Q_INVOKABLE REIXSControlMoveActionInfo(const AMControlInfo& setpoint = AMControlInfo(), QObject *parent = 0) : AMControlMoveActionInfo(setpoint, parent) {}

	/// Copy Constructor
	REIXSControlMoveActionInfo(const REIXSControlMoveActionInfo& other) : AMControlMoveActionInfo(other) {}

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo* createCopy() const { return new REIXSControlMoveActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "REIXS Control Move"; }

};

#endif // REIXSCONTROLMOVEACTIONINFO_H
