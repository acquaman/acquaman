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


#include "BioXASImagingAppController.h"

#include "beamline/BioXAS/BioXASImagingBeamline.h"

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

#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/util/AMChooseDataFolderDialog.h"

#include "ui/BioXAS/BioXASCarbonFilterFarmView.h"

BioXASImagingAppController::BioXASImagingAppController(QObject *parent)
	: CLSAppController("BioXAS", parent)
{
	setDefaultUseLocalStorage(true);
}

bool BioXASImagingAppController::startup()
{
	// Start up the main program.
	if(CLSAppController::startup()) {
		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		return true;
	}
	else
		return false;
}

void BioXASImagingAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	CLSAppController::shutdown();
}

bool BioXASImagingAppController::setupDataFolder()
{
	// Get a destination folder.
	if ( !AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-i/AcquamanImagingData", "/home/bioxas-i/AcquamanImagingData", "users", QStringList()) )
		return false;

	return true;
}

void BioXASImagingAppController::initializeBeamline()
{
	// Initialize central beamline object
	BioXASImagingBeamline::bioXAS();
}

void BioXASImagingAppController::registerClasses()
{

}

void BioXASImagingAppController::setupExporterOptions()
{
	QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "BioXAS Default XAS");

	AMExporterOptionGeneralAscii *bioXASDefaultXAS = new AMExporterOptionGeneralAscii();

	if (matchIDs.count() != 0)
			bioXASDefaultXAS->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

	bioXASDefaultXAS->setName("BioXASImaging Default XAS");
	bioXASDefaultXAS->setFileName("$name_$fsIndex.dat");
	bioXASDefaultXAS->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
	bioXASDefaultXAS->setHeaderIncluded(true);
	bioXASDefaultXAS->setColumnHeader("$dataSetName $dataSetInfoDescription");
	bioXASDefaultXAS->setColumnHeaderIncluded(true);
	bioXASDefaultXAS->setColumnHeaderDelimiter("");
	bioXASDefaultXAS->setSectionHeader("");
	bioXASDefaultXAS->setSectionHeaderIncluded(true);
	bioXASDefaultXAS->setIncludeAllDataSources(true);
	bioXASDefaultXAS->setFirstColumnOnly(true);
	bioXASDefaultXAS->setIncludeHigherDimensionSources(true);
	bioXASDefaultXAS->setSeparateHigherDimensionalSources(true);
	bioXASDefaultXAS->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
	bioXASDefaultXAS->setHigherDimensionsInRows(true);
	bioXASDefaultXAS->storeToDb(AMDatabase::database("user"));

	/*
	if(bioXASDefaultXAS->id() > 0)
			AMAppControllerSupport::registerClass<IDEASXASScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(bioXASDefaultXAS->id());
	*/

}

void BioXASImagingAppController::setupUserConfiguration()
{

}

void BioXASImagingAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("General", 0);

	mw_->insertHeading("Components", 1);

	BioXASCarbonFilterFarmView *carbonFilterFarmView = new BioXASCarbonFilterFarmView(BioXASImagingBeamline::bioXAS()->carbonFilterFarm());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Carbon Filter Farm", ":/system-software-update.png", carbonFilterFarmView), "Components", "Carbon Filter Farm", ":/system-software-update.png");

	mw_->insertHeading("Detectors", 2);

	mw_->insertHeading("Scans", 3);

}

void BioXASImagingAppController::makeConnections()
{
}

void BioXASImagingAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}

void BioXASImagingAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}
