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

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

#include "beamline/BioXAS/BioXASSideBeamline.h"

#include "ui/BioXAS/BioXASSidePersistentView.h"
#include "ui/BioXAS/BioXASSideXASScanConfigurationView.h"
#include "ui/acquaman/CLS/CLSJJSlitScanConfigurationView.h"

BioXASSideAppController::BioXASSideAppController(QObject *parent)
	: BioXASAppController(parent)
{
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

	addComponentView(BioXASSideBeamline::bioXAS()->m1Mirror());
	addComponentView(BioXASSideBeamline::bioXAS()->mono());
	addComponentView(BioXASSideBeamline::bioXAS()->m2Mirror());
	addComponentView(BioXASSideBeamline::bioXAS()->carbonFilterFarm());
	addComponentView(BioXASSideBeamline::bioXAS()->dbhrMirrors());
	addComponentView(BioXASSideBeamline::bioXAS()->xiaFilters());
	addComponentView(BioXASSideBeamline::bioXAS()->jjSlits());
	addComponentView(BioXASSideBeamline::bioXAS()->standardsWheel());
	addComponentView(BioXASSideBeamline::bioXAS()->endstationTable());

	AMMotorGroupView *cryostatStageView = new AMMotorGroupView(BioXASSideBeamline::bioXAS()->motorGroup(), AMMotorGroupView::CompactView);
	mw_->addPane(AMMainWindow::buildMainWindowPane("Cryostat Stage", ":/system-software-update.png", cryostatStageView), "General", "Cryostat Stage", ":/system-software-update.png");

	addDetectorView(BioXASSideBeamline::bioXAS()->scaler());
	addDetectorView(BioXASSideBeamline::bioXAS()->ge32ElementDetector());
//	addDetectorView(BioXASSideBeamline::bioXAS()->fourElementDetector());

	configuration_ = new BioXASSideXASScanConfiguration();
	configuration_->setEnergy(10000);
	addXASScanConfigurationView(configuration_);

	commissioningConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningConfiguration_->setAutoExportEnabled(false);
	addCommissioningScanConfigurationView(commissioningConfiguration_);

	addJJSlitScanConfigurationView(CLSBeamline::clsBeamline()->jjSlits());

	addPersistentView(new BioXASSidePersistentView());
}

void BioXASSideAppController::addXASScanConfigurationView(BioXASSideXASScanConfiguration *configuration)
{
	if (configuration) {
		BioXASSideXASScanConfigurationView *configurationView = new BioXASSideXASScanConfigurationView(configuration);
		AMScanConfigurationViewHolder3 *configurationViewHolder = new AMScanConfigurationViewHolder3("Configure an XAS Scan", true, true, configurationView);

		connect( configuration, SIGNAL(totalTimeChanged(double)), configurationViewHolder, SLOT(updateOverallScanTime(double)) );
		configurationViewHolder->updateOverallScanTime(configuration->totalTime());

		mw_->addPane(configurationViewHolder, "Scans", "XAS Scan", ":/utilities-system-monitor.png");
	}
}

void BioXASSideAppController::addJJSlitScanConfigurationView(CLSJJSlits *jjSlits)
{
	if (jjSlits) {
		CLSJJSlitScanConfiguration *configuration = jjSlits->slitScanConfiguration();
		CLSJJSlitScanConfigurationView *configurationView = new CLSJJSlitScanConfigurationView(configuration);
		AMScanConfigurationViewHolder3 *configurationViewHolder = new AMScanConfigurationViewHolder3("", true, true, configurationView);

		connect( configuration, SIGNAL(totalTimeChanged(double)), configurationViewHolder, SLOT(updateOverallScanTime(double)) );
		configurationViewHolder->updateOverallScanTime(configuration->totalTime());

		mw_->addPane(configurationViewHolder, "Scans", "JJ Slit Scan", ":/utilities-system-monitor.png");
	}
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
