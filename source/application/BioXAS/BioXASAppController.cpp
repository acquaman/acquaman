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

	xasConfiguration_ = 0;
	commissioningConfiguration_ = 0;
	energyCalibrationConfiguration_ = 0;

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

	if (xasConfiguration_)
		xasConfiguration_->addRegionOfInterest(region);
}

void BioXASAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	if (userConfiguration_ && userConfiguration_->regionsOfInterest().contains(region))
		userConfiguration_->removeRegionOfInterest(region);

	if (xasConfiguration_)
		xasConfiguration_->removeRegionOfInterest(region);
}

void BioXASAppController::goToEnergyCalibrationScanConfigurationView()
{
	if (energyCalibrationView_)
		mw_->setCurrentPane(energyCalibrationView_);
}

void BioXASAppController::goToEnergyCalibrationView(AMScan *toView)
{
	qDebug() << "Going to energy calibration view...";

	if (energyCalibrationView_) {
		energyCalibrationView_->setCurrentScan(toView);
		mw_->setCurrentPane(energyCalibrationView_);
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
	// Save current configuration to the database.

	if (userConfiguration_)
		userConfiguration_->storeToDb(AMDatabase::database("user"));

	// If the scan was an energy calibration scan, set the calibration view's scan and make it the current pane.

	if (action && action->controller() && action->controller()->scan()) {

		AMScan *scan = action->controller()->scan();

		if (scan) {
			BioXASXASScanConfiguration *xasScanConfiguration = qobject_cast<BioXASXASScanConfiguration*>(action->controller()->scan()->scanConfiguration());

			if (xasScanConfiguration && xasScanConfiguration->name() == "Energy Calibration XAS Scan") {
				qDebug() << "It was a mono calibration scan.\n";
				goToEnergyCalibrationView(scan);

			} else {
				qDebug() << "It was NOT a mono calibration scan.\n";
			}
		}
	}
}

void BioXASAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo>();
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASXASScanConfiguration>();
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

	addComponentView(BioXASBeamline::bioXAS()->m1Mirror(), "M1 Mirror");
	addComponentView(BioXASBeamline::bioXAS()->mono(), "Monochromator");
	addComponentView(BioXASBeamline::bioXAS()->m2Mirror(), "M2 Mirror");
	addComponentView(BioXASBeamline::bioXAS()->carbonFilterFarm(), "Carbon Filter Farm");
	addComponentView(BioXASBeamline::bioXAS()->jjSlits(), "JJ Slits");
	addComponentView(BioXASBeamline::bioXAS()->xiaFilters(), "XIA Filters");
	addComponentView(BioXASBeamline::bioXAS()->dbhrMirrors(), "DBHR Mirrors");
	addComponentView(BioXASBeamline::bioXAS()->standardsWheel(), "Standards Wheel");
	addComponentView(BioXASBeamline::bioXAS()->cryostatStage(), "Cryostat Stage");
	addComponentView(BioXASBeamline::bioXAS()->endstationTable(), "Endstation Table");

	addDetectorView(BioXASBeamline::bioXAS()->scaler(), "Scaler");
	addDetectorView(BioXASBeamline::bioXAS()->ge32ElementDetector(), "Ge 32-el");
	addDetectorView(BioXASBeamline::bioXAS()->fourElementVortexDetector(), "Four element");

	// Create scan views:
	////////////////////////////////////

	xasConfiguration_ = new BioXASXASScanConfiguration();
	setupXASScanConfiguration(xasConfiguration_);
	xasConfigurationView_ = createScanConfigurationViewWithHolder(xasConfiguration_);
	addViewToScansPane(xasConfigurationView_, "XAS Scan");

	commissioningConfiguration_ = new AMGenericStepScanConfiguration;
	setupGenericStepScanConfiguration(commissioningConfiguration_);
	commissioningConfigurationView_ = createScanConfigurationViewWithHolder(commissioningConfiguration_);
	addViewToScansPane(commissioningConfigurationView_, "Commissioning Tool");

	energyCalibrationConfiguration_ = new BioXASXASScanConfiguration();
	energyCalibrationConfiguration_->setName("Energy Calibration XAS Scan");
	energyCalibrationConfiguration_->setUserScanName("Energy Calibration XAS Scan");
	setupXASScanConfiguration(energyCalibrationConfiguration_);
	energyCalibrationConfigurationView_ = createScanConfigurationViewWithHolder(energyCalibrationConfiguration_);
	addViewToScansPane(energyCalibrationConfigurationView_, "Energy Calibration");

	// Create calibration views:
	////////////////////////////////////

	energyCalibrationView_ = qobject_cast<BioXASSSRLMonochromatorEnergyCalibrationView*>(createCalibrationView(BioXASBeamline::bioXAS()->mono()));
	addViewToCalibrationPane(energyCalibrationView_, "Energy");
}

void BioXASAppController::addViewToGeneralPane(QWidget *view, const QString &viewName)
{
	if (view)
		mw_->addPane(AMMainWindow::buildMainWindowPane(viewName, ":/system-software-update.png", view), "General", viewName, ":/system-software-update.png");
}

void BioXASAppController::addViewToDetectorsPane(QWidget *view, const QString &viewName)
{
	if (view) {
		BioXAS32ElementGeDetectorView *ge32View = qobject_cast<BioXAS32ElementGeDetectorView*>(view);
		if (ge32View)
			mw_->addPane(view, "Detectors", viewName, ":/utilities-system-monitor.png");
		else
			mw_->addPane(AMMainWindow::buildMainWindowPane(viewName, ":/utilities-system-monitor.png", view), "Detectors", viewName, ":/utilities-system-monitor.png");
	}
}

void BioXASAppController::addViewToScansPane(QWidget *view, const QString &viewName)
{
	if (view)
		mw_->addPane(AMMainWindow::buildMainWindowPane(viewName, ":/utilities-system-monitor.png", view), "Scans", viewName, ":/utilities-system-monitor.png");
}

void BioXASAppController::addViewToCalibrationPane(QWidget *view, const QString &viewName)
{
	if (view)
		mw_->addPane(AMMainWindow::buildMainWindowPane(viewName, ":system-search.png", view), "Calibration", viewName, ":system-search.png");
}

QWidget* BioXASAppController::createComponentView(QObject *component)
{
	QWidget *componentView = 0;

	if (component) {
		bool componentFound = false;

		// Try to match up given component with known component types.
		// If match found, create appropriate view.

		BioXASM1Mirror *m1Mirror = qobject_cast<BioXASM1Mirror*>(component);
		if (!componentFound && m1Mirror) {
			componentView = new BioXASM1MirrorView(m1Mirror);
			componentFound = true;
		}

		BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(component);
		if (!componentFound && mono) {
			componentView = new BioXASSSRLMonochromatorConfigurationView(mono);
			componentFound = true;
		}

		BioXASM2Mirror *m2Mirror = qobject_cast<BioXASM2Mirror*>(component);
		if (!componentFound && m2Mirror) {
			componentView = new BioXASM2MirrorView(m2Mirror);
			componentFound = true;
		}

		BioXASCarbonFilterFarm *carbonFilterFarm = qobject_cast<BioXASCarbonFilterFarm*>(component);
		if (!componentFound && carbonFilterFarm) {
			componentView = new BioXASCarbonFilterFarmView(carbonFilterFarm);
			componentFound = true;
		}

		CLSJJSlits *jjSlits = qobject_cast<CLSJJSlits*>(component);
		if (!componentFound && jjSlits) {
			componentView = new CLSJJSlitsView(jjSlits);
			componentFound = true;
		}

		BioXASXIAFilters *xiaFilters = qobject_cast<BioXASXIAFilters*>(component);
		if (!componentFound && xiaFilters) {
			componentView = new BioXASXIAFiltersView(xiaFilters);
			componentFound = true;
		}

		BioXASDBHRMirrors *dbhrMirrors = qobject_cast<BioXASDBHRMirrors*>(component);
		if (!componentFound && dbhrMirrors) {
			componentView = new BioXASDBHRMirrorsView(dbhrMirrors);
			componentFound = true;
		}

		CLSStandardsWheel *standardsWheel = qobject_cast<CLSStandardsWheel*>(component);
		if (!componentFound && standardsWheel) {
			componentView = new CLSStandardsWheelConfigurationView(standardsWheel);
			componentFound = true;
		}

		BioXASEndstationTable *table = qobject_cast<BioXASEndstationTable*>(component);
		if (!componentFound && table) {
			componentView = new BioXASEndstationTableView(table);
			componentFound = true;
		}

		CLSSIS3820Scaler *scaler = qobject_cast<CLSSIS3820Scaler*>(component);
		if (!componentFound && scaler) {
			componentView = new BioXASSIS3820ScalerView(scaler, true);
			componentFound = true;
		}

		BioXAS32ElementGeDetector *ge32Detector = qobject_cast<BioXAS32ElementGeDetector*>(component);
		if (!componentFound && ge32Detector) {
			BioXAS32ElementGeDetectorView *view = new BioXAS32ElementGeDetectorView(ge32Detector);
			view->buildDetectorView();
			view->addEmissionLineNameFilter(QRegExp("1"));
			view->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));

			componentView = view;
			componentFound = true;
		}

		BioXASFourElementVortexDetector *vortex4Detector = qobject_cast<BioXASFourElementVortexDetector*>(component);
		if (!componentFound && vortex4Detector) {
			BioXASFourElementVortexDetectorView *view = new BioXASFourElementVortexDetectorView(vortex4Detector);
			view->buildDetectorView();
			view->setEnergyRange(3000, 28000);
			view->addEmissionLineNameFilter(QRegExp("1"));
			view->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));

			componentView = view;
			componentFound = true;
		}

		BioXASCryostatStage *cryostatStage = qobject_cast<BioXASCryostatStage*>(component);
		if (!componentFound && cryostatStage) {
			componentView = new AMMotorGroupView(cryostatStage->motorGroup(), AMMotorGroupView::CompactView);
			componentFound = true;
		}
	}

	return componentView;
}

AMScanConfigurationView* BioXASAppController::createScanConfigurationView(AMScanConfiguration *configuration)
{
	AMScanConfigurationView *configurationView = 0;

	if (configuration) {
		bool configurationFound = false;

		BioXASXASScanConfiguration *xasConfiguration = qobject_cast<BioXASXASScanConfiguration*>(configuration);
		if (!configurationFound && xasConfiguration) {
			configurationView = new BioXASXASScanConfigurationView(xasConfiguration);
			configurationFound = true;
		}

		AMGenericStepScanConfiguration *commissioningConfiguration = qobject_cast<AMGenericStepScanConfiguration*>(configuration);
		if (!configurationFound && commissioningConfiguration) {
			configurationView = new AMGenericStepScanConfigurationView(commissioningConfiguration);
			configurationFound = true;
		}
	}

	return configurationView;
}

AMScanConfigurationViewHolder3* BioXASAppController::createScanConfigurationViewWithHolder(AMScanConfiguration *configuration)
{
	AMScanConfigurationViewHolder3 *view = 0;

	if (configuration) {
		AMScanConfigurationView *configurationView = createScanConfigurationView(configuration);

		// If the configuration view was created, wrap it in a configuration view holder.

		if (configurationView) {
			view = new AMScanConfigurationViewHolder3(configurationView, true);

			// If the configuration has a total time, make the appropriate connections.

			AMGenericStepScanConfiguration *stepScanConfiguration = qobject_cast<AMGenericStepScanConfiguration*>(configuration);
			if (stepScanConfiguration) {
				connect( stepScanConfiguration, SIGNAL(totalTimeChanged()), view, SLOT(updateOverallScanTime(double)) );
				view->updateOverallScanTime(stepScanConfiguration->totalTime());
			}
		}
	}

	return view;
}

QWidget* BioXASAppController::createCalibrationView(QObject *component)
{
	QWidget *calibrationView = 0;

	if (component) {
		bool componentFound = false;

		BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(component);
		if (!componentFound && mono) {
			calibrationView = new BioXASSSRLMonochromatorEnergyCalibrationView(mono);
			connect( calibrationView, SIGNAL(energyCalibrationScanRequested()), this, SLOT(goToEnergyCalibrationScanConfigurationView()) );
			componentFound = true;
		}
	}

	return calibrationView;
}

void BioXASAppController::addComponentView(QObject *component, const QString &componentName)
{
	addViewToGeneralPane( createComponentView(component), componentName );
}

void BioXASAppController::addDetectorView(QObject *detector, const QString &detectorName)
{
	addViewToDetectorsPane( createComponentView(detector), detectorName );
}

void BioXASAppController::addScanConfigurationView(AMScanConfiguration *configuration, const QString &viewName)
{
	addViewToScansPane( createScanConfigurationViewWithHolder(configuration), viewName );
}

void BioXASAppController::addCalibrationView(QObject *component, const QString &calibrationName)
{
	addViewToCalibrationPane( createCalibrationView(component), calibrationName );
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
