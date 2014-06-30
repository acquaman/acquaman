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


#include <QCoreApplication>
#include "beamline/SGM/SGMLookupTableCoordinator.h"

int main(int argc, char *argv[])
{

	// =================================
	QCoreApplication app(argc, argv);
	app.setApplicationName("SGM Lookup Table Coordinator");

	//SGMLookupTableCoordinator *coordinator = new SGMLookupTableCoordinator("BL1611-ID-1:Energy:fbk", "PSH16114I1001:V", "BL1611-ID-1:AddOns:lookupTable:linkage", "BL1611-ID-1:AddOns:lookupTable:loadLookup");
	SGMLookupTableCoordinator *coordinator = new SGMLookupTableCoordinator("BL1611-ID-1:AddOns:InputControl", "BL1611-ID-1:AddOns:OutputControl", "BL1611-ID-1:AddOns:lookupTable:linkage", "BL1611-ID-1:AddOns:lookupTable:loadLookup");
	//SGMLookupTableCoordinator *coordinator = new SGMLookupTableCoordinator("BL1611-ID-1:AddOns:lookupTable:fakeInput", "BL1611-ID-1:AddOns:lookupTable:fakeOutput", "BL1611-ID-1:AddOns:lookupTable:linkage", "BL1611-ID-1:AddOns:lookupTable:loadLookup");

	Q_UNUSED(coordinator)

	return app.exec();
}
