#include "BioXASImagingAppController.h"

#include "beamline/BioXAS/BioXASImagingBeamline.h"

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

BioXASImagingAppController::BioXASImagingAppController(QObject *parent)
	: AMAppController(parent)
{
}

bool BioXASImagingAppController::startup()
{
	getUserDataFolderFromDialog();

	// Start up the main program.
	if(AMAppController::startup()) {


		// Initialize central beamline object
		BioXASImagingBeamline::bioXAS();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

			AMRun firstRun("BioXASImaging", 8);	/// \todo For now, we know that 8 is the ID of the BioXAS imaging endstation facility, but this is a hardcoded hack.
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

void BioXASImagingAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
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

	bioXASDefaultXAS->setName("IDEAS Default XAS");
	bioXASDefaultXAS->setFileName("$name_$fsIndex.dat");
	bioXASDefaultXAS->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
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

//	if(bioXASDefaultXAS->id() > 0)
//			AMAppControllerSupport::registerClass<IDEASXASScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(bioXASDefaultXAS->id());

}

void BioXASImagingAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("General", 0);

	mw_->insertHeading("Detectors", 1);

	mw_->insertHeading("Scans", 2);

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