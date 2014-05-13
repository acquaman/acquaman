#include "BioXASSideAppController.h"

#include "beamline/BioXAS/BioXASSideBeamline.h"

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

#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "ui/BioXAS/BioXASSidePersistentView.h"

BioXASSideAppController::BioXASSideAppController(QObject *parent)
	: AMAppController(parent)
{
    scalerView_ = 0;
    persistentPanel_ = 0;
}

bool BioXASSideAppController::startup()
{
	getUserDataFolderFromDialog();

	// Start up the main program.
	if(AMAppController::startup()) {


		// Initialize central beamline object
		BioXASSideBeamline::bioXAS();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

			AMRun firstRun("BioXASSide", 6);	/// \todo For now, we know that 6 is the ID of the BioXAS Side endstation facility, but this is a hardcoded hack.
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

void BioXASSideAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void BioXASSideAppController::onScalerConnected()
{
    if (BioXASSideBeamline::bioXAS()->scaler()->isConnected() && !scalerView_) {
        scalerView_ = new CLSSIS3820ScalerView(BioXASSideBeamline::bioXAS()->scaler());

        mw_->addPane(scalerView_, "Detectors", "Scaler", ":/system-search.png", true);
    }
}

void BioXASSideAppController::onBeamlineConnected()
{
    if (BioXASSideBeamline::bioXAS()->isConnected() && !persistentPanel_) {
        persistentPanel_ = new BioXASSidePersistentView();
        mw_->addRightWidget(persistentPanel_);
    }

}

void BioXASSideAppController::registerClasses()
{

}

void BioXASSideAppController::setupExporterOptions()
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

void BioXASSideAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("General", 0);

	mw_->insertHeading("Detectors", 1);

    connect( BioXASSideBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()) );

    if (BioXASSideBeamline::bioXAS()->scaler()->isConnected()) {
        onScalerConnected();
    }

	mw_->insertHeading("Scans", 2);

    connect( BioXASSideBeamline::bioXAS(), SIGNAL(connected(bool)), this, SLOT(onBeamlineConnected()) );

    if (BioXASSideBeamline::bioXAS()->isConnected()) {
        onBeamlineConnected();
    }

}

void BioXASSideAppController::makeConnections()
{
}

void BioXASSideAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}

void BioXASSideAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}