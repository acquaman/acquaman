#include "AMMockAppController.h"

#include "tests/mocks/beamline/AMMockBeamline.h"
#include "tests/mocks/ui/AMMockPersistentView.h"

#include "beamline/CLS/CLSStorageRing.h"
#include "dataman/AMRun.h"
#include "ui/util/AMChooseDataFolderDialog.h"
#include "ui/AMMainWindow.h"

AMMockAppController::AMMockAppController(QObject *parent) :
    AMAppController(parent)
{
}

bool AMMockAppController::startup()
{
	bool dataFolderRetrieved = AMChooseDataFolderDialog::getDataFolder(
	            "/AcquamanLocalData/",
	            "/home/acquaman/AcquamanData/",
	            "users",
	            QStringList());

	if(!dataFolderRetrieved) {
		return false;
	}


	if(!AMAppController::startup()) {
		return false;
	}

	// Create the mock beamline
	AMMockBeamline::mockbl();

	// Initialize the storage ring
	CLSStorageRing::sr1();

	// Retrieve the current run, or create one if there isn't one
	AMRun existingRun;
	if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
		// No run found, create one
		AMRun firstRun("Mock Beamline", 0);
		firstRun.storeToDb(AMDatabase::database("user"));

	}

	registerClasses();
	setupExporterOptions();
	setupUserInterface();
	makeConnections();

	return true;
}

void AMMockAppController::shutdown()
{
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void AMMockAppController::registerClasses()
{
}

void AMMockAppController::setupOnFirstRun()
{
}

void AMMockAppController::setupExporterOptions()
{
}

void AMMockAppController::setupUserInterface()
{

	AMMockPersistentView* persistentView = new AMMockPersistentView(AMMockBeamline::mockbl()->exposedControls());
	mw_->addRightWidget(persistentView);
}

void AMMockAppController::makeConnections()
{
}
