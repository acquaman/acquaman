#include "AMMockAppController.h"

#include <QDateTime>

#include "tests/mocks/beamline/AMMockBeamline.h"
#include "tests/mocks/ui/AMMockPersistentView.h"

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "ui/util/AMChooseDataFolderDialog.h"
#include "ui/beamline/AMDetectorView.h"
#include "ui/AMMainWindow.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"

AMMockAppController::AMMockAppController(QObject *parent) :
    AMAppController(parent)
{
	clsFacility_ = AMFacility("MockBeamline", QString("CLS %1 Beamline").arg("MockBeamline"), ":/clsIcon.png");
}

bool AMMockAppController::startup()
{
	// We use random number extensively in the mock application, set it here
	// once with the current msecs since 1970-01-01T00:00:00.000
	srand(quint64(QDateTime::currentMSecsSinceEpoch()));

	bool dataFolderRetrieved = AMChooseDataFolderDialog::getDataFolder(
				"/AcquamanLocalData",
				"/home/acquaman/AcquamanData",
				"MockBeamline/users",
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

	// Setup generic scan configuration
	genericStepScanConfiguration_ = new AMGenericStepScanConfiguration();
	genericStepScanConfiguration_->setAutoExportEnabled(false);


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

	mw_->insertHeading("Detectors", 0);

	foreach(AMDetector* detector, AMMockBeamline::mockbl()->exposedDetectors()->toList()) {
		AMDetectorGeneralDetailedView* detectorView = new AMDetectorGeneralDetailedView(detector);
		mw_->addPane(detectorView, "Detectors", detector->name(), ":/system-software-update.png");
	}


	genericStepScanView_ = new AMGenericStepScanConfigurationView(genericStepScanConfiguration_,
	                                                              AMMockBeamline::mockbl()->exposedControls(),
	                                                              AMMockBeamline::mockbl()->exposedDetectors());

	genericScanViewHolder_ = new AMScanConfigurationViewHolder3("Step Scan",
	                                                            false,
	                                                            true,
	                                                            genericStepScanView_);

	mw_->insertHeading("Scans", 1);
	mw_->addPane(genericScanViewHolder_, "Scans", "Step Scan", ":/utilities-system-monitor.png");

	mw_->expandAllHeadings();
}

void AMMockAppController::makeConnections()
{
}
