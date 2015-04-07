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

#include "beamline/CLS/CLSFacilityID.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"

#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/AMRun.h"

#include "util/AMPeriodicTable.h"

#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "ui/BioXAS/BioXASSidePersistentView.h"
#include "acquaman/BioXAS/BioXASSideXASScanConfiguration.h"
#include "ui/BioXAS/BioXASSideXASScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "ui/BioXAS/BioXAS32ElementGeDetectorView.h"
#include "acquaman/BioXAS/BioXASXRFScanConfiguration.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h"

BioXASSideAppController::BioXASSideAppController(QObject *parent)
	: AMAppController(parent)
{
	scalerView_ = 0;
	persistentPanel_ = 0;
	configuration_ = 0;
	configurationView_ = 0;
	configurationViewHolder_ = 0;
}

bool BioXASSideAppController::startup()
{
	getUserDataFolderFromDialog();

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
	if (BioXASSideBeamline::bioXAS()->scaler()->isConnected() && !scalerView_) {
		scalerView_ = new CLSSIS3820ScalerView(BioXASSideBeamline::bioXAS()->scaler(), false);

		mw_->addPane(scalerView_, "Detectors", "Scaler", ":/system-search.png", true);
	}
}

void BioXASSideAppController::onBeamlineConnected()
{
	if (BioXASSideBeamline::bioXAS()->isConnected() && !configurationView_) {
		configuration_ = new BioXASSideXASScanConfiguration();
		configuration_->setEnergy(10000);
		/*
		configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(10000);
		configuration_->scanAxisAt(0)->regionAt(0)->setRegionStep(1);
		configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(10010);
		configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
		*/

		configurationView_ = new BioXASSideXASScanConfigurationView(configuration_);

		configurationViewHolder_ = new AMScanConfigurationViewHolder3(configurationView_);

		mw_->addPane(configurationViewHolder_, "Scans", "Test Scan", ":/utilities-system-monitor.png");
	}
}

void BioXASSideAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<BioXASSideXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASXRFScanConfiguration>();
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
	// Create UI elements:
	//////////////////////

	// Set up the general monochromator configuration view.
	monoConfigView_ = new BioXASSSRLMonochromatorConfigurationView(BioXASSideBeamline::bioXAS()->mono());

	// Create scaler view, if scaler is present and connected.
	if (BioXASSideBeamline::bioXAS()->scaler()->isConnected()) {
		onScalerConnected();
	}

	// Create the 32 element Ge detector view.
	BioXAS32ElementGeDetectorView *view = new BioXAS32ElementGeDetectorView(BioXASSideBeamline::bioXAS()->ge32ElementDetector());
	view->buildDetectorView();
	view->addEmissionLineNameFilter(QRegExp("1"));
	view->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));

	// Create right side panel.
	persistentPanel_ = new BioXASSidePersistentView();

	// Add views to 'General'.
	mw_->insertHeading("General", 0);
	mw_->addPane(monoConfigView_, "General", "Monochromator", ":/system-software-update.png");

	// Add views to 'Detectors'.
	mw_->insertHeading("Detectors", 1);
	mw_->addPane(view, "Detectors", "Ge 32-el", ":/system-search.png");

	// Add views to 'Scans'.
	mw_->insertHeading("Scans", 2);

	// Add right side panel.

	mw_->addRightWidget(persistentPanel_);
}

void BioXASSideAppController::makeConnections()
{
	connect( BioXASSideBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()) );
	connect( BioXASSideBeamline::bioXAS(), SIGNAL(connected(bool)), this, SLOT(onBeamlineConnected()) );
}

void BioXASSideAppController::applyCurrentSettings()
{
	onScalerConnected();
	onBeamlineConnected();
}

void BioXASSideAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}

void BioXASSideAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}
