#include "BioXASAppController.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
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

	calibrationPaneIcon_ = ":system-search.png";
}

BioXASAppController::~BioXASAppController()
{

}

bool BioXASAppController::startup()
{
	bool result = false;

	// Start up the main program.
	if (CLSAppController::startup()) {

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		result = true;
	}

	return result;
}

void BioXASAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	CLSAppController::shutdown();
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
						geDetector->addRegionOfInterest(region->createCopy());
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

void BioXASAppController::goToBeamStatusView(AMControl *control)
{
	if (beamStatusView_) {

		// Set the given control as the view's selected control.

		beamStatusView_->setSelectedComponent(control);

		// Set the beam status pane as the current pane.

		QWidget *windowPane = viewPaneMapping_.value(beamStatusView_, 0);

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

void BioXASAppController::updateXASScanConfigurationDetectors()
{
	updateScanConfigurationDetectors(xasConfiguration_, BioXASBeamline::bioXAS()->defaultXASScanDetectors());
	updateScanConfigurationDetectors(energyCalibrationConfiguration_, BioXASBeamline::bioXAS()->defaultXASScanDetectors());
}

void BioXASAppController::updateGenericScanConfigurationDetectors()
{
	AMDetectorSet *defaultDetectors = BioXASBeamline::bioXAS()->defaultGenericScanDetectors();

	// Update the detectors added to the configuration by default.

	updateScanConfigurationDetectors(genericConfiguration_, defaultDetectors);

	 // Reset the configuration's I0 detector.
	 // If one of the detectors is I0, set the configuration's I0 detector.

	if (genericConfiguration_) {

		genericConfiguration_->setI0(AMDetectorInfo());

		AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();

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
	xasConfiguration_ = new BioXASXASScanConfiguration();
	setupXASScanConfiguration(xasConfiguration_);

	genericConfiguration_ = new BioXASGenericStepScanConfiguration();
	setupGenericStepScanConfiguration(genericConfiguration_);

	energyCalibrationConfiguration_ = new BioXASXASScanConfiguration();
	energyCalibrationConfiguration_->setName("Energy Calibration XAS Scan");
	energyCalibrationConfiguration_->setUserScanName("Energy Calibration XAS Scan");
	setupXASScanConfiguration(energyCalibrationConfiguration_);
}

void BioXASAppController::setupUserInterface()
{
	CLSAppController::setupUserInterface();

	mw_->setWindowTitle("Acquaman - BioXAS");

	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("Components", 1);
	createComponentsPane("Components");

	mw_->insertHeading("Calibration", 4);
	createCalibrationPane("Calibration");
}

void BioXASAppController::makeConnections()
{

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

void BioXASAppController::createPersistentView()
{
	BioXASPersistentView *persistentView = new BioXASPersistentView();

	persistentView->setFixedWidth(400);
	mw_->addRightWidget(persistentView);

	connect( persistentView, SIGNAL(beamStatusButtonsSelectedControlChanged(AMControl*)), this, SLOT(goToBeamStatusView(AMControl*)) );
}

void BioXASAppController::createGeneralPanes()
{
	addViewToGeneralPane( createComponentView(BioXASBeamline::bioXAS()), "Configuration", generalPaneCategeryName_);

	beamStatusView_ = new BioXASBeamStatusView(BioXASBeamline::bioXAS()->beamStatus());
	addViewToGeneralPane(beamStatusView_, "Beam status", generalPaneCategeryName_);
}

void BioXASAppController::createDetectorPanes()
{
	addViewToDetectorsPane( createComponentView(BioXASBeamline::bioXAS()->scaler()), "Scaler", detectorPaneCategoryName_ );
	addViewToDetectorsPane( createComponentView(BioXASBeamline::bioXAS()->fourElementVortexDetector()), "Four element", detectorPaneCategoryName_ );
}

void BioXASAppController::createScanConfigurationPanes()
{
	xasConfigurationView_ = createScanConfigurationViewWithHolder(xasConfiguration_);
	addViewToScansPane(xasConfigurationView_, "XAS Scan", scanPaneCategoryName_);

	genericConfigurationView_ = createScanConfigurationViewWithHolder(genericConfiguration_);
	addViewToScansPane(genericConfigurationView_, "Generic Scan", scanPaneCategoryName_);

	energyCalibrationConfigurationView_ = createScanConfigurationViewWithHolder(energyCalibrationConfiguration_);
	addViewToScansPane(energyCalibrationConfigurationView_, "Energy Calibration", scanPaneCategoryName_);
}

void BioXASAppController::createComponentsPane(const QString &paneCategoryName)
{
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->carbonFilterFarm()), "Carbon Filter Farm", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->m1Mirror()), "M1 Mirror", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->mono()), "Monochromator", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->m2Mirror()), "M2 Mirror", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->beWindow()), "Be Window", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->endstationTable()), "Endstation Table", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->dbhrMirrors()), "DBHR Mirrors", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->jjSlits()), "JJ Slits", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->xiaFilters()), "XIA Filters", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->standardsWheel()), "Standards Wheel", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->cryostatStage()), "Cryostat Stage", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->filterFlipper()), "Filter Flipper", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->sollerSlit()), "Soller slits", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->detectorStageLateralMotors()), "Ge 32-el Stage", paneCategoryName);
	addViewToComponentsPane( createComponentView(BioXASBeamline::bioXAS()->zebra()), "Zebra", paneCategoryName);
}

void BioXASAppController::createCalibrationPane(const QString &paneCategoryName)
{
	if (BioXASBeamline::bioXAS()->mono()) {
		energyCalibrationView_ = new BioXASSSRLMonochromatorEnergyCalibrationView(BioXASBeamline::bioXAS()->mono(), 0);

		addViewToCalibrationPane(energyCalibrationView_, "Energy", paneCategoryName );
		connect( energyCalibrationView_, SIGNAL(energyCalibrationScanRequested()), this, SLOT(goToEnergyCalibrationScanConfigurationView()) );
	}
}


QWidget* BioXASAppController::createGeneralPaneView(QWidget *view, const QString &viewName)
{
	QWidget *pane = 0;

	if (view)
		pane = AMMainWindow::buildMainWindowPane(viewName, generalPaneIcon_, view);

	return pane;
}

QWidget* BioXASAppController::createComponentPaneView(QWidget *view, const QString &viewName)
{
	QWidget *pane = 0;

	if (view)
		pane = AMMainWindow::buildMainWindowPane(viewName, generalPaneIcon_, view);

	return pane;
}

QWidget* BioXASAppController::createDetectorPaneView(QWidget *view, const QString &viewName)
{
	QWidget *pane = 0;

	if (view)
		pane = AMMainWindow::buildMainWindowPane(viewName, detectorsPaneIcon_, view);

	return pane;
}

QWidget* BioXASAppController::createScansPane(QWidget *view, const QString &viewName)
{
	QWidget *pane = 0;

	if (view)
		pane = AMMainWindow::buildMainWindowPane(viewName, scansPaneIcon_, view);

	return pane;
}

QWidget* BioXASAppController::createCalibrationPane(QWidget *view, const QString &viewName)
{
	QWidget *pane = 0;

	if (view)
		pane = AMMainWindow::buildMainWindowPane(viewName, calibrationPaneIcon_, view);

	return pane;
}

void BioXASAppController::addViewToGeneralPane(QWidget *view, const QString &viewName, const QString &paneCategoryName)
{
	if (view) {
		QWidget *generalView = createGeneralPaneView(view, viewName);

		if (generalView) {
			mw_->addPane(generalView, paneCategoryName, viewName, generalPaneIcon_);
			viewPaneMapping_.insert(view, generalView);
		}
	}
}

void BioXASAppController::addViewToComponentsPane(QWidget *view, const QString &viewName, const QString &paneCategoryName)
{
	if (view) {
		QWidget *componentView = createComponentPaneView(view, viewName);

		if (componentView) {
			mw_->addPane(componentView, paneCategoryName, viewName, generalPaneIcon_);
			viewPaneMapping_.insert(view, componentView);
		}
	}
}

void BioXASAppController::addViewToDetectorsPane(QWidget *view, const QString &viewName, const QString &categoryName)
{
	if (view) {
		QWidget *detectorView = 0;

		BioXAS32ElementGeDetectorView *ge32View = qobject_cast<BioXAS32ElementGeDetectorView*>(view);
		if (ge32View)
			detectorView = view;
		else
			detectorView = createDetectorPaneView(view, viewName);

		if (detectorView) {
			mw_->addPane(detectorView, categoryName, viewName, detectorsPaneIcon_);
			viewPaneMapping_.insert(view, detectorView);
		}
	}
}

void BioXASAppController::addViewToScansPane(QWidget *view, const QString &viewName, const QString &paneCategoryName)
{
	if (view) {
		QWidget *scanView = createScansPane(view, viewName);

		if (scanView) {
			mw_->addPane(scanView, paneCategoryName, viewName, scansPaneIcon_);
			viewPaneMapping_.insert(view, scanView);
		}
	}
}

void BioXASAppController::addViewToCalibrationPane(QWidget *view, const QString &viewName, const QString &paneCategoryName)
{
	if (view) {
		QWidget *calibrationView = createCalibrationPane(view, viewName);

		if (calibrationView) {
			mw_->addPane(calibrationView, paneCategoryName, viewName, calibrationPaneIcon_);
			viewPaneMapping_.insert(view, calibrationView);
		}
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

		BioXASBeamStatus *beamStatus = qobject_cast<BioXASBeamStatus*>(component);
		if (!componentFound && beamStatus) {
			componentView = new BioXASBeamStatusView(beamStatus);
			componentFound = true;
		}

		BioXASFrontEndShutters *shutters = qobject_cast<BioXASFrontEndShutters*>(component);
		if (!componentFound && shutters) {
			componentView = new BioXASFrontEndShuttersView(shutters);
			componentFound = true;
		}

		BioXASValves *valves = qobject_cast<BioXASValves*>(component);
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
			componentView = new BioXASControlEditor(motor);
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
	AMScanConfigurationViewHolder3 *view = 0;

	if (configuration) {
		AMScanConfigurationView *configurationView = createScanConfigurationView(configuration);

		// If the configuration view was created, wrap it in a configuration view holder.

		if (configurationView) {
			view = new AMScanConfigurationViewHolder3(configurationView, true);

			// If the configuration has a total time, make the appropriate connections.

			AMGenericStepScanConfiguration *stepScanConfiguration = qobject_cast<AMGenericStepScanConfiguration*>(configuration);
			if (stepScanConfiguration) {
				connect( stepScanConfiguration, SIGNAL(totalTimeChanged(double)), view, SLOT(updateOverallScanTime(double)) );
				view->updateOverallScanTime(stepScanConfiguration->totalTime());
			}
		}
	}

	return view;
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
