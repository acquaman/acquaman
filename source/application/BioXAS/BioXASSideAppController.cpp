/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "BioXASSideAppController.h"

#include "acquaman/BioXAS/BioXASSideXASScanConfiguration.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "ui/BioXAS/BioXASSidePersistentView.h"

BioXASSideAppController::BioXASSideAppController(QObject *parent)
	: BioXASAppController(parent)
{
	scalerView_ = 0;
	monoConfigView_ = 0;
	jjSlitsView_ = 0;
	xiaFiltersView_ = 0;
	carbonFilterFarmView_ = 0;
	m2MirrorView_ = 0;
	dbhrView_ = 0;

	persistentPanel_ = 0;

	configuration_ = 0;
	configurationView_ = 0;
	configurationViewHolder_ = 0;

	commissioningConfiguration_ = 0;
	commissioningConfigurationView_ = 0;
	commissioningConfigurationViewHolder_ = 0;

	monoCalibrationConfiguration_ = 0;
	monoCalibrationConfigurationView_ = 0;
	monoCalibrationConfigurationViewHolder_ = 0;
}

BioXASSideAppController::~BioXASSideAppController()
{

}

bool BioXASSideAppController::startup()
{
	// Get a destination folder.
	if ( !AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-s/AcquamanSideData", "/home/bioxas-s/AcquamanSideData", "users", QStringList()) )
		return false;

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

			AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::BioXASSideBeamline), CLSFacilityID::BioXASSideBeamline); //6: BioXAS Side Beamline
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		setupExporterOptions();
		setupUserInterface();
		makeConnections();
		applyCurrentSettings();

		if (!userConfiguration_->loadFromDb(AMDatabase::database("user"), 1)){

			userConfiguration_->storeToDb(AMDatabase::database("user"));
			onUserConfigurationLoadedFromDb();
		}

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

void BioXASSideAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	BioXASAppController::onRegionOfInterestAdded(region);

	configuration_->addRegionOfInterest(region);
}

void BioXASSideAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	BioXASAppController::onRegionOfInterestRemoved(region);

	configuration_->removeRegionOfInterest(region);
}

void BioXASSideAppController::onScalerConnected()
{
	CLSSIS3820Scaler *scaler = BioXASSideBeamline::bioXAS()->scaler();

	if (scaler && scaler->isConnected() && !scalerView_) {
		scalerView_ = new BioXASSIS3820ScalerView(scaler, true);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Scaler", ":/system-search.png", scalerView_), "Detectors", "Scaler", ":/system-search.png", true);
	}
}

void BioXASSideAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<BioXASSideXASScanConfiguration>();
}

void BioXASSideAppController::setupExporterOptions()
{
	AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", "", "", true);

	if (bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASSideXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
}

void BioXASSideAppController::initializeBeamline()
{
	BioXASSideBeamline::bioXAS();
}

void BioXASSideAppController::setupUserInterface()
{
	mw_->setWindowTitle("Acquaman - BioXAS Side");

	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("General", 0);
	mw_->insertHeading("Detectors", 1);
	mw_->insertHeading("Scans", 2);
	mw_->insertHeading("Calibration", 3);

	// Add widgets to main window panes:
	////////////////////////////////////

	endstationTableView_ = new BioXASEndstationTableView(BioXASSideBeamline::bioXAS()->endstationTable());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Endstation Table", ":/system-software-update.png", endstationTableView_), "General", "Endstation Table", ":/system-software-update.png");

	carbonFilterFarmView_ = new BioXASCarbonFilterFarmView(BioXASSideBeamline::bioXAS()->carbonFilterFarm());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Carbon Filter Farm", ":/system-software-update.png", carbonFilterFarmView_), "General", "Carbon filter farm", ":/system-software-update.png");

	m1MirrorView_ = new BioXASM1MirrorView(BioXASSideBeamline::bioXAS()->m1Mirror());
	mw_->addPane(AMMainWindow::buildMainWindowPane("M1 Mirror", ":/system-software-update.png", m1MirrorView_), "General", "M1 Mirror", ":/system-software-update.png");

	monoConfigView_ = new BioXASSSRLMonochromatorConfigurationView(BioXASSideBeamline::bioXAS()->mono());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Monochromator", ":/system-software-update.png", monoConfigView_), "General", "Monochromator", ":/system-software-update.png");

	jjSlitsView_ = new CLSJJSlitsView(BioXASSideBeamline::bioXAS()->jjSlits());
	mw_->addPane(AMMainWindow::buildMainWindowPane("JJ Slits", ":/system-software-update.png", jjSlitsView_), "General", "JJ Slits", ":/system-software-update.png");

	xiaFiltersView_ = new BioXASXIAFiltersView(BioXASSideBeamline::bioXAS()->xiaFilters());
	mw_->addPane(AMMainWindow::buildMainWindowPane("XIA Filters", ":/system-software-update.png", xiaFiltersView_), "General", "XIA Filters", ":/system-software-update.png");

	m2MirrorView_ = new BioXASM2MirrorView(BioXASSideBeamline::bioXAS()->m2Mirror());
	mw_->addPane(AMMainWindow::buildMainWindowPane("M2 Mirror", ":/system-software-update.png", m2MirrorView_), "General", "M2 Mirror", ":/system-software-update.png");

	dbhrView_ = new BioXASDBHRMirrorView(BioXASSideBeamline::bioXAS()->dbhrMirror());
	mw_->addPane(AMMainWindow::buildMainWindowPane("DBHR Mirrors", ":/system-software-update.png", dbhrView_), "General", "DBHR Mirrors", ":/system-software-update.png");

	CLSStandardsWheelConfigurationView *wheelView = new CLSStandardsWheelConfigurationView(BioXASSideBeamline::bioXAS()->standardsWheel());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Standards Wheel", ":/system-software-update.png", wheelView), "General", "Standards Wheel", ":/system-software-update.png");

	AMMotorGroupView *cryostatStageView = new AMMotorGroupView(BioXASSideBeamline::bioXAS()->motorGroup());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Cryostat Stage", ":/system-software-update.png", cryostatStageView), "General", "Cryostat Stage", ":/system-software-update.png");

	// Create scaler view, if scaler is present and connected.
	onScalerConnected();

//	BioXAS32ElementGeDetectorView *geDetectorView = new BioXAS32ElementGeDetectorView(BioXASSideBeamline::bioXAS()->ge32ElementDetector());
//	geDetectorView->buildDetectorView();
//	geDetectorView->addEmissionLineNameFilter(QRegExp("1"));
//	geDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
//	mw_->addPane(geDetectorView, "Detectors", "Ge 32-el", ":/system-search.png");

	BioXASFourElementVortexDetectorView *fourElementDetectorView = new BioXASFourElementVortexDetectorView(BioXASSideBeamline::bioXAS()->fourElementVortexDetector());
	fourElementDetectorView->buildDetectorView();
	fourElementDetectorView->setEnergyRange(3000, 28000);
	fourElementDetectorView->addEmissionLineNameFilter(QRegExp("1"));
	fourElementDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	mw_->addPane(fourElementDetectorView, "Detectors", "4-element", ":/system-search.png");

	configuration_ = new BioXASSideXASScanConfiguration();
	configuration_->setEnergy(10000);
	configurationView_ = new BioXASXASScanConfigurationView(configuration_);
	configurationViewHolder_ = new AMScanConfigurationViewHolder3("Configure an XAS Scan", true, true, configurationView_);
	connect(configuration_, SIGNAL(totalTimeChanged(double)), configurationViewHolder_, SLOT(updateOverallScanTime(double)));
	configurationViewHolder_->updateOverallScanTime(configuration_->totalTime());
	mw_->addPane(configurationViewHolder_, "Scans", "XAS Scan", ":/utilities-system-monitor.png");

	commissioningConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningConfiguration_->setAutoExportEnabled(false);
	commissioningConfiguration_->addDetector(BioXASSideBeamline::bioXAS()->exposedDetectorByName("I0Detector")->toInfo());
	commissioningConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningConfiguration_);
	commissioningConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Commissioning Tool", false, true, commissioningConfigurationView_);
	mw_->addPane(commissioningConfigurationViewHolder_, "Scans", "Commissioning Tool", ":/utilities-system-monitor.png");

	monoCalibrationConfiguration_ = new BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration();
	monoCalibrationConfiguration_->setEnergy(10000);
	monoCalibrationConfigurationView_ = new BioXASXASScanConfigurationView(monoCalibrationConfiguration_);
	monoCalibrationConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Calibrate Energy", false, true, monoCalibrationConfigurationView_, ":/system-search.png");
	mw_->addPane(monoCalibrationConfigurationViewHolder_, "Calibration", "Calibrate Energy", ":/system-search.png");

	// Create persistent view panel:
	////////////////////////////////////

	persistentPanel_ = new BioXASSidePersistentView();
	persistentPanel_->setFixedWidth(400);
	mw_->addRightWidget(persistentPanel_);
}

void BioXASSideAppController::makeConnections()
{
	connect( BioXASSideBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()) );
}

void BioXASSideAppController::applyCurrentSettings()
{
	onScalerConnected();
}
