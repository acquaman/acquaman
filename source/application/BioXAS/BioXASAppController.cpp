#include "BioXASAppController.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"

BioXASAppController::BioXASAppController(QObject *parent) :
    AMAppController(parent)
{	
	userConfiguration_ = new BioXASUserConfiguration(this);

	setDefaultUseLocalStorage(true);
}

BioXASAppController::~BioXASAppController()
{

}

bool BioXASAppController::startup()
{
	bool result = false;

	// Get a destination folder.
	if ( !AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-m/AcquamanMainData", "/home/bioxas-m/AcquamanMainData", "users", QStringList()) )
		return result;

	// Start up the main program.
	if(AMAppController::startup()) {

		initializeBeamline();

		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		setupExporterOptions();
		setupUserInterface();
		makeConnections();
		applyCurrentSettings();

		if (userConfiguration_) {
			bool loaded = userConfiguration_->loadFromDb(AMDatabase::database("user"), 1);

			if (!loaded) {
				userConfiguration_->storeToDb(AMDatabase::database("user"));
				connect( userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()) );
				onUserConfigurationLoadedFromDb();
			}
		}

		result = true;
	}

	return result;
}

void BioXASAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void BioXASAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo>();
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASXRFScanConfiguration>();
}

void BioXASAppController::setupExporterOptions()
{

}

void BioXASAppController::initializeStorageRing()
{
	CLSStorageRing::sr1();
}

void BioXASAppController::initializeBeamline()
{
	BioXASBeamline::bioXAS();
}

void BioXASAppController::setupUserInterface()
{

}

void BioXASAppController::makeConnections()
{

}

void BioXASAppController::applyCurrentSettings()
{

}

void BioXASAppController::onUserConfigurationLoadedFromDb()
{
	if (userConfiguration_) {
		AMXRFDetector *vortexDetector = BioXASBeamline::bioXAS()->fourElementVortexDetector();

		if (vortexDetector) {

			// Iterate through regions in the user configuration, adding them to the detector's and configuration's ROIs.
			foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
				AMRegionOfInterest *newRegion = region->createCopy();
				vortexDetector->addRegionOfInterest(newRegion);
				onRegionOfInterestAdded(region);
			}

			// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
			// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
			connect(vortexDetector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
			connect(vortexDetector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
		}

//		BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();

//		if (geDetector) {

//			// Iterate through regions in the user configuration, adding them to the detector's and configuration's ROIs.
//			foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
//				AMRegionOfInterest *newRegion = region->createCopy();
//				geDetector->addRegionOfInterest(newRegion);
//				configuration_->addRegionOfInterest(region);
//			}

//			// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup./*
//			connect(geDetector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
//			connect(geDetector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));*/
//		}
	}
}

void BioXASAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)

	// Save current configuration to the database.

	if (userConfiguration_)
		userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void BioXASAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)

	if (userConfiguration_)
		userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void BioXASAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	if (userConfiguration_ && !userConfiguration_->regionsOfInterest().contains(region))
		userConfiguration_->addRegionOfInterest(region);
}

void BioXASAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	if (userConfiguration_ && userConfiguration_->regionsOfInterest().contains(region))
		userConfiguration_->removeRegionOfInterest(region);
}
