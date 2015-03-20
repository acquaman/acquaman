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

#include "beamline/BioXAS/BioXASMainBeamline.h"

#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"

#include "source/dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/AMRun.h"

#include "util/AMPeriodicTable.h"


#include "ui/BioXAS/BioXASMainPersistentView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "acquaman/BioXAS/BioXASMainXASScanConfiguration.h"
#include "ui/BioXAS/BioXASMainXASScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h"

BioXASMainAppController::BioXASMainAppController(QObject *parent)
	: AMAppController(parent)
{
    // Initialize variables.

    scalerView_ = 0;
    configuration_ = 0;
    configurationView_ = 0;
    configurationViewHolder_ = 0;
}

bool BioXASMainAppController::startup()
{
	getUserDataFolderFromDialog();

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

			AMRun firstRun("BioXASMain", 7);	/// \todo For now, we know that 7 is the ID of the BioXAS main endstation facility, but this is a hardcoded hack.
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

void BioXASMainAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void BioXASMainAppController::onScalerConnected()
{
	CLSSIS3820Scaler *scaler = BioXASMainBeamline::bioXAS()->scaler();

    if (scaler && scaler->isConnected() && !scalerView_) {
	scalerView_ = new CLSSIS3820ScalerView(scaler, false);
	mw_->addPane(scalerView_, "Detectors", "Scaler", ":/system-search.png", true);
    }
}

void BioXASMainAppController::onBeamlineConnected()
{
    if (BioXASMainBeamline::bioXAS()->isConnected() && !configurationView_) {
	configuration_ = new BioXASMainXASScanConfiguration();
        configuration_->setEdgeEnergy(10000);

	configurationView_ = new BioXASMainXASScanConfigurationView(configuration_);

        configurationViewHolder_ = new AMScanConfigurationViewHolder3(configurationView_);

        mw_->addPane(configurationViewHolder_, "Scans", "Test Scan", ":/utilities-system-monitor.png");
    }
}

void BioXASMainAppController::registerClasses()
{
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
	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("General", 0);

	// Create mono configuration view and add to 'General' category.
	monoConfigView_ = new BioXASSSRLMonochromatorConfigurationView(BioXASMainBeamline::bioXAS()->mono());
	mw_->addPane(monoConfigView_, "General", "Mono", ":/system-software-update.png");

	mw_->insertHeading("Detectors", 1);

	mw_->insertHeading("Scans", 2);

	// Create persistent view panel and add to right side.
	persistentPanel_ = new BioXASMainPersistentView();
	mw_->addRightWidget(persistentPanel_);
}

void BioXASMainAppController::makeConnections()
{
    connect( BioXASMainBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()) );
    connect( BioXASMainBeamline::bioXAS(), SIGNAL(connected(bool)), this, SLOT(onBeamlineConnected()) );
}

void BioXASMainAppController::applyCurrentSettings()
{
    onScalerConnected();
    onBeamlineConnected();
}

void BioXASMainAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}

void BioXASMainAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}
