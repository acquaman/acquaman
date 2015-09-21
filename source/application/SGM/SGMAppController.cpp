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


#include "SGMAppController.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/AMRun.h"
#include "beamline/CLS/CLSFacilityID.h"
#include "ui/SGM/SGMPersistentView.h"
#include "ui/AMMainWindow.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "util/AMErrorMonitor.h"
#include "beamline/SGM/monochromator/SGMMonochromatorInfo.h"
#include "ui/SGM/SGMHexapodView.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "ui/CLS/CLSAmptekSDD123DetailedDetectorView.h"

#include <stdlib.h> // Used for obtaining username to prevent users other than iain (for dev) or SGM-Upgrade (for commissioning). Remove for deploy.

SGMAppController::SGMAppController(QObject *parent) :
	AMAppController(parent)
{
}

bool SGMAppController::startup() {

	QString currentUser = getenv("USER");

	if(currentUser != "workmai" && currentUser != "sgm-upgrade") {
		AMErrorMon::error(this, SGMAPPCONTROLLER_WRONG_USER, "This user account is not permitted to run the SGM Upgrade version of Acquaman.");
		return false;
	}

	// Run all of the Acquaman App startup routines. Some of these are reimplemented in this class.
	if(!AMAppController::startup())
		return false;

	// Creates the SGM Beamline object
	SGMBeamline::sgm();

	// Initialize the storage ring.
	CLSStorageRing::sr1();

	// Retrieve the current run or create one if there is none
	AMRun existingRun;
	if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
		// no run yet... let's create one.
		AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::SGMBeamline), CLSFacilityID::SGMBeamline); //3: SGM Beamline
		firstRun.storeToDb(AMDatabase::database("user"));
	}

	registerClasses();
	setupExporterOptions();
	setupUserInterface();
	makeConnections();

	return true;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void SGMAppController::initializeAmptekView()
{
	if(!amptekDetectorView_) {

		amptekDetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD1());
		amptekDetectorView_->buildDetectorView();
		amptekDetectorView_->setEnergyRange(270,2000);
		amptekDetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptekDetectorView_, "Amptek Detector", "Amptek Detector", ":/system-software-update.png");
		connect(amptekDetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
}

void SGMAppController::onAmptekDetectorViewResized()
{
	// Hack solution copied over from the old SGM App Controller.
	// If this timer is not here it doesn't always resize properly.
	QTimer::singleShot(100, this, SLOT(resizeToMinimum()));
}


void SGMAppController::resizeToMinimum()
{
	mw_->resize(mw_->minimumSizeHint());
}

void SGMAppController::onCurrentScanActionStartedImplementation(AMScanAction */*action*/)
{
}

void SGMAppController::onCurrentScanActionFinishedImplementation(AMScanAction */*action*/)
{
}

void SGMAppController::registerClasses()
{
}

void SGMAppController::setupExporterOptions()
{
}

void SGMAppController::setupUserInterface()
{
	SGMPersistentView* persistentView =
			new SGMPersistentView();

	mw_->addRightWidget(persistentView);

	CLSSIS3820ScalerView* scalerView =
			new CLSSIS3820ScalerView(SGMBeamline::sgm()->scaler());

	mw_->insertHeading("Components", 0);

	mw_->addPane(AMMainWindow::buildMainWindowPane("Scaler", ":/system-software-update.png", scalerView),"Components", "Scaler",  ":/system-software-update.png");

	SGMHexapodView* hexapodView =
			new SGMHexapodView(SGMBeamline::sgm()->hexapod());

	mw_->addPane(AMMainWindow::buildMainWindowPane("Hexapod", ":/system-software-update.png", hexapodView), "Components", "Hexapod", ":/system-software-update.png");

	mw_->insertHeading("Scans", 1);

	commissioningConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningConfiguration_->setAutoExportEnabled(false);
	commissioningConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	commissioningConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningConfiguration_);
	commissioningConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Commissioning Tool", false, true, commissioningConfigurationView_);
	mw_->addPane(commissioningConfigurationViewHolder_, "Scans", "Commissioning Tool", ":/utilities-system-monitor.png");

	amptekDetectorView_ = 0;
	if(SGMBeamline::sgm()->amptekSDD1() && SGMBeamline::sgm()->amptekSDD1()->isConnected()) {
		initializeAmptekView();
	} else if(SGMBeamline::sgm()->amptekSDD1()) {
		connect(SGMBeamline::sgm()->amptekSDD1(), SIGNAL(connected(bool)), this, SLOT(initializeAmptekView()));
	}
}

void SGMAppController::makeConnections()
{
}



