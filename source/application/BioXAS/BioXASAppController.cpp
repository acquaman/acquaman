#include "BioXASAppController.h"

BioXASAppController::BioXASAppController(QObject *parent) :
    AMAppController(parent)
{

}

BioXASAppController::~BioXASAppController()
{

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
//	AMDbObjectSupport::s()->registerClass<AM1DDarkCurrentCorrectionAB>();
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
//	AMDbObjectSupport::s()->registerClass<BioXASXRFScanConfiguration>();
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

void BioXASAppController::addCommissioningScanConfigurationView(AMGenericStepScanConfiguration *configuration)
{
	if (configuration) {

		QString scanName = "Commissioning Tool";
		QString iconName = ":/utilities-system-monitor.png";

		AMGenericStepScanConfigurationView *view = new AMGenericStepScanConfigurationView(configuration, AMBeamline::bl()->exposedControls(), AMBeamline::bl()->exposedDetectors());
		AMScanConfigurationViewHolder3 *viewHolder = new AMScanConfigurationViewHolder3(scanName, true, true, view, iconName);

		connect( configuration, SIGNAL(totalTimeChanged(double)), viewHolder, SLOT(updateOverallScanTime(double)) );
		viewHolder->updateOverallScanTime(configuration->totalTime());

		mw_->addPane(viewHolder, "Scans", scanName, iconName);
	}
}

void BioXASAppController::addPersistentView(QWidget *persistentView)
{
	if (persistentView) {
		persistentView->setFixedWidth(400);
		mw_->addRightWidget(persistentView);
	}
}
