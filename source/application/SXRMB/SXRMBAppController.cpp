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


#include "SXRMBAppController.h"

#include "beamline/SXRMB/SXRMBBeamline.h"

#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/AMRun.h"
#include "dataman/SXRMB/SXRMBUserConfiguration.h"
#include "dataman/AMRegionOfInterest.h"

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "ui/SXRMB/SXRMBPersistentView.h"
#include "ui/SXRMB/SXRMBEXAFSScanConfigurationView.h"
#include "ui/SXRMB/SXRMB2DMapScanConfigurationView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

#include "util/AMPeriodicTable.h"

SXRMBAppController::SXRMBAppController(QObject *parent)
	: AMAppController(parent)
{
	userConfiguration_ = new SXRMBUserConfiguration(this);
}

bool SXRMBAppController::startup()
{
	getUserDataFolderFromDialog();

	// Start up the main program.
	if(AMAppController::startup()) {


		// Initialize central beamline object
		SXRMBBeamline::sxrmb();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

						AMRun firstRun("SXRMB", 9);	/// \todo For now, we know that 7 is the ID of the BioXAS main endstation facility, but this is a hardcoded hack.
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		setupExporterOptions();
		setupUserInterface();
		makeConnections();

		if (!userConfiguration_->loadFromDb(AMDatabase::database("user"), 1))
			userConfiguration_->storeToDb(AMDatabase::database("user"));

		return true;
	}
	else
		return false;
}

void SXRMBAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void SXRMBAppController::onBeamlineConnected(bool connected)
{
	if (connected && !exafsScanConfigurationView_) {
		exafsScanConfiguration_ = new SXRMBEXAFSScanConfiguration();

		exafsScanConfigurationView_ = new SXRMBEXAFSScanConfigurationView(exafsScanConfiguration_);
		exafsScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3(exafsScanConfigurationView_);

		mw_->addPane(exafsScanConfigurationViewHolder_, "Scans", "EXAFS Scan", ":/utilites-system-monitor.png");
	}

	if (connected && !microProbe2DScanConfigurationView_) {
		microProbe2DScanConfiguration_ = new SXRMB2DMapScanConfiguration();

		microProbe2DScanConfigurationView_ = new SXRMB2DMapScanConfigurationView(microProbe2DScanConfiguration_);
		microProbe2DScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3(microProbe2DScanConfigurationView_);

		mw_->addPane(microProbe2DScanConfigurationViewHolder_, "Scans", "2D Scan", ":/utilites-system-monitor.png");
	}
}

void SXRMBAppController::onScalerConnected(bool isConnected){
	if(isConnected && SXRMBBeamline::sxrmb()->isConnected()){
		if(!scalerView_)
			scalerView_ = new CLSSIS3820ScalerView(SXRMBBeamline::sxrmb()->scaler());

		mw_->addPane(scalerView_, "Detectors", "Scaler", ":/system-search.png", true);
	}
	else if(scalerView_)
		mw_->removePane(scalerView_);
}

void SXRMBAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<SXRMBScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<SXRMBEXAFSScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SXRMB2DMapScanConfiguration>();
}

void SXRMBAppController::setupExporterOptions()
{

}

void SXRMBAppController::setupUserInterface()
{
	exafsScanConfiguration_ = 0; //NULL
	exafsScanConfigurationView_ = 0; //NULL
	exafsScanConfigurationViewHolder_ = 0; //NULL

	microProbe2DScanConfiguration_ = 0; //NULL
	microProbe2DScanConfigurationView_ = 0; //NULL
	microProbe2DScanConfigurationViewHolder_ = 0; //NULL

	scalerView_ = 0; //NULL

	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("General", 0);

	mw_->insertHeading("Detectors", 1);

	AMXRFDetailedDetectorView *brukerView = new AMXRFDetailedDetectorView(SXRMBBeamline::sxrmb()->brukerDetector());
	brukerView->buildDetectorView();
	brukerView->setEnergyRange(3000, 20000);
	brukerView->addEmissionLineNameFilter(QRegExp("1"));
	brukerView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	brukerView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));

	mw_->addPane(brukerView, "Detectors", "Bruker", ":/system-search.png");

	mw_->insertHeading("Scans", 2);

	sxrmbPersistentView_ = new SXRMBPersistentView();
	mw_->addRightWidget(sxrmbPersistentView_);

	connect(SXRMBBeamline::sxrmb(), SIGNAL(connected(bool)), this, SLOT(onBeamlineConnected(bool)));
	connect(SXRMBBeamline::sxrmb()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));

	if(SXRMBBeamline::sxrmb()->isConnected()){
		onBeamlineConnected(true);
		if(SXRMBBeamline::sxrmb()->scaler()->isConnected())
			onScalerConnected(true);
	}
}

void SXRMBAppController::makeConnections()
{
	// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
	connect(userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()));
}

void SXRMBAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void SXRMBAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void SXRMBAppController::onUserConfigurationLoadedFromDb()
{
	AMXRFDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();

	foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest())
		detector->addRegionOfInterest(region->createCopy());

	// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
	connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
}

void SXRMBAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	userConfiguration_->addRegionOfInterest(region);
	microProbe2DScanConfiguration_->addRegionOfInterest(region);
	exafsScanConfiguration_->addRegionOfInterest(region);
}

void SXRMBAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	userConfiguration_->removeRegionOfInterest(region);
	microProbe2DScanConfiguration_->removeRegionOfInterest(region);
	exafsScanConfiguration_->removeRegionOfInterest(region);
}
