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


#ifndef AMDBOBJECTDEFINITIONS_H
#define AMDBOBJECTDEFINITIONS_H

namespace AMDbObjectDefinitions {
	/// This enum lists the possible load statuses and is returned by AMDbObject::loadFromDb() as well as by functions in AMDbObjectSupport
	enum AMDbObjectLoadStatus{
		AlreadyLoadedStatus = 0,	///< The AMDbObject is already loaded into the retention system
		NotYetLoadedStatus = 1,		///< The AMDbObject is not yet loaded into the retention system
		SuccessfullyLoadedStatus = 2	///< Whatever operation just occured successfully loaded the AMDbObject into the retention system
	};
}

#endif // AMDBOBJECTDEFINITIONS_H
