#include "BioXASAppController.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"

#include <QDebug>

BioXASAppController::BioXASAppController(QObject *parent) :
    AMAppController(parent)
{	
	// Initialize controller settings.

	userConfiguration_ = new BioXASUserConfiguration(this);
	setDefaultUseLocalStorage(true);

	// Initialize member variables.

	commissioningConfiguration_ = 0;
	commissioningConfigurationView_ = 0;
	commissioningConfigurationViewHolder_ = 0;

	monoCalibrationConfiguration_ = 0;
	monoCalibrationConfigurationView_ = 0;
	monoCalibrationConfigurationViewHolder_ = 0;

	energyCalibrationView_ = 0;
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

void BioXASAppController::onUserConfigurationLoadedFromDb()
{
	if (userConfiguration_) {

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

void BioXASAppController::goToEnergyCalibrationView(AMScan *toView)
{
	qDebug() << "Going to energy calibration view...";

	if (energyCalibrationView_) {
		energyCalibrationView_->setCurrentScan(toView);
		mw_->setCurrentPane(energyCalibrationView_);
	}
}

void BioXASAppController::goToEnergyCalibrationScanConfigurationView()
{
	if (monoCalibrationConfigurationViewHolder_)
		mw_->setCurrentPane(monoCalibrationConfigurationViewHolder_);
}

void BioXASAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)

	// Save current configuration to the database.

	if (userConfiguration_)
		userConfiguration_->storeToDb(AMDatabase::database("user"));

	// Testing - energy calibration view.

	qDebug() << "\nScan started.";

//	if (action) {
//		AMScanActionInfo *info = qobject_cast<AMScanActionInfo*>(action->info());

//		if (info) {
//			BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *config = qobject_cast<BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration*>(info->configuration());

//			if (config) {
//				qDebug() << "It is a mono calibration scan.\n";

//				goToEnergyCalibrationView(0);
//			}
//		}
//	}
}

void BioXASAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	// Save current configuration to the database.

	if (userConfiguration_)
		userConfiguration_->storeToDb(AMDatabase::database("user"));

//	// If the scan was an energy calibration scan, set the view's scan and make it the current pane.

//	if (action && action->controller() && action->controller()->scan()) {

//		AMScan *scan = action->controller()->scan();

//		if (scan) {
//			BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *energyCalibrationConfiguration = qobject_cast<BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration*>(action->controller()->scan()->scanConfiguration());

//			if (energyCalibrationConfiguration) {
//				qDebug() << "It was a mono calibration scan.\n";
//				goToEnergyCalibrationView(scan);

//			} else {
//				qDebug() << "It was NOT a mono calibration scan.\n";
//			}
//		}
//	}
}

void BioXASAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo>();
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASXASScanConfiguration>();
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
	mw_->insertHeading("Calibration", 3);

	// Create beamline component views:
	////////////////////////////////////

	addComponentView(BioXASBeamline::bioXAS()->m1Mirror());
	addComponentView(BioXASBeamline::bioXAS()->mono());
	addComponentView(BioXASBeamline::bioXAS()->m2Mirror());
	addComponentView(BioXASBeamline::bioXAS()->carbonFilterFarm());
	addComponentView(BioXASBeamline::bioXAS()->jjSlits());
	addComponentView(BioXASBeamline::bioXAS()->xiaFilters());
	addComponentView(BioXASBeamline::bioXAS()->dbhrMirrors());
	addComponentView(BioXASBeamline::bioXAS()->standardsWheel());
	addComponentView(BioXASBeamline::bioXAS()->endstationTable());


	AMMotorGroup *cryostatStageMotors = BioXASBeamline::bioXAS()->cryostatStageMotors();
	if (cryostatStageMotors) {
		AMMotorGroupView *cryostatStageView = new AMMotorGroupView(cryostatStageMotors);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Cryostat Stage", ":/system-software-update.png", cryostatStageView), "General", "Cryostat Stage", ":/system-software-update.png");
	}

	addDetectorView(BioXASBeamline::bioXAS()->scaler());
	addDetectorView(BioXASBeamline::bioXAS()->ge32ElementDetector());
	addDetectorView(BioXASBeamline::bioXAS()->fourElementVortexDetector());

	// Create scan views:
	////////////////////////////////////

	xasConfiguration_ = new BioXASXASScanConfiguration();
	setupXASScanConfiguration(xasConfiguration_);
	addXASScanConfigurationView(xasConfiguration_);

	commissioningConfiguration_ = new AMGenericStepScanConfiguration;
	setupGenericStepScanConfiguration(commissioningConfiguration_);
	addCommissioningScanConfigurationView(commissioningConfiguration_);

	monoCalibrationConfiguration_ = new BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration();
	setupXASScanConfiguration(monoCalibrationConfiguration_);
	addXASScanConfigurationView(monoCalibrationConfiguration_);

	// Create calibration views:
	////////////////////////////////////

	addCalibrationView(BioXASBeamline::bioXAS()->mono());
}

void BioXASAppController::addComponentView(QObject *component)
{
	if (component) {
		QWidget *componentView = 0;
		QString componentName;
		bool componentFound = false;

		// Try to match up given component with known component types.
		// If match found, create appropriate view.

		BioXASM1Mirror *m1Mirror = qobject_cast<BioXASM1Mirror*>(component);
		if (!componentFound && m1Mirror) {
			componentView = new BioXASM1MirrorView(m1Mirror);
			componentName = "M1 Mirror";
			componentFound = true;
		}

		BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(component);
		if (!componentFound && mono) {
			componentView = new BioXASSSRLMonochromatorConfigurationView(mono);
			componentName = "Monochromator";
			componentFound = true;
		}

		BioXASM2Mirror *m2Mirror = qobject_cast<BioXASM2Mirror*>(component);
		if (!componentFound && m2Mirror) {
			componentView = new BioXASM2MirrorView(m2Mirror);
			componentName = "M2 Mirror";
			componentFound = true;
		}

		BioXASCarbonFilterFarm *carbonFilterFarm = qobject_cast<BioXASCarbonFilterFarm*>(component);
		if (!componentFound && carbonFilterFarm) {
			componentView = new BioXASCarbonFilterFarmView(carbonFilterFarm);
			componentName = "Carbon Filter Farm";
			componentFound = true;
		}

		CLSJJSlits *jjSlits = qobject_cast<CLSJJSlits*>(component);
		if (!componentFound && jjSlits) {
			componentView = new CLSJJSlitsView(jjSlits);
			componentName = "JJ Slits";
			componentFound = true;
		}

		BioXASXIAFilters *xiaFilters = qobject_cast<BioXASXIAFilters*>(component);
		if (!componentFound && xiaFilters) {
			componentView = new BioXASXIAFiltersView(xiaFilters);
			componentName = "XIA Filters";
			componentFound = true;
		}

		BioXASDBHRMirrors *dbhrMirrors = qobject_cast<BioXASDBHRMirrors*>(component);
		if (!componentFound && dbhrMirrors) {
			componentView = new BioXASDBHRMirrorsView(dbhrMirrors);
			componentName = "DBHR Mirrors";
			componentFound = true;
		}

		CLSStandardsWheel *standardsWheel = qobject_cast<CLSStandardsWheel*>(component);
		if (!componentFound && standardsWheel) {
			componentView = new CLSStandardsWheelConfigurationView(standardsWheel);
			componentName = "Standards Wheel";
			componentFound = true;
		}

		BioXASEndstationTable *table = qobject_cast<BioXASEndstationTable*>(component);
		if (!componentFound && table) {
			componentView = new BioXASEndstationTableView(table);
			componentName = "Endstation Table";
			componentFound = true;
		}

		// If a view was created, add it to the 'General' pane.

		if (componentFound) {
			mw_->addPane(AMMainWindow::buildMainWindowPane(componentName, ":/system-software-update.png", componentView), "General", componentName, ":/system-software-update.png");
		}
	}
}

void BioXASAppController::addDetectorView(QObject *detector)
{
	if (detector) {
		QWidget *detectorView = 0;
		QString detectorName;
		bool detectorFound = false;

		// Try to match up given detector with known detector types.
		// If match found, create appropriate view.

		CLSSIS3820Scaler *scaler = qobject_cast<CLSSIS3820Scaler*>(detector);
		if (!detectorFound && scaler) {
			detectorView = new BioXASSIS3820ScalerView(scaler, true);
			detectorName = "Scaler";
			detectorFound = true;
		}

		BioXAS32ElementGeDetector *ge32Detector = qobject_cast<BioXAS32ElementGeDetector*>(detector);
		if (!detectorFound && ge32Detector) {
			BioXAS32ElementGeDetectorView *view = new BioXAS32ElementGeDetectorView(ge32Detector);
			view->buildDetectorView();
			view->addEmissionLineNameFilter(QRegExp("1"));
			view->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));

			detectorView = view;
			detectorName = "Ge 32-el";
			detectorFound = true;
		}

		BioXASFourElementVortexDetector *vortex4Detector = qobject_cast<BioXASFourElementVortexDetector*>(detector);
		if (!detectorFound && vortex4Detector) {
			BioXASFourElementVortexDetectorView *view = new BioXASFourElementVortexDetectorView(vortex4Detector);
			view->buildDetectorView();
			view->setEnergyRange(3000, 28000);
			view->addEmissionLineNameFilter(QRegExp("1"));
			view->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));

			detectorView = view;
			detectorName = "4-element";
			detectorFound = true;
		}

		// If a view was created, add it to the 'Detectors' pane.

		if (detectorFound) {
			mw_->addPane(AMMainWindow::buildMainWindowPane(detectorName, ":/utilities-system-monitor.png", detectorView), "Detectors", detectorName, ":/utilities-system-monitor.png");
		}
	}
}

void BioXASAppController::addXASScanConfigurationView(BioXASXASScanConfiguration *configuration)
{
	if (configuration) {
		QString scanName;
		bool scanFound = false;

		BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *energyCalibration = qobject_cast<BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration*>(configuration);
		if (!scanFound && energyCalibration) {
			scanName = "Energy calibration";
			scanFound = true;
		}

		BioXASXASScanConfigurationView *configurationView = new BioXASXASScanConfigurationView(configuration);
		AMScanConfigurationViewHolder3 *configurationViewHolder = new AMScanConfigurationViewHolder3(scanName, true, true, configurationView);

		connect(configuration, SIGNAL(totalTimeChanged(double)), configurationViewHolder, SLOT(updateOverallScanTime(double)));
		configurationViewHolder->updateOverallScanTime(configuration->totalTime());

		mw_->addPane(configurationViewHolder, "Scans", scanName, ":/utilities-system-monitor.png");
	}
}

void BioXASAppController::addCommissioningScanConfigurationView(AMGenericStepScanConfiguration *configuration)
{
	if (configuration) {

		QString scanName = "Commissioning Tool";
		QString iconName = ":/utilities-system-monitor.png";

		AMGenericStepScanConfigurationView *view = new AMGenericStepScanConfigurationView(configuration);
		AMScanConfigurationViewHolder3 *viewHolder = new AMScanConfigurationViewHolder3(scanName, true, true, view, iconName);

		connect( configuration, SIGNAL(totalTimeChanged(double)), viewHolder, SLOT(updateOverallScanTime(double)) );
		viewHolder->updateOverallScanTime(configuration->totalTime());

		mw_->addPane(new AMScanConfigurationViewHolder3(scanName, true, true, new AMGenericStepScanConfigurationView(configuration), iconName), "Scans", scanName, iconName);
	}
}

void BioXASAppController::addCalibrationView(QObject *toCalibrate)
{
	if (toCalibrate) {
		QWidget *calibrationView = 0;
		QString calibrationName;
		bool calibrationItemFound = false;

		BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(toCalibrate);
		if (!calibrationItemFound && mono) {
			calibrationView = new BioXASSSRLMonochromatorEnergyCalibrationView(mono);
			calibrationName = "Energy";
			calibrationItemFound = true;

			connect( calibrationView, SIGNAL(energyCalibrationScanRequested()), this, SLOT(goToEnergyCalibrationScanConfigurationView()) );
		}

		if (calibrationItemFound)
			mw_->addPane(AMMainWindow::buildMainWindowPane(calibrationName, ":/system-search.png", calibrationView), "Calibration", calibrationName, ":system-search.png");
	}
}

void BioXASAppController::addPersistentView(QWidget *persistentView)
{
	if (persistentView) {
		persistentView->setFixedWidth(400);
		mw_->addRightWidget(persistentView);
	}
}

void BioXASAppController::setupXASScanConfiguration(BioXASXASScanConfiguration *configuration)
{
	if (configuration) {

		// Set the energy.

		configuration->setEnergy(1000);

		// Set the energy as the scanned control.

		BioXASMonochromator *mono = BioXASBeamline::bioXAS()->mono();
		if (mono) {
			AMControl *energyControl = mono->energyControl();
			if (energyControl)
				configuration->setControl(energyControl->toInfo());
		}

		// Set scan detectors.

		AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();
		if (i0Detector)
			configuration->addDetector(i0Detector->toInfo());

		AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();
		if (i1Detector)
			configuration->addDetector(i1Detector->toInfo());

		AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();
		if (i2Detector)
			configuration->addDetector(i2Detector->toInfo());

		AMDetector *scalerDwellTimeDetector = BioXASBeamline::bioXAS()->scalerDwellTimeDetector();
		if (scalerDwellTimeDetector)
			configuration->addDetector(scalerDwellTimeDetector->toInfo());

		AMDetector *vortexDetector = BioXASBeamline::bioXAS()->fourElementVortexDetector();
		if (vortexDetector && configuration->usingXRFDetector())
			configuration->addDetector(vortexDetector->toInfo());

		AMDetector *ge32Detector = BioXASBeamline::bioXAS()->ge32ElementDetector();
		if (ge32Detector && configuration->usingXRFDetector())
			configuration->addDetector(ge32Detector->toInfo());
	}
}

void BioXASAppController::setupGenericStepScanConfiguration(AMGenericStepScanConfiguration *configuration)
{
	if (configuration) {

		configuration->setAutoExportEnabled(false);

		// Set scan detectors.

		AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();
		if (i0Detector)
			configuration->addDetector(i0Detector->toInfo());
	}
}
