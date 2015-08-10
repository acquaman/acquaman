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


#include "BioXASMainAppController.h"

#include "acquaman/BioXAS/BioXASMainXASScanConfiguration.h"

#include "beamline/BioXAS/BioXASMainBeamline.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"

#include "ui/BioXAS/BioXASMainXASScanConfigurationView.h"
#include "ui/BioXAS/BioXASMainPersistentView.h"

BioXASMainAppController::BioXASMainAppController(QObject *parent)
	: BioXASAppController(parent)
{
	// Initialize variables.

	m1MirrorView_ = 0;
	monoConfigView_ = 0;
	m2MirrorView_ = 0;
	scalerView_ = 0;
	configuration_ = 0;
	configurationView_ = 0;
	configurationViewHolder_ = 0;

	setDefaultUseLocalStorage(true);
}

bool BioXASMainAppController::startup()
{
	// Get a destination folder.
	if ( !AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-m/AcquamanMainData", "/home/bioxas-m/AcquamanMainData", "users", QStringList()) )
		return false;

	// Start up the main program.
	if(AMAppController::startup()) {

		// Initialize central beamline object
		BioXASMainBeamline::bioXAS();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

			AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::BioXASMainBeamline), CLSFacilityID::BioXASMainBeamline); //7: BioXAS main Beamline
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		setupExporterOptions();
		setupUserInterface();
		makeConnections();
		applyCurrentSettings();

		return true;

	} else {

		return false;
	}
}

void BioXASMainAppController::onScalerConnected()
{
	CLSSIS3820Scaler *scaler = BioXASMainBeamline::bioXAS()->scaler();

	if (scaler && scaler->isConnected() && !scalerView_) {
		scalerView_ = new BioXASSIS3820ScalerView(scaler, true);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Scaler",":/utilities-system-monitor.png", scalerView_), "Detectors", "Scaler", ":/utilities-system-monitor.png", true);
	}
}

void BioXASMainAppController::registerClasses()
{
	BioXASAppController::registerClasses();
	AMDbObjectSupport::s()->registerClass<BioXASMainXASScanConfiguration>();
}

void BioXASMainAppController::setupExporterOptions()
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

	if(bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASMainXASScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(bioXASDefaultXAS->id());
}

void BioXASMainAppController::setupUserInterface()
{
	mw_->setWindowTitle("Acquaman - BioXAS Main");

	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("General", 0);
	mw_->insertHeading("Detectors", 1);
	mw_->insertHeading("Scans", 2);

	// Add widgets to main window panes:
	////////////////////////////////////

	m1MirrorView_ = new BioXASM1MirrorView(BioXASMainBeamline::bioXAS()->m1Mirror());
	mw_->addPane(AMMainWindow::buildMainWindowPane("M1 Mirror", ":/system-software-update.png", m1MirrorView_), "General", "M1 Mirror", ":/system-software-update.png");

	monoConfigView_ = new BioXASSSRLMonochromatorConfigurationView(BioXASMainBeamline::bioXAS()->mono());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Monochromator", ":/system-software-update.png", monoConfigView_), "General", "Monochromator", ":/system-software-update.png");

	m2MirrorView_ = new BioXASM2MirrorView(BioXASMainBeamline::bioXAS()->m2Mirror());
	mw_->addPane(AMMainWindow::buildMainWindowPane("M2 Mirror", ":/system-software-update.png", m2MirrorView_), "General", "M2 Mirror", ":/system-software-update.png");

	jjSlitsView_ = new CLSJJSlitsView(BioXASMainBeamline::bioXAS()->jjSlits());
	mw_->addPane(AMMainWindow::buildMainWindowPane("JJ Slits", ":/system-software-update.png", jjSlitsView_), "General", "JJ Slits", ":/system-software-update.png");

	configuration_ = new BioXASMainXASScanConfiguration();
	configuration_->setEnergy(10000);
	configurationView_ = new BioXASMainXASScanConfigurationView(configuration_);
	configurationViewHolder_ = new AMScanConfigurationViewHolder3("Configure an XAS Scan", true, true, configurationView_);
	connect(configuration_, SIGNAL(totalTimeChanged(double)), configurationViewHolder_, SLOT(updateOverallScanTime(double)));
	configurationViewHolder_->updateOverallScanTime(configuration_->totalTime());
	mw_->addPane(configurationViewHolder_, "Scans", "XAS Scan", ":/utilities-system-monitor.png");

	commissioningConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningConfiguration_);
	commissioningConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Commissioning Tool",true, true, commissioningConfigurationView_);
	mw_->addPane(commissioningConfigurationViewHolder_, "Scans", "Commissioning Tool", ":/utilities-system-monitor.png");

	// Create persistent view panel:
	////////////////////////////////////

	persistentPanel_ = new BioXASMainPersistentView();
	persistentPanel_->setFixedWidth(400);
	mw_->addRightWidget(persistentPanel_);
}

void BioXASMainAppController::makeConnections()
{
	connect( BioXASMainBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()) );
}

void BioXASMainAppController::applyCurrentSettings()
{
	onScalerConnected();
}

void BioXASMainAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}

void BioXASMainAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}
