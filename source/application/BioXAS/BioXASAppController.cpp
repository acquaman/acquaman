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
	bool dataFolderOK = false;

	// Get a destination folder.
	dataFolderOK = setupDataFolder();

	// Start up the main program.
	if (dataFolderOK && AMAppController::startup()) {

		// Initialize singleton objects.

		initializeStorageRing();
		initializeBeamline();
		initializePeriodicTable();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		setupExporterOptions();
		setupUserInterface();

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
	AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", "", "", true);

	if (bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
}

void BioXASAppController::initializeStorageRing()
{
	CLSStorageRing::sr1();
}

void BioXASAppController::initializeBeamline()
{
	BioXASBeamline::bioXAS();
}

void BioXASAppController::initializePeriodicTable()
{
	AMPeriodicTable::table();
}

void BioXASAppController::setupUserInterface()
{
	mw_->setWindowTitle("Acquaman - BioXAS");

	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("General", 0);
	mw_->insertHeading("Detectors", 1);
	mw_->insertHeading("Scans", 2);

	// Create beamline component widgets:
	////////////////////////////////////

	BioXASEndstationTable *table = BioXASBeamline::bioXAS()->endstationTable();
	if (table) {
		BioXASEndstationTableView *endstationTableView = new BioXASEndstationTableView(table);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Endstation Table", ":/system-software-update.png", endstationTableView), "General", "Endstation Table", ":/system-software-update.png");
	}

	BioXASCarbonFilterFarm *filterFarm = BioXASBeamline::bioXAS()->carbonFilterFarm();
	if (filterFarm) {
		BioXASCarbonFilterFarmView *carbonFilterFarmView = new BioXASCarbonFilterFarmView(filterFarm);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Carbon Filter Farm", ":/system-software-update.png", carbonFilterFarmView), "General", "Carbon filter farm", ":/system-software-update.png");
	}

	BioXASM1Mirror *m1Mirror = BioXASBeamline::bioXAS()->m1Mirror();
	if (m1Mirror) {
		BioXASM1MirrorView *m1MirrorView = new BioXASM1MirrorView(m1Mirror);
		mw_->addPane(AMMainWindow::buildMainWindowPane("M1 Mirror", ":/system-software-update.png", m1MirrorView), "General", "M1 Mirror", ":/system-software-update.png");
	}

	BioXASSSRLMonochromator *mono = BioXASBeamline::bioXAS()->mono();
	if (mono) {
		BioXASSSRLMonochromatorConfigurationView *monoConfigView = new BioXASSSRLMonochromatorConfigurationView(mono);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Monochromator", ":/system-software-update.png", monoConfigView), "General", "Monochromator", ":/system-software-update.png");
	}

	CLSJJSlits *jjSlits = BioXASBeamline::bioXAS()->jjSlits();
	if (jjSlits) {
		CLSJJSlitsView *jjSlitsView = new CLSJJSlitsView(jjSlits);
		mw_->addPane(AMMainWindow::buildMainWindowPane("JJ Slits", ":/system-software-update.png", jjSlitsView), "General", "JJ Slits", ":/system-software-update.png");
	}

	BioXASXIAFilters *xiaFilters = BioXASBeamline::bioXAS()->xiaFilters();
	if (xiaFilters) {
		BioXASXIAFiltersView *xiaFiltersView = new BioXASXIAFiltersView(xiaFilters);
		mw_->addPane(AMMainWindow::buildMainWindowPane("XIA Filters", ":/system-software-update.png", xiaFiltersView), "General", "XIA Filters", ":/system-software-update.png");
	}

	BioXASM2Mirror *m2Mirror = BioXASBeamline::bioXAS()->m2Mirror();
	if (m2Mirror) {
		BioXASM2MirrorView *m2MirrorView = new BioXASM2MirrorView(m2Mirror);
		mw_->addPane(AMMainWindow::buildMainWindowPane("M2 Mirror", ":/system-software-update.png", m2MirrorView), "General", "M2 Mirror", ":/system-software-update.png");
	}

	BioXASDBHRMirror *dbhrMirrors = BioXASBeamline::bioXAS()->dbhrMirrors();
	if (dbhrMirrors) {
		 BioXASDBHRMirrorView *dbhrView = new BioXASDBHRMirrorView(dbhrMirrors);
		mw_->addPane(AMMainWindow::buildMainWindowPane("DBHR Mirrors", ":/system-software-update.png", dbhrView), "General", "DBHR Mirrors", ":/system-software-update.png");
	}

	CLSStandardsWheel *standardsWheel = BioXASBeamline::bioXAS()->standardsWheel();
	if (standardsWheel) {
		CLSStandardsWheelConfigurationView *wheelView = new CLSStandardsWheelConfigurationView(standardsWheel);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Standards Wheel", ":/system-software-update.png", wheelView), "General", "Standards Wheel", ":/system-software-update.png");
	}

	AMMotorGroup *cryostatStageMotors = BioXASBeamline::bioXAS()->cryostatStageMotors();
	if (cryostatStageMotors) {
		AMMotorGroupView *cryostatStageView = new AMMotorGroupView(cryostatStageMotors);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Cryostat Stage", ":/system-software-update.png", cryostatStageView), "General", "Cryostat Stage", ":/system-software-update.png");
	}

	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();
	if (scaler) {
		BioXASSIS3820ScalerView *scalerView = new BioXASSIS3820ScalerView(scaler, true);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Scaler", ":/system-search.png", scalerView), "Detectors", "Scaler", ":/system-search.png");
	}

	BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();
	if (geDetector) {
		BioXAS32ElementGeDetectorView *geDetectorView = new BioXAS32ElementGeDetectorView(geDetector);
		geDetectorView->buildDetectorView();
		geDetectorView->addEmissionLineNameFilter(QRegExp("1"));
		geDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
		mw_->addPane(geDetectorView, "Detectors", "Ge 32-el", ":/system-search.png");
	}

	BioXASFourElementVortexDetector *fourElementDetector = BioXASBeamline::bioXAS()->fourElementVortexDetector();
	if (fourElementDetector) {
		BioXASFourElementVortexDetectorView *fourElementDetectorView = new BioXASFourElementVortexDetectorView(fourElementDetector);
		fourElementDetectorView->buildDetectorView();
		fourElementDetectorView->setEnergyRange(3000, 28000);
		fourElementDetectorView->addEmissionLineNameFilter(QRegExp("1"));
		fourElementDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
		mw_->addPane(fourElementDetectorView, "Detectors", "4-element", ":/system-search.png");
	}
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

		BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();
		if (geDetector) {
			foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
				AMRegionOfInterest *newRegion = region->createCopy();
				geDetector->addRegionOfInterest(newRegion);
				onRegionOfInterestAdded(region);
			}

			connect(geDetector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
			connect(geDetector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
		}
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
