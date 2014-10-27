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


#include "SXRMBAppController.h"

#include "beamline/SXRMB/SXRMBBeamline.h"

#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/AMRun.h"

#include "util/AMPeriodicTable.h"

SXRMBAppController::SXRMBAppController(QObject *parent)
	: AMAppController(parent)
{
}

bool SXRMBAppController::startup()
{
	getUserDataFolderFromDialog();

	// Start up the main program.
	if(AMAppController::startup()) {


		// Initialize central beamline object
		SXRMBBeamline::sxrmb();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

                        AMRun firstRun("SXRMB", 9);	/// \todo For now, we know that 7 is the ID of the BioXAS main endstation facility, but this is a hardcoded hack.
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		setupExporterOptions();
		setupUserInterface();
		makeConnections();

		return true;
	}
	else
		return false;
}

void SXRMBAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void SXRMBAppController::registerClasses()
{

}

void SXRMBAppController::setupExporterOptions()
{

}

void SXRMBAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("General", 0);

	mw_->insertHeading("Detectors", 1);

	mw_->insertHeading("Scans", 2);

}

void SXRMBAppController::makeConnections()
{
}

void SXRMBAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}

void SXRMBAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}
