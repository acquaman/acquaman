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

#include <QGroupBox>
#include <QWidget>

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"
#include "application/BioXAS/BioXAS.h"

#include "dataman/AMRun.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "dataman/BioXAS/BioXASUserConfiguration.h"

#include "util/AMPeriodicTable.h"

#include "beamline/CLS/CLSFacilityID.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "acquaman/BioXAS/BioXASSideXASScanConfiguration.h"
#include "acquaman/BioXAS/BioXASXRFScanConfiguration.h"

#include "ui/AMMainWindow.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/AMMotorGroupView.h"

#include "ui/CLS/CLSJJSlitsView.h"

#include "ui/util/AMChooseDataFolderDialog.h"

#include "ui/BioXAS/BioXASEndstationTableView.h"
#include "ui/BioXAS/BioXASSidePersistentView.h"
#include "ui/BioXAS/BioXASSideXASScanConfigurationView.h"
#include "ui/BioXAS/BioXAS32ElementGeDetectorView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h"

#include "ui/CLS/CLSStandardsWheelConfigurationView.h"

#include "ui/BioXAS/BioXASXIAFiltersView.h"
#include "ui/BioXAS/BioXASCarbonFilterFarmView.h"
#include "ui/BioXAS/BioXASM2MirrorView.h"
#include "ui/BioXAS/BioXASDBHRMirrorView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerView.h"
#include "ui/BioXAS/BioXASFourElementVortexDetectorView.h"

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

	setDefaultUseLocalStorage(true);
	userConfiguration_ = new BioXASUserConfiguration(this);
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
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASXRFScanConfiguration>();
}

void BioXASSideAppController::setupExporterOptions()
{
	AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", "", "", true);

	if (bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASSideXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
}

void BioXASSideAppController::setupUserInterface()
{
	mw_->setWindowTitle("Acquaman - BioXAS Side");

	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("General", 0);
	mw_->insertHeading("Detectors", 1);
	mw_->insertHeading("Scans", 2);

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

	BioXAS32ElementGeDetectorView *geDetectorView = new BioXAS32ElementGeDetectorView(BioXASSideBeamline::bioXAS()->ge32ElementDetector());
	geDetectorView->buildDetectorView();
	geDetectorView->addEmissionLineNameFilter(QRegExp("1"));
	geDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	mw_->addPane(geDetectorView, "Detectors", "Ge 32-el", ":/system-search.png");

//	BioXASFourElementVortexDetectorView *fourElementDetectorView = new BioXASFourElementVortexDetectorView(BioXASSideBeamline::bioXAS()->fourElementVortexDetector());
//	fourElementDetectorView->buildDetectorView();
//	fourElementDetectorView->setEnergyRange(3000, 28000);
//	fourElementDetectorView->addEmissionLineNameFilter(QRegExp("1"));
//	fourElementDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
//	mw_->addPane(fourElementDetectorView, "Detectors", "4-element", ":/system-search.png");

	configuration_ = new BioXASSideXASScanConfiguration();
	configuration_->setEnergy(10000);
	configurationView_ = new BioXASSideXASScanConfigurationView(configuration_);
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

	// Create persistent view panel:
	////////////////////////////////////

	persistentPanel_ = new BioXASSidePersistentView();
	persistentPanel_->setFixedWidth(400);
	mw_->addRightWidget(persistentPanel_);
}

void BioXASSideAppController::makeConnections()
{
	connect( BioXASSideBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()) );

	// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
	connect(userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()));
}

void BioXASSideAppController::applyCurrentSettings()
{
	onScalerConnected();
}

void BioXASSideAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)

	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void BioXASSideAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)

	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void BioXASSideAppController::onUserConfigurationLoadedFromDb()
{
	AMXRFDetector *detector = BioXASSideBeamline::bioXAS()->ge32ElementDetector();

	foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){

		AMRegionOfInterest *newRegion = region->createCopy();
		detector->addRegionOfInterest(newRegion);
		configuration_->addRegionOfInterest(region);
	}

	// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
	connect(BioXASSideBeamline::bioXAS()->ge32ElementDetector(), SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(BioXASSideBeamline::bioXAS()->ge32ElementDetector(), SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
//	connect(BioXASSideBeamline::bioXAS()->fourElementVortexDetector(), SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
//	connect(BioXASSideBeamline::bioXAS()->fourElementVortexDetector(), SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
}

void BioXASSideAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	userConfiguration_->addRegionOfInterest(region);
	configuration_->addRegionOfInterest(region);
}

void BioXASSideAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	userConfiguration_->removeRegionOfInterest(region);
	configuration_->removeRegionOfInterest(region);
}
