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

#include "application/AMAppControllerSupport.h"

#include "beamline/CLS/CLSFacilityID.h"
#include "beamline/BioXAS/BioXASMainBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h"

#include "analysis/AM1DDarkCurrentCorrectionAB.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/AMRun.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "dataman/BioXAS/BioXASUserConfiguration.h"

#include "acquaman/BioXAS/BioXASMainXASScanConfiguration.h"

#include "util/AMPeriodicTable.h"

#include "ui/AMMainWindow.h"
#include "ui/AMTopFrame.h"

#include "ui/dataman/AMGenericScanEditor.h"

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"

#include "ui/CLS/CLSSIS3820ScalerView.h"

#include "ui/BioXAS/BioXASMainXASScanConfigurationView.h"
#include "ui/BioXAS/BioXASMainPersistentView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerView.h"

#include "ui/AMTopFrame.h"

#include "ui/acquaman/AMGenericStepScanConfigurationView.h"

BioXASMainAppController::BioXASMainAppController(QObject *parent)
	: AMAppController(parent)
{
	// Initialize variables.

	monoConfigView_ = 0;
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

		scalerView_ = new BioXASSIS3820ScalerView(scaler, true);

		QHBoxLayout *horizontalLayout = new QHBoxLayout();
		horizontalLayout->addStretch();
		horizontalLayout->addWidget(scalerView_);
		horizontalLayout->addStretch();

		QVBoxLayout *verticalLayout = new QVBoxLayout();
		verticalLayout->addWidget(new AMTopFrame("Scaler", QIcon(":/utilities-system-monitor.png")));
		verticalLayout->addStretch();
		verticalLayout->addLayout(horizontalLayout);
		verticalLayout->addStretch();

		QGroupBox *scalerBox = new QGroupBox();
		scalerBox->setFlat(true);
		scalerBox->setLayout(verticalLayout);

	mw_->addPane(scalerBox, "Detectors", "Scaler", ":/utilities-system-monitor.png", true);
	}
}

void BioXASMainAppController::onBeamlineConnected()
{
	if (BioXASMainBeamline::bioXAS()->isConnected() && !configurationView_) {

		configuration_ = new BioXASMainXASScanConfiguration();
		configuration_->setEnergy(10000);

		configurationView_ = new BioXASMainXASScanConfigurationView(configuration_);
		configurationViewHolder_ = new AMScanConfigurationViewHolder3(configurationView_);

		mw_->addPane(configurationViewHolder_, "Scans", "XAS Scan", ":/utilities-system-monitor.png");

		commissioningConfiguration_ = new AMGenericStepScanConfiguration;
		commissioningConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningConfiguration_);
		commissioningConfigurationViewHolder_ = new AMScanConfigurationViewHolder3(commissioningConfigurationView_);

		mw_->addPane(commissioningConfigurationViewHolder_, "Scans", "Commissioning Tool", ":/utilities-system-monitor.png");
	}
}

void BioXASMainAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo>();

	AMDbObjectSupport::s()->registerClass<AM1DDarkCurrentCorrectionAB>();

	AMDbObjectSupport::s()->registerClass<BioXASMainXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
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

	monoConfigView_ = new BioXASSSRLMonochromatorConfigurationView(BioXASMainBeamline::bioXAS()->mono());

	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addStretch();
	hLayout->addWidget(monoConfigView_);
	hLayout->addStretch();

	QVBoxLayout *vLayout = new QVBoxLayout();
	vLayout->addWidget(new AMTopFrame("Monochromator", QIcon(":/utilities-system-monitor.png")));
	vLayout->addStretch();
	vLayout->addLayout(hLayout);
	vLayout->addStretch();

	QGroupBox *monoBox = new QGroupBox();
	monoBox->setFlat(true);
	monoBox->setLayout(vLayout);

	mw_->insertHeading("General", 0);
	mw_->addPane(monoBox, "General", "Monochromator", ":/utilities-system-monitor.png");

	mw_->insertHeading("Detectors", 1);

	mw_->insertHeading("Scans", 2);

	// Create persistent view panel and add to right side.
	persistentPanel_ = new BioXASMainPersistentView();
	persistentPanel_->setFixedWidth(320);
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
