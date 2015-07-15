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


#include "SGMAppController.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/AMRun.h"
#include "beamline/CLS/CLSFacilityID.h"

SGMAppController::SGMAppController(QObject *parent) :
	AMAppController(parent)
{

}

bool SGMAppController::startup() {

	// Run all of the Acquaman App startup routines. Some of these are reimplemented in this class.
	if(!AMAppController::startup())
		return false;

	// Creates the SGM Beamline object
	SGMBeamline::sgm();

	// Initialize the storage ring.
	CLSStorageRing::sr1();

	// Retrieve the current run or create one if there is none
	AMRun existingRun;
	if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
		// no run yet... let's create one.
		AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::SGMBeamline), CLSFacilityID::SGMBeamline); //3: SGM Beamline
		firstRun.storeToDb(AMDatabase::database("user"));
	}

	registerClasses();
	setupExporterOptions();
	setupUserInterface();
	makeConnections();

	return true;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void SGMAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
}

void SGMAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
}

void SGMAppController::registerClasses()
{
}

void SGMAppController::setupExporterOptions()
{
}

void SGMAppController::setupUserInterface()
{
}

void SGMAppController::makeConnections()
{
}
