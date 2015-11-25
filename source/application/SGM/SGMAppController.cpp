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

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/AMGenericContinuousScanConfiguration.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "beamline/CLS/CLSFacilityID.h"
#include "beamline/SGM/energy/SGMEnergyPosition.h"
#include "dataman/AMRun.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "ui/AMMainWindow.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/acquaman/AMGenericContinuousScanConfigurationView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/CLS/CLSAmptekSDD123DetailedDetectorView.h"
#include "ui/SGM/SGMHexapodView.h"
#include "ui/SGM/SGMPersistentView.h"
#include "ui/SGM/SGMEnergyView.h"
#include "ui/SGM/SGMLaddersView.h"
#include "ui/SGM/SGMXASScanConfigurationView.h"
#include "util/AMErrorMonitor.h"
#include "ui/CLS/CLSAMDSScalerView.h"

#include <stdlib.h> // Used for obtaining username to prevent users other than iain (for dev) or SGM-Upgrade (for commissioning). Remove for deploy.

SGMAppController::SGMAppController(QObject *parent) :
	AMAppController(parent)
{
}

bool SGMAppController::startup() {

	QString currentUser = getenv("USER");

	if(currentUser != "workmai" && currentUser != "sgm-upgrade" && currentUser != "iain" && currentUser != "hunterd" && currentUser != "chevrid" && currentUser != "helfrij" && currentUser != "acquaman") {
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
	setupAMDSClientAppController();
	setupExporterOptions();
	setupUserInterface();
	makeConnections();

	return true;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
	AMDSClientAppController::releaseAppController();
}

void SGMAppController::initializeAmptekView()
{
	if(!amptek1DetectorView_) {

		amptek1DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD1());
		amptek1DetectorView_->buildDetectorView();
		amptek1DetectorView_->setEnergyRange(270,2000);
		amptek1DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek1DetectorView_, "Amptek Detectors", "Amptek Detector 1", ":/system-software-update.png");
		connect(amptek1DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek2DetectorView_) {

		amptek2DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD2());
		amptek2DetectorView_->buildDetectorView();
		amptek2DetectorView_->setEnergyRange(270,2000);
		amptek2DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek2DetectorView_, "Amptek Detectors", "Amptek Detector 2", ":/system-software-update.png");
		connect(amptek2DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek3DetectorView_) {

		amptek3DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD3());
		amptek3DetectorView_->buildDetectorView();
		amptek3DetectorView_->setEnergyRange(270,2000);
		amptek3DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek3DetectorView_, "Amptek Detectors", "Amptek Detector 3", ":/system-software-update.png");
		connect(amptek3DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek4DetectorView_) {

		amptek4DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD4());
		amptek4DetectorView_->buildDetectorView();
		amptek4DetectorView_->setEnergyRange(270,2000);
		amptek4DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek4DetectorView_, "Amptek Detectors", "Amptek Detector 4", ":/system-software-update.png");
		connect(amptek4DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
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

void SGMAppController::connectAMDSServers()
{
	AMDSClientAppController *clientAppController = AMDSClientAppController::clientAppController();
	foreach (AMDSServerConfiguration serverConfiguration, AMDSServerDefs_.values())
		clientAppController->connectToServer(serverConfiguration);
}

void SGMAppController::onAMDSServerConnected(const QString &hostIdentifier)
{
	SGMBeamline::sgm()->configAMDSServer(hostIdentifier);
}

void SGMAppController::setupAMDSClientAppController()
{
	AMDSServerDefs_.insert(QString("AmptekServer"), AMDSServerConfiguration(QString("AmptekServer"), QString("10.52.48.40"), 28044));
	AMDSServerDefs_.insert(QString("ScalerServer"), AMDSServerConfiguration(QString("ScalerServer"), QString("10.52.48.1"), 28044));

	// NOTE: it will be better to move this to CLSBeamline, when
	AMDSClientAppController *AMDSClientController = AMDSClientAppController::clientAppController();
	connect(AMDSClientController, SIGNAL(AMDSClientControllerConnected()), this, SLOT(connectAMDSServers()));
	connect(AMDSClientController, SIGNAL(newServerConnected(QString)), this, SLOT(onAMDSServerConnected(QString)));
	if (AMDSClientController->isSessionOpen()) {
		connectAMDSServers();
	}
}

void SGMAppController::onCurrentScanActionStartedImplementation(AMScanAction */*action*/)
{
}

void SGMAppController::onCurrentScanActionFinishedImplementation(AMScanAction */*action*/)
{
}

void SGMAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration>();
}

void SGMAppController::setupExporterOptions()
{
}
#include "beamline/SGM/energy/SGMEnergyControlSet.h"
void SGMAppController::setupUserInterface()
{
	SGMPersistentView* persistentView =
			new SGMPersistentView();

	mw_->addRightWidget(persistentView);

	CLSAMDSScalerView *amdsScalerView = new CLSAMDSScalerView(SGMBeamline::sgm()->amdsScaler());

	mw_->insertHeading("Components", 0);

	mw_->addPane(AMMainWindow::buildMainWindowPane("AMDS Scaler", ":/system-software-update.png", amdsScalerView),"Components", "AMDS Scaler",  ":/system-software-update.png");

	SGMHexapodView* hexapodView =
			new SGMHexapodView(SGMBeamline::sgm()->hexapod());

	mw_->addPane(AMMainWindow::buildMainWindowPane("Hexapod", ":/system-software-update.png", hexapodView), "Components", "Hexapod", ":/system-software-update.png");

	SGMEnergyView* energyView =
			new SGMEnergyView(SGMBeamline::sgm()->energyControlSet(),
							  SGMEnergyView::Advanced);

	mw_->addPane(AMMainWindow::buildMainWindowPane("Energy", ":/system-software-update.png", energyView), "Components", "Energy", ":/system-software-update.png");

	SGMLaddersView *laddersView =
			new SGMLaddersView(SGMBeamline::sgm()->xpsLadder(), SGMBeamline::sgm()->bypassLadder(), SGMBeamline::sgm()->xasLadder());

	mw_->addPane(AMMainWindow::buildMainWindowPane("Diagnostics", ":/system-software-update.png", laddersView), "Components", "Diagnostics", ":/system-software-update.png");

	mw_->insertHeading("Scans", 1);

	commissioningStepConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningStepConfiguration_->setAutoExportEnabled(false);
	commissioningStepConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	commissioningStepConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningStepConfiguration_, SGMBeamline::sgm()->exposedControls(), SGMBeamline::sgm()->exposedDetectors());
	commissioningStepConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Commissioning Tool", false, true, commissioningStepConfigurationView_);

	commissioningContinuousConfiguration_ = new AMGenericContinuousScanConfiguration;
	commissioningContinuousConfiguration_->setAutoExportEnabled(false);
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TEY")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TFY")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("PD")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD1")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD2")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD3")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD4")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodRed")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodBlack")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderUp")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderDown")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3")->toInfo());
	commissioningContinuousConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4")->toInfo());
	commissioningContinuousConfiguration_->setControl(0, SGMBeamline::sgm()->energyControlSet()->energy()->toInfo());
	commissioningContinuousConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(270);
	commissioningContinuousConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(320);
	commissioningContinuousConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(10);
	commissioningContinuousConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.1);
	commissioningContinuousConfigurationView_ = new AMGenericContinuousScanConfigurationView(commissioningContinuousConfiguration_, SGMBeamline::sgm()->exposedControls(), SGMBeamline::sgm()->exposedDetectors());
	commissioningContinuousConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Continuous Tool", false, true, commissioningContinuousConfigurationView_);

	xasScanConfiguration_ = new SGMXASScanConfiguration;
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TEY")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TFY")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("PD")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD1")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD2")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD3")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD4")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4")->toInfo());
	xasScanConfigurationView_ = new SGMXASScanConfigurationView(xasScanConfiguration_);
	AMDetectorSet *xasScanConfigurationDetectors = new AMDetectorSet(this);
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TEY"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TFY"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("PD"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD1"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD2"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD3"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("FilteredPD4"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3"));
	xasScanConfigurationDetectors->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4"));
	xasScanConfigurationView_->setDetectors(xasScanConfigurationDetectors);
	xasScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("XAS", false, true, xasScanConfigurationView_);

	mw_->addPane(commissioningStepConfigurationViewHolder_, "Scans", "Commissioning Tool", ":/utilities-system-monitor.png");
	mw_->addPane(commissioningContinuousConfigurationViewHolder_, "Scans", "Continuous Tool", ":/utilities-system-monitor.png");
	mw_->addPane(xasScanConfigurationViewHolder_, "Scans", "XAS", ":/utilities-system-monitor.png");

	amptek1DetectorView_ = 0;
	amptek2DetectorView_ = 0;
	amptek3DetectorView_ = 0;
	amptek4DetectorView_ = 0;
	if(SGMBeamline::sgm()->amptekSDD1() && SGMBeamline::sgm()->amptekSDD1()->isConnected()) {
		initializeAmptekView();
	} else if(SGMBeamline::sgm()->amptekSDD1()) {
		connect(SGMBeamline::sgm()->amptekSDD1(), SIGNAL(connected(bool)), this, SLOT(initializeAmptekView()));
	}
}

void SGMAppController::makeConnections()
{
}






