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

#include "ui/util/AMChooseDataFolderDialog.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"

PGMAppController::PGMAppController(QObject *parent)
	: CLSAppController("PGM", parent)
{
	setDefaultUseLocalStorage(true);
}

bool PGMAppController::startup()
{
    // Get a destination folder.
	if (!AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/pgm", "/home/pgm", "users"))
        return false;

	// Start up the main program.
	if(CLSAppController::startup()) {
		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);
		return true;
	}
	else
		return false;
}

void PGMAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	CLSAppController::shutdown();
}

void PGMAppController::initializeBeamline()
{
	// Initialize central beamline object
	PGMBeamline::pgm();
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

	// PUT REAL PERSISTENT VIEW HERE
//	PGMPersistentView *persistentPanel = new PGMPersistentView;
//	mw_->addRightWidget(persistentPanel);
}

void PGMAppController::makeConnections()
{

}
