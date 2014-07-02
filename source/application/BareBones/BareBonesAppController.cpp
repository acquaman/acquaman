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


#include "BareBonesAppController.h"


BareBonesAppController::BareBonesAppController(QObject *parent) :
    AMAppController(parent)
{
}

// Re-implemented to create the Beamline object

//bool BareBonesAppController::startupBeforeAnything() {
//	if(!AMAppController::startupBeforeAnything()) return false;

//	// Initialize the central beamline object
//	BareBonesBeamline::bl();

//	return true;
//}


// Re-implemented to add custom user-interface elements

//bool BareBonesAppController::startupCreateUserInterface() {
//	if(!AMAppController::startupCreateUserInterface()) return false;

//	// create UI elements and add to main window

//	return true;
//}



//void BareBonesAppController::shutdown() {

//    // Make sure we release/clean-up the beamline interface
//    AMBeamline::releaseBl();

//    AMAppController::shutdown();
//}
