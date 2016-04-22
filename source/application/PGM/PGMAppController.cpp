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


#include "PGMAppController.h"

#include "beamline/CLS/CLSFacilityID.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/PGM/PGMBeamline.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/AMRun.h"
#include "dataman/AMScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMSMAKExporter.h"
#include "dataman/export/AMExporterOptionSMAK.h"

#include "util/AMPeriodicTable.h"

#include "ui/util/AMChooseDataFolderDialog.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"

#include "ui/PGM/PGMPersistentView.h"

#include "ui/PGM/PGMHVControlViewBranchA.h"
#include "ui/PGM/PGMHVControlViewBranchB.h"

PGMAppController::PGMAppController(QObject *parent)
	: AMAppController(parent)
{
	setDefaultUseLocalStorage(true);
}

bool PGMAppController::startup()
{
    // Get a destination folder.
    if (!AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/ideas", "/home/pgm", "users"))
        return false;

	// Start up the main program.
	if(AMAppController::startup()) {

		// Initialize central beamline object
		PGMBeamline::pgm();
		// Initialize the periodic table object.
		AMPeriodicTable::table();
		// Initialize the storage ring.
		CLSStorageRing::sr1();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

			AMRun firstRun("PGM", 11); //11: because this is temporary.
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

void PGMAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void PGMAppController::registerClasses()
{

}

void PGMAppController::setupExporterOptions()
{

}

void PGMAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("General", 0);

	mw_->insertHeading("XRF Detectors", 1);

	mw_->insertHeading("Scans", 2);

    PGMHVControlViewBranchA *hvBranchA = new PGMHVControlViewBranchA();
    mw_->addPane(hvBranchA, "General", "High Voltage Branch A",  ":/utilities-system-monitor.png");

    PGMHVControlViewBranchB *hvBranchB = new PGMHVControlViewBranchB();
    mw_->addPane(hvBranchB, "General", "High Voltage Branch B",  ":/utilities-system-monitor.png");



    PGMPersistentView *persistentPanel = new PGMPersistentView;
    mw_->addRightWidget(persistentPanel);
}

void PGMAppController::makeConnections()
{

}
