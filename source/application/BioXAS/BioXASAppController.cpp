#include "BioXASAppController.h"

#include "beamline/CLS/CLSBeamlineStatus.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASUtilities.h"
#include "dataman/BioXAS/BioXASDbUpgrade1Pt1.h"


BioXASAppController::BioXASAppController(const QString &beamlineName, QObject *parent) :
	CLSAppController(beamlineName, parent)
{
	// Initialize controller settings.

	userConfiguration_ = new BioXASUserConfiguration(this);
	setDefaultUseLocalStorage(true);

	// Database upgrades.

	AMDbUpgrade *bioxas1pt1UserDb = new BioXASDbUpgrade1Pt1("user", this);
	appendDatabaseUpgrade(bioxas1pt1UserDb);
	AMDbUpgrade *bioxas1pt1ActionsDb = new BioXASDbUpgrade1Pt1("actions", this);
	appendDatabaseUpgrade(bioxas1pt1ActionsDb);

	// Initialize member variables.

	xasConfiguration_ = 0;
	genericConfiguration_ = 0;
	energyCalibrationConfiguration_ = 0;

	energyCalibrationView_ = 0;

	componentPaneCategoryName_ = "Components";
	componentPaneIcon_ = ":/system-software-update.png";
	calibrationPaneCategoryName_ = "Calibration";
	calibrationPaneIcon_ = ":system-search.png";
}

BioXASAppController::~BioXASAppController()
{

}

void BioXASAppController::onUserConfigurationLoadedFromDb()
{
	if (userConfiguration_) {

		AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

		for (int i = 0; i < geDetectors->count(); i++) {

			AMXRFDetector *geDetector = qobject_cast<AMXRFDetector*>(geDetectors->at(i));

			if (geDetector) {

				foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
					if (!containsRegionOfInterest(geDetector->regionsOfInterest(), region)) {
						geDetector->addRegionOfInterest(region);
						onRegionOfInterestAdded(region);
					}
				}

				connect(geDetector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
				connect(geDetector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
				connect(geDetector, SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));
			}
		}
	}
}

void BioXASAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	if (region) {

		// Add the region of interest to the user configuration, if it doesn't have it already.

		if (userConfiguration_ && !containsRegionOfInterest(userConfiguration_->regionsOfInterest(), region))
			userConfiguration_->addRegionOfInterest(region);

		// Add the region of interest to the XAS scan configuration, if it doesn't have it already.

		if (xasConfiguration_ && !containsRegionOfInterest(xasConfiguration_->regionsOfInterest(), region))
			xasConfiguration_->addRegionOfInterest(region);
	}
}

void BioXASAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	if (userConfiguration_ && userConfiguration_->regionsOfInterest().contains(region))
		userConfiguration_->removeRegionOfInterest(region);

	if (xasConfiguration_)
		xasConfiguration_->removeRegionOfInterest(region);
}

void BioXASAppController::onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region)
{
	if (userConfiguration_ && userConfiguration_->regionsOfInterest().contains(region))
		userConfiguration_->setRegionOfInterestBoundingRange(region);

	if (xasConfiguration_)
		xasConfiguration_->setRegionOfInterestBoundingRange(region);
}

void BioXASAppController::goToBeamlineStatusView(AMControl *control)
{
	if (beamlineStatusView_) {

		// Set the given control as the view's selected control.

		beamlineStatusView_->setSelectedComponent(control);

		// Set the beam status pane as the current pane.

		QWidget *windowPane = viewPaneMapping_.value(beamlineStatusView_, 0);
		if (windowPane)
			mw_->setCurrentPane(windowPane);
	}
}

void BioXASAppController::goToEnergyCalibrationScanConfigurationView()
{
	if (energyCalibrationConfigurationView_) {
		QWidget *windowPane = viewPaneMapping_.value(energyCalibrationConfigurationView_, 0);

		if (windowPane)
			mw_->setCurrentPane(windowPane);
	}
}

void BioXASAppController::goToEnergyCalibrationView(AMScan *toView)
{
	if (energyCalibrationView_) {
		QWidget *windowPane = viewPaneMapping_.value(energyCalibrationView_, 0);

		if (windowPane) {
			energyCalibrationView_->setCurrentScan(toView);
			mw_->setCurrentPane(windowPane);
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
	// Save current user configuration to the database.

	if (userConfiguration_)
		userConfiguration_->storeToDb(AMDatabase::database("user"));

	// If the scan was an energy calibration scan, set the calibration view's scan and make it the current pane.

	if (action && action->controller() && action->controller()->scan()) {
		BioXASXASScanConfiguration *xasScanConfiguration = qobject_cast<BioXASXASScanConfiguration*>(action->controller()->scan()->scanConfiguration());
		if (xasScanConfiguration && xasScanConfiguration->name() == "Energy Calibration XAS Scan")
			goToEnergyCalibrationView(action->controller()->scan());
	}
}

void BioXASAppController::updateXASScanConfigurationDetectors()
{
	updateScanConfigurationDetectors(xasConfiguration_, BioXASBeamline::bioXAS()->defaultXASScanDetectors());
	updateScanConfigurationDetectors(energyCalibrationConfiguration_, BioXASBeamline::bioXAS()->defaultXASScanDetectors());
}

void BioXASAppController::updateGenericScanConfigurationDetectors()
{
	AMDetectorSet *defaultDetectors = BioXASBeamline::bioXAS()->defaultGenericScanDetectors();
	AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();

	if (genericConfiguration_) {
		// Update the detectors added to the configuration by default.
		updateScanConfigurationDetectors(genericConfiguration_, defaultDetectors);

		 // Reset the configuration's I0 detector.
		 // If one of the detectors is I0, set the configuration's I0 detector.
		genericConfiguration_->setI0(AMDetectorInfo());

		if (defaultDetectors && i0Detector) {

			bool i0Found = false;

			for (int i = 0, count = defaultDetectors->count(); i < count && !i0Found; i++) {
				AMDetector *defaultDetector = defaultDetectors->at(i);

				if (defaultDetector == i0Detector) {
					genericConfiguration_->setI0(defaultDetector->toInfo());
					i0Found = true;
				}
			}
		}
	}
}

void BioXASAppController::initializeBeamline()
{
	BioXASBeamline::bioXAS();
}

void BioXASAppController::registerDBClasses()
{
	AMDbObjectSupport::s()->registerClass<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASGenericStepScanConfiguration>();
}

void BioXASAppController::registerExporterOptions()
{
	AMExporterOptionXDIFormat *xasExporterOption = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", "", "", true);

	if (xasExporterOption->id() > 0)
		AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(xasExporterOption->id());

	AMExporterOptionXDIFormat *genericExporterOption = BioXAS::buildStandardXDIFormatExporterOption("BioXAS Generic Step Scan (XDI Format)", "", "", true);

	if (genericExporterOption->id() > 0)
		AMAppControllerSupport::registerClass<BioXASGenericStepScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(genericExporterOption->id());

}

void BioXASAppController::setupScanConfigurations()
{
	// setup XAS scan configuration
	xasConfiguration_ = new BioXASXASScanConfiguration();
	setupXASScanConfiguration(xasConfiguration_);

	// setup generic scan configuration
	genericConfiguration_ = new BioXASGenericStepScanConfiguration();
	setupGenericStepScanConfiguration(genericConfiguration_);

	// setup energy calibration scan configuration
	energyCalibrationConfiguration_ = new BioXASXASScanConfiguration();
	energyCalibrationConfiguration_->setName("Energy Calibration XAS Scan");
	energyCalibrationConfiguration_->setUserScanName("Energy Calibration XAS Scan");
	setupXASScanConfiguration(energyCalibrationConfiguration_);
}

void BioXASAppController::setupUserConfiguration()
{
	if (userConfiguration_) {

		connect( userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()) );

		bool loaded = userConfiguration_->loadFromDb(AMDatabase::database("user"), 1);
		if (!loaded) {
			userConfiguration_->storeToDb(AMDatabase::database("user"));
			onUserConfigurationLoadedFromDb();
		}
	}
}

void BioXASAppController::setupUserInterfaceImplementation()
{
	mw_->setWindowTitle("Acquaman - BioXAS");

	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading(componentPaneCategoryName_, 1);
	createComponentsPane();

	mw_->insertHeading(calibrationPaneCategoryName_, 4);
	createCalibrationPane();
}

void BioXASAppController::createPersistentView()
{
	BioXASPersistentView *persistentView = new BioXASPersistentView();
	persistentView->setFixedWidth(400);

	mw_->addRightWidget(persistentView);

	connect( persistentView, SIGNAL(beamStatusButtonsSelectedControlChanged(AMControl*)), this, SLOT(goToBeamStatusView(AMControl*)) );
}

void BioXASAppController::createGeneralPanes()
{
	QWidget* beamlineConfigurationView = createComponentView(BioXASBeamline::bioXAS());
	addMainWindowViewToPane( beamlineConfigurationView, "Configuration", generalPaneCategeryName_, generalPaneIcon_);

	beamlineStatusView_ = new CLSBeamlineStatusView(BioXASBeamline::bioXAS()->beamStatus(), false);
	addMainWindowViewToPane( beamlineStatusView_, "Beamline status", generalPaneCategeryName_, generalPaneIcon_);
}

void BioXASAppController::createDetectorPanes()
{
	QWidget* scalerView = createComponentView(BioXASBeamline::bioXAS()->scaler());
	addMainWindowViewToPane( scalerView, "Scaler", detectorPaneCategoryName_, detectorPaneIcon_);

	QWidget* fourElementDetectorView = createComponentView(BioXASBeamline::bioXAS()->fourElementVortexDetector());
	addMainWindowViewToPane( fourElementDetectorView, "Four element", detectorPaneCategoryName_, detectorPaneIcon_);
}

void BioXASAppController::createScanConfigurationPanes()
{
	xasConfigurationView_ = createScanConfigurationViewWithHolder(xasConfiguration_);
	if (xasConfigurationView_)
		addMainWindowViewToPane(xasConfigurationView_, "XAS Scan", scanPaneCategoryName_, scanPaneIcon_);

	genericConfigurationView_ = createScanConfigurationViewWithHolder(genericConfiguration_);
	if (genericConfigurationView_)
		addMainWindowViewToPane(genericConfigurationView_, "Generic Scan", scanPaneCategoryName_, scanPaneIcon_);

	energyCalibrationConfigurationView_ = createScanConfigurationViewWithHolder(energyCalibrationConfiguration_);
	if (energyCalibrationConfigurationView_)
		addMainWindowViewToPane(energyCalibrationConfigurationView_, "Energy Calibration", scanPaneCategoryName_, scanPaneIcon_);
}

void BioXASAppController::createComponentsPane()
{
	BioXASBeamline *bioXASBL = BioXASBeamline::bioXAS();

	addMainWindowViewToPane( createComponentView(bioXASBL->carbonFilterFarm()), "Carbon Filter Farm", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->m1Mirror()), "M1 Mirror", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->mono()), "Monochromator", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->m2Mirror()), "M2 Mirror", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->beWindow()), "Be Window", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->endstationTable()), "Endstation Table", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->dbhrMirrors()), "DBHR Mirrors", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->jjSlits()), "JJ Slits", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->xiaFilters()), "XIA Filters", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->standardsWheel()), "Standards Wheel", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->cryostatStage()), "Cryostat Stage", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->filterFlipper()), "Filter Flipper", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->sollerSlit()), "Soller slits", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->detectorStageLateralMotors()), "Ge 32-el Stage", componentPaneCategoryName_, componentPaneIcon_);
	addMainWindowViewToPane( createComponentView(bioXASBL->zebra()), "Zebra", componentPaneCategoryName_, componentPaneIcon_);

}

void BioXASAppController::createCalibrationPane()
{
	if (BioXASBeamline::bioXAS()->mono()) {
		energyCalibrationView_ = new BioXASSSRLMonochromatorEnergyCalibrationView(BioXASBeamline::bioXAS()->mono(), 0);

		addMainWindowViewToPane(energyCalibrationView_, "Energy", calibrationPaneCategoryName_, calibrationPaneIcon_);
		connect( energyCalibrationView_, SIGNAL(energyCalibrationScanRequested()), this, SLOT(goToEnergyCalibrationScanConfigurationView()) );
	}
}

QWidget* BioXASAppController::createComponentView(QObject *component)
{
	QWidget *componentView = 0;

	if (component) {
		bool componentFound = false;

		// Try to match up given component with known component types.
		// If match found, create appropriate view.

		BioXASCryostat *cryostat = qobject_cast<BioXASCryostat*>(component);
		if (!componentFound && cryostat) {
			componentView = new BioXASCryostatView(cryostat);
			componentFound = true;
		}

		AMSlits *jjSlits = qobject_cast<AMSlits*>(component);
		if (!componentFound && jjSlits) {
			componentView = new AMSlitsView(jjSlits);
			componentFound = true;
		}

		BioXASBeamline *beamline = qobject_cast<BioXASBeamline*>(component);
		if (!componentFound && beamline) {
			componentView = new BioXASBeamlineConfigurationView();
			componentFound = true;
		}

		BioXASSollerSlit *sollerSlit = qobject_cast<BioXASSollerSlit*>(component);
		if (!componentFound && sollerSlit) {
			componentView = new BioXASSollerSlitView(sollerSlit);
			componentFound = true;
		}

		BioXASZebra *zebra = qobject_cast<BioXASZebra*>(component);
		if (!componentFound && zebra) {
			componentView = new BioXASZebraView(zebra);
			componentFound = true;
		}

		CLSBeamlineStatus *beamStatus = qobject_cast<CLSBeamlineStatus*>(component);
		if (!componentFound && beamStatus) {
			componentView = new CLSBeamlineStatusView(beamStatus, false);
			componentFound = true;
		}

		BioXASFrontEndShutters *shutters = qobject_cast<BioXASFrontEndShutters*>(component);
		if (!componentFound && shutters) {
			componentView = new BioXASFrontEndShuttersView(shutters);
			componentFound = true;
		}

		CLSValves *valves = qobject_cast<CLSValves*>(component);
		if (!componentFound && valves) {
			componentView = new BioXASValvesView(valves);
			componentFound = true;
		}

		BioXASM1Mirror *m1Mirror = qobject_cast<BioXASM1Mirror*>(component);
		if (!componentFound && m1Mirror) {
			componentView = new BioXASM1MirrorView(m1Mirror);
			componentFound = true;
		}

		BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(component);
		if (!componentFound && mono) {
			componentView = new BioXASSSRLMonochromatorView(mono);
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

		BioXASFilterFlipper *filterFlipper = qobject_cast<BioXASFilterFlipper*>(component);
		if (!componentFound && filterFlipper) {
			componentView = new BioXASFilterFlipperView(filterFlipper);
			componentFound = true;
		}

		CLSMAXvMotor *motor = qobject_cast<CLSMAXvMotor*>(component);
		if (!componentFound && motor) {
			componentView = new CLSControlEditor(motor);
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
			configurationView = new BioXASXASScanConfigurationEditor(xasConfiguration);
			configurationFound = true;
		}

		AMGenericStepScanConfiguration *commissioningConfiguration = qobject_cast<AMGenericStepScanConfiguration*>(configuration);
		if (!configurationFound && commissioningConfiguration) {
			configurationView = new AMGenericStepScanConfigurationView(commissioningConfiguration, BioXASBeamline::bioXAS()->exposedControls(), BioXASBeamline::bioXAS()->defaultGenericScanDetectorOptions());
			configurationFound = true;
		}
	}

	return configurationView;
}

AMScanConfigurationViewHolder3* BioXASAppController::createScanConfigurationViewWithHolder(AMScanConfiguration *configuration)
{
	AMScanConfigurationViewHolder3 *configurationViewHolder = 0;

	if (configuration) {
		AMScanConfigurationView *configurationView = createScanConfigurationView(configuration);

		// If the configuration view was created, wrap it in a configuration view holder.

		if (configurationView) {
			configurationViewHolder = new AMScanConfigurationViewHolder3(configurationView, true);

			// If the configuration has a total time, make the appropriate connections.

			AMGenericStepScanConfiguration *stepScanConfiguration = qobject_cast<AMGenericStepScanConfiguration*>(configuration);
			if (stepScanConfiguration) {
				connect( stepScanConfiguration, SIGNAL(totalTimeChanged(double)), configurationViewHolder, SLOT(updateOverallScanTime(double)) );
				configurationViewHolder->updateOverallScanTime(stepScanConfiguration->totalTime());
			}
		}
	}

	return configurationViewHolder;
}

void BioXASAppController::setupXASScanConfiguration(BioXASXASScanConfiguration *configuration)
{
	if (configuration) {

		// Set the energy as the scanned control.

		BioXASMonochromator *mono = BioXASBeamline::bioXAS()->mono();

		if (mono) {
			AMControl *energyControl = mono->energy();

			if (energyControl) {

				configuration->setControl(0, energyControl->toInfo());
				configuration->setupDefaultXANESRegions();
			}
		}

		// Sets the default edge, Cu.

		AMElement *defaultElement = AMPeriodicTable::table()->elementBySymbol("Cu");
		QList<AMAbsorptionEdge> edges = defaultElement->absorptionEdges();

		if (!edges.isEmpty()) {
			AMAbsorptionEdge defaultEdge = edges.first();
			double defaultEnergy = defaultEdge.energy();

			configuration->setEdge(defaultEdge.toString().split(":").first());
			configuration->setEnergy(defaultEnergy);
		}

		// Set scan detectors.
		connect( BioXASBeamline::bioXAS()->defaultXASScanDetectors(), SIGNAL(connected(bool)), this, SLOT(updateXASScanConfigurationDetectors()) );
		connect( BioXASBeamline::bioXAS()->defaultXASScanDetectors(), SIGNAL(detectorAdded(int)), this, SLOT(updateXASScanConfigurationDetectors()) );
		connect( BioXASBeamline::bioXAS()->defaultXASScanDetectors(), SIGNAL(detectorRemoved(int)), this, SLOT(updateXASScanConfigurationDetectors()) );

		updateXASScanConfigurationDetectors();
	}
}

void BioXASAppController::setupGenericStepScanConfiguration(AMGenericStepScanConfiguration *configuration)
{
	if (configuration) {

		configuration->setAutoExportEnabled(false);

		// Set scan detectors.
		connect( BioXASBeamline::bioXAS()->defaultGenericScanDetectors(), SIGNAL(connected(bool)), this, SLOT(updateGenericScanConfigurationDetectors()) );
		connect( BioXASBeamline::bioXAS()->defaultGenericScanDetectors(), SIGNAL(detectorAdded(int)), this, SLOT(updateGenericScanConfigurationDetectors()) );
		connect( BioXASBeamline::bioXAS()->defaultGenericScanDetectors(), SIGNAL(detectorRemoved(int)), this, SLOT(updateGenericScanConfigurationDetectors()) );

		updateGenericScanConfigurationDetectors();
	}
}

void BioXASAppController::updateScanConfigurationDetectors(AMGenericStepScanConfiguration *configuration, AMDetectorSet *detectors)
{
	if (configuration && detectors) {

		// Clear the configuration detectors.

		configuration->detectorConfigurations().clear();

		// Add the valid, connected detectors from the detectors set.

		for (int i = 0, count = detectors->count(); i < count; i++) {
			AMDetector *detector = detectors->at(i);

			if (detector && detector->isConnected())
				configuration->addDetector(detector->toInfo());
		}
	}
}

bool BioXASAppController::containsRegionOfInterest(QList<AMRegionOfInterest *> regionOfInterestList, AMRegionOfInterest *toFind) const
{
	bool regionOfInterestFound = false;

	if (!regionOfInterestList.isEmpty() && toFind) {
		for (int i = 0, count = regionOfInterestList.count(); i < count && !regionOfInterestFound; i++) {
			AMRegionOfInterest *regionOfInterest = regionOfInterestList.at(i);

			if (regionOfInterest && regionOfInterest->name() == toFind->name())
				regionOfInterestFound = true;
		}
	}

	return regionOfInterestFound;
}
