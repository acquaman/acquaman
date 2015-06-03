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

#include "dataman/AMRun.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"

#include "util/AMPeriodicTable.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"

#include "acquaman/BioXAS/BioXASSideXASScanConfiguration.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"

#include "ui/util/AMChooseDataFolderDialog.h"

#include "ui/BioXAS/BioXASSidePersistentView.h"
#include "ui/BioXAS/BioXASSideXASScanConfigurationView.h"

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
		mw_->addPane(createSqueezeGroupBoxWithView(scalerView_, "Scaler", ":/system-search.png"), "Detectors", "Scaler", ":/system-search.png", true);
	}
}

void BioXASSideAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<BioXASSideXASScanConfiguration>();
}

void BioXASSideAppController::setupExporterOptions()
{
	QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "BioXAS Default XAS");

	AMExporterOptionGeneralAscii *bioXASDefaultXAS = new AMExporterOptionGeneralAscii();

	if (matchIDs.count() != 0)
		bioXASDefaultXAS->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

	bioXASDefaultXAS->setName("BioXAS Default XAS");
	bioXASDefaultXAS->setFileName("$name_$fsIndex.dat");
	bioXASDefaultXAS->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
	bioXASDefaultXAS->setHeaderIncluded(true);
	bioXASDefaultXAS->setColumnHeader("$dataSetName $dataSetInfoDescription");
	bioXASDefaultXAS->setColumnHeaderIncluded(true);
	bioXASDefaultXAS->setColumnHeaderDelimiter("");
	bioXASDefaultXAS->setSectionHeader("");
	bioXASDefaultXAS->setSectionHeaderIncluded(true);
	bioXASDefaultXAS->setIncludeAllDataSources(true);
	bioXASDefaultXAS->setFirstColumnOnly(true);
	bioXASDefaultXAS->setIncludeHigherDimensionSources(true);
	bioXASDefaultXAS->setSeparateHigherDimensionalSources(true);
	bioXASDefaultXAS->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
	bioXASDefaultXAS->setHigherDimensionsInRows(true);
	bioXASDefaultXAS->storeToDb(AMDatabase::database("user"));

	if (bioXASDefaultXAS->id() > 0) {
		AMAppControllerSupport::registerClass<BioXASSideXASScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(bioXASDefaultXAS->id());
	}

}

void BioXASSideAppController::setupUserInterface()
{
	mw_->setWindowTitle("Acquaman - BioXAS Side");

	// Create UI elements:
	//////////////////////

	// Set up the general monochromator configuration view.
	monoConfigView_ = new BioXASSSRLMonochromatorConfigurationView(BioXASSideBeamline::bioXAS()->mono());

	// Create JJ slits view.
	jjSlitsView_ = new CLSJJSlitsView(BioXASSideBeamline::bioXAS()->jjSlits());

	// Create XIA filters view.
	xiaFiltersView_ = new BioXASXIAFiltersView(BioXASSideBeamline::bioXAS()->xiaFilters());

	// Create carbon filter farm view.
	carbonFilterFarmView_ = new BioXASCarbonFilterFarmView(BioXASSideBeamline::bioXAS()->carbonFilterFarm());

	// Create m1 mirror view.
	m1MirrorView_ = new BioXASM1MirrorView(BioXASSideBeamline::bioXAS()->m1Mirror());

	// Create m2 mirror view.
	m2MirrorView_ = new BioXASM2MirrorView(BioXASSideBeamline::bioXAS()->m2Mirror());

	// Create DBHR mirror view.
	dbhrView_ = new BioXASDBHRMirrorView(BioXASSideBeamline::bioXAS()->dbhrMirror());

	// Create scaler view, if scaler is present and connected.
	onScalerConnected();

	// Create the 32 element Ge detector view.
//	BioXAS32ElementGeDetectorView *geDetectorView = new BioXAS32ElementGeDetectorView(BioXASSideBeamline::bioXAS()->ge32ElementDetector());
//	geDetectorView->buildDetectorView();
//	geDetectorView->addEmissionLineNameFilter(QRegExp("1"));
//	geDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));

	// Create the four element detector view.
	BioXASFourElementVortexDetectorView *fourElementDetectorView = new BioXASFourElementVortexDetectorView(BioXASSideBeamline::bioXAS()->fourElementVortexDetector());
	fourElementDetectorView->buildDetectorView();
	fourElementDetectorView->setEnergyRange(3000, 28000);
	fourElementDetectorView->addEmissionLineNameFilter(QRegExp("1"));
	fourElementDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));

	// Create right side panel.
	persistentPanel_ = new BioXASSidePersistentView();

	// Add views to 'General'.
	mw_->insertHeading("General", 0);
	mw_->addPane(createSqueezeGroupBoxWithView(m1MirrorView_, "M1 Mirror", ":/system-software-update.png"), "General", "M1 Mirror", ":/system-software-update.png");
	mw_->addPane(createSqueezeGroupBoxWithView(monoConfigView_, "Monochromator", ":/system-software-update.png"), "General", "Monochromator", ":/system-software-update.png");
	mw_->addPane(createSqueezeGroupBoxWithView(jjSlitsView_, "JJ Slits", ":/system-software-update.png"), "General", "JJ Slits", ":/system-software-update.png");
	mw_->addPane(createSqueezeGroupBoxWithView(xiaFiltersView_, "XIA Filters", ":/system-software-update.png"), "General", "XIA Filters", ":/system-software-update.png");
	mw_->addPane(createSqueezeGroupBoxWithView(carbonFilterFarmView_, "Carbon Filter Farm", ":/system-software-update.png"), "General", "Carbon filter farm", ":/system-software-update.png");
	mw_->addPane(createSqueezeGroupBoxWithView(m2MirrorView_, "M2 Mirror", ":/system-software-update.png"), "General", "M2 Mirror", ":/system-software-update.png");
	mw_->addPane(createSqueezeGroupBoxWithView(dbhrView_, "DBHR Mirrors", ":/system-software-update.png"), "General", "DBHR Mirrors", ":/system-software-update.png");

	// Add views to 'Detectors'.
	mw_->insertHeading("Detectors", 1);
//	mw_->addPane(geDetectorView, "Detectors", "Ge 32-el", ":/system-search.png");
	mw_->addPane(fourElementDetectorView, "Detectors", "4-element", ":/system-search.png");

	// Add views to 'Scans'.
	mw_->insertHeading("Scans", 2);

	configuration_ = new BioXASSideXASScanConfiguration();
	configuration_->setEnergy(10000);
	configurationView_ = new BioXASSideXASScanConfigurationView(configuration_);
	configurationViewHolder_ = new AMScanConfigurationViewHolder3("Configure an XAS Scan", true, true, configurationView_);

	mw_->addPane(configurationViewHolder_, "Scans", "XAS Scan", ":/utilities-system-monitor.png");

	connect(configuration_, SIGNAL(totalTimeChanged(double)), configurationViewHolder_, SLOT(updateOverallScanTime(double)));
	configurationViewHolder_->updateOverallScanTime(configuration_->totalTime());

	commissioningConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningConfiguration_->setAutoExportEnabled(false);
	commissioningConfiguration_->addDetector(BioXASSideBeamline::bioXAS()->exposedDetectorByName("I0Detector")->toInfo());
	commissioningConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningConfiguration_);
	commissioningConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("BioXAS Commissioning Tool",true, true, commissioningConfigurationView_);

	mw_->addPane(commissioningConfigurationViewHolder_, "Scans", "Commissioning Tool", ":/utilities-system-monitor.png");

	// Add right side panel.
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
	AMXRFDetector *detector = BioXASSideBeamline::bioXAS()->fourElementVortexDetector();

	foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){

		AMRegionOfInterest *newRegion = region->createCopy();
		detector->addRegionOfInterest(newRegion);
		configuration_->addRegionOfInterest(region);
	}

	// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
//	connect(BioXASSideBeamline::bioXAS()->ge32ElementDetector(), SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
//	connect(BioXASSideBeamline::bioXAS()->ge32ElementDetector(), SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
	connect(BioXASSideBeamline::bioXAS()->fourElementVortexDetector(), SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(BioXASSideBeamline::bioXAS()->fourElementVortexDetector(), SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
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


