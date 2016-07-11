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

#include "actions3/actions/AMScanAction.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMLineScanConfiguration.h"
#include "acquaman/SGM/SGMMapScanConfiguration.h"

#include "application/AMAppControllerSupport.h"
#include "application/AMPluginsManager.h"
#include "application/SGM/SGM.h"

#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"
#include "beamline/SGM/energy/SGMEnergyPosition.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"
#include "dataman/AMRun.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/SGM/SGMUserConfiguration.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

#include "ui/AMMainWindow.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/CLS/CLSAMDSScalerView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/CLS/CLSAmptekSDD123DetailedDetectorView.h"
#include "ui/SGM/SGMHexapodView.h"
#include "ui/SGM/SGMPersistentView.h"
#include "ui/SGM/SGMEnergyView.h"
#include "ui/SGM/SGMLaddersView.h"
#include "ui/SGM/SGMSampleChamberView.h"
#include "ui/SGM/SGMXASScanConfigurationView.h"
#include "ui/SGM/SGMLineScanConfigurationView.h"
#include "ui/SGM/SGMMapScanConfigurationView.h"
#include "ui/beamline/AMDetectorView.h"

#include "util/AMErrorMonitor.h"

SGMAppController::SGMAppController(QObject *parent) :
	CLSAppController("SGM", parent)
{
	// Ensure we're using local storage by default.
	setDefaultUseLocalStorage(true);

	sgmUserConfiguration_ = 0;

	generalPaneCategeryName_ = "Components";
	detectorPaneCategoryName_ = "Beamline Detectors";

	amptek1DetectorView_ = 0;
	amptek2DetectorView_ = 0;
	amptek3DetectorView_ = 0;
	amptek4DetectorView_ = 0;
}

bool SGMAppController::startup() {

	// Run all of the Acquaman App startup routines. Some of these are reimplemented in this class.
	if(!CLSAppController::startup())
		return false;

	setupAMDSClientAppController();

	return true;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	CLSAppController::shutdown();
	AMDSClientAppController::releaseAppController();
}

/// ================ implementation of the protected slots =================
void SGMAppController::onInitializeAmptekView()
{
	if(!amptek1DetectorView_) {

		amptek1DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD1());
		amptek1DetectorView_->buildDetectorView();
		amptek1DetectorView_->setEnergyRange(270,2000);
		amptek1DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek1DetectorView_, "Amptek Detectors", "Amptek Detector 1", generalPaneIcon_);
		connect(amptek1DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek2DetectorView_) {

		amptek2DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD2());
		amptek2DetectorView_->buildDetectorView();
		amptek2DetectorView_->setEnergyRange(270,2000);
		amptek2DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek2DetectorView_, "Amptek Detectors", "Amptek Detector 2", generalPaneIcon_);
		connect(amptek2DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek3DetectorView_) {

		amptek3DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD3());
		amptek3DetectorView_->buildDetectorView();
		amptek3DetectorView_->setEnergyRange(270,2000);
		amptek3DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek3DetectorView_, "Amptek Detectors", "Amptek Detector 3", generalPaneIcon_);
		connect(amptek3DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek4DetectorView_) {

		amptek4DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD4());
		amptek4DetectorView_->buildDetectorView();
		amptek4DetectorView_->setEnergyRange(270,2000);
		amptek4DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek4DetectorView_, "Amptek Detectors", "Amptek Detector 4", generalPaneIcon_);
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

void SGMAppController::onUserConfigurationLoadedFromDb()
{
	AMXRFDetector *detector = SGMBeamline::sgm()->amptekSDD1();

	foreach (AMRegionOfInterest *region, sgmUserConfiguration_->regionsOfInterest()){
		detector->addRegionOfInterest(region);
		xasScanConfiguration_->addRegionOfInterest(region);
		lineScanConfiguration_->addRegionOfInterest(region);
		mapScanConfiguration_->addRegionOfInterest(region);
	}

	// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
	connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
}

void SGMAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	sgmUserConfiguration_->addRegionOfInterest(region);
	xasScanConfiguration_->addRegionOfInterest(region);
	lineScanConfiguration_->addRegionOfInterest(region);
	mapScanConfiguration_->addRegionOfInterest(region);
}

void SGMAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	sgmUserConfiguration_->removeRegionOfInterest(region);
	xasScanConfiguration_->removeRegionOfInterest(region);
	lineScanConfiguration_->removeRegionOfInterest(region);
	mapScanConfiguration_->removeRegionOfInterest(region);
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

/// =================== Implementation of protected methods ==========================
bool SGMAppController::setupDataFolder()
{
	// for SGM, the data storage is account based, which doesn't need to pop up the data holder/proposal dialog
	return true;
}

void SGMAppController::initializeBeamline()
{
	// Creates the SGM Beamline object
	SGMBeamline::sgm();
}

void SGMAppController::registerDBClasses()
{
	CLSAppController::registerDBClasses();

	AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SGMLineScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SGMMapScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SGMUserConfiguration>();
}

void SGMAppController::registerExporterOptions()
{
	AMExporterOptionXDIFormat *sgmXASExportOptions = SGM::buildXDIFormatExporterOption("SGMXASDefault", true);
	if(sgmXASExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SGMXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(sgmXASExportOptions->id());

	AMExporterOptionGeneralAscii *sgmLineExportOptions = SGM::buildAsciiFormatExporterOption("SGMLineDefault", true);
	if(sgmLineExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SGMLineScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(sgmLineExportOptions->id());

	AMExporterOptionGeneralAscii *sgmMapExportOptions = SGM::buildAsciiFormatExporterOption("SGMMapDefault", true);
	if(sgmMapExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SGMMapScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(sgmMapExportOptions->id());

}

void SGMAppController::setupScanConfigurations()
{
	// create the commissioning step scan configuration
	commissioningStepConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningStepConfiguration_->setAutoExportEnabled(false);
	commissioningStepConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());

	// create the XAS scan configuration
	xasScanConfiguration_ = new SGMXASScanConfiguration;
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(270);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(320);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(10);
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TEY")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("ConstantFrequency")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("PD")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderUp")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderDown")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodRed")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodBlack")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4")->toInfo());
	xasScanConfiguration_->setI0(SGMBeamline::sgm()->exposedDetectorByName("ConstantFrequency")->toInfo());

	// create the line scan configuration
	lineScanConfiguration_ = new SGMLineScanConfiguration;
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(-1);
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(1);
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(10);
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("ConstantFrequency")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodRed")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodBlack")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3")->toInfo());
//	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4")->toInfo());

	// create the mapping scan configuration
	mapScanConfiguration_ = new SGMMapScanConfiguration;
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0);
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(0.5);
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(10);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(-0.5);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.1);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(0);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(10);
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TEY")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("ConstantFrequency")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("PD")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekROI1")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekROI2")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekROI3")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekROI4")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodRed")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodBlack")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderUp")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderDown")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4")->toInfo());
}

void SGMAppController::setupUserConfiguration()
{
	if (!sgmUserConfiguration_){
		sgmUserConfiguration_ = new SGMUserConfiguration(this);

		// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
		connect(sgmUserConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()));

		if (!sgmUserConfiguration_->loadFromDb(AMDatabase::database("user"), 1)){
			sgmUserConfiguration_->storeToDb(AMDatabase::database("user"));

			AMDetector *detector = SGMBeamline::sgm()->amptekSDD1();
			// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
			connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
			connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
		}

		userConfiguration_ = sgmUserConfiguration_;
	}
}

void SGMAppController::createPersistentView()
{
	SGMPersistentView* persistentView = new SGMPersistentView();
	mw_->addRightWidget(persistentView);
}

void SGMAppController::createGeneralPanes()
{
	CLSAMDSScalerView *amdsScalerView = new CLSAMDSScalerView(SGMBeamline::sgm()->amdsScaler());
	amdsScalerView->setAmplifierViewFormat('g');
	amdsScalerView->setAmplifierViewPrecision(3);
	mw_->addPane(AMMainWindow::buildMainWindowPane("AMDS Scaler", generalPaneIcon_, amdsScalerView),generalPaneCategeryName_, "AMDS Scaler",  generalPaneIcon_);

	SGMHexapodView* hexapodView = new SGMHexapodView(SGMBeamline::sgm()->hexapod());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Hexapod", generalPaneIcon_, hexapodView), generalPaneCategeryName_, "Hexapod", generalPaneIcon_);

	SGMEnergyView* energyView = new SGMEnergyView(SGMBeamline::sgm()->energyControlSet(), SGMEnergyView::Advanced);
	mw_->addPane(AMMainWindow::buildMainWindowPane("Energy", generalPaneIcon_, energyView), generalPaneCategeryName_, "Energy", generalPaneIcon_);

	SGMLaddersView *laddersView = new SGMLaddersView(SGMBeamline::sgm()->xpsLadder(), SGMBeamline::sgm()->bypassLadder(), SGMBeamline::sgm()->xasLadder());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Diagnostics", generalPaneIcon_, laddersView), generalPaneCategeryName_, "Diagnostics", generalPaneIcon_);

	SGMSampleChamberView *sampleChamberView = new SGMSampleChamberView(SGMBeamline::sgm()->sampleChamber());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Sample Chamber", generalPaneIcon_, sampleChamberView), generalPaneCategeryName_, "Sample Chamber", generalPaneIcon_);
}

void SGMAppController::createDetectorPanes()
{
	qe65000DetectorView_ = new AMDetectorGeneralDetailedView(SGMBeamline::sgm()->qe6500Detector());
	mw_->addPane(qe65000DetectorView_, detectorPaneCategoryName_, "SGM QE 65000", generalPaneIcon_);

	// setup the Amptek views
	if(SGMBeamline::sgm()->amptekSDD1() && SGMBeamline::sgm()->amptekSDD1()->isConnected()) {
		onInitializeAmptekView();
	} else if(SGMBeamline::sgm()->amptekSDD1()) {
		connect(SGMBeamline::sgm()->amptekSDD1(), SIGNAL(connected(bool)), this, SLOT(onInitializeAmptekView()));
	}
}

void SGMAppController::createScanConfigurationPanes()
{
	commissioningStepConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningStepConfiguration_, SGMBeamline::sgm()->exposedControls(), SGMBeamline::sgm()->exposedScientificDetectors());
	commissioningStepConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Commissioning Tool", false, true, commissioningStepConfigurationView_);
	mw_->addPane(commissioningStepConfigurationViewHolder_, scanPaneCategoryName_, "Commissioning Tool", scanPaneIcon_);

	xasScanConfigurationView_ = new SGMXASScanConfigurationView(xasScanConfiguration_, SGMBeamline::sgm()->exposedScientificDetectors());
	xasScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("XAS", false, true, xasScanConfigurationView_);
	mw_->addPane(xasScanConfigurationViewHolder_, scanPaneCategoryName_, "XAS", scanPaneIcon_);

	lineScanConfigurationView_ = new SGMLineScanConfigurationView(lineScanConfiguration_, SGMBeamline::sgm()->hexapodControlSet(), SGMBeamline::sgm()->exposedScientificDetectors());
	lineScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Line", false, true, lineScanConfigurationView_);
	mw_->addPane(lineScanConfigurationViewHolder_, scanPaneCategoryName_, "Line", scanPaneIcon_);

	mapScanConfigurationView_ = new SGMMapScanConfigurationView(mapScanConfiguration_, SGMBeamline::sgm()->exposedScientificDetectors());
	mapScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Line", false, true, mapScanConfigurationView_);
	mw_->addPane(mapScanConfigurationViewHolder_, scanPaneCategoryName_, "Mapping", scanPaneIcon_);
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
	sgmUserConfiguration_->storeToDb(AMDatabase::database("user"));
}

void SGMAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	const AMScanActionInfo *actionInfo = qobject_cast<const AMScanActionInfo *>(action->info());
	const AMGenericContinuousScanConfiguration *sgmScanConfig = dynamic_cast<const AMGenericContinuousScanConfiguration *>(actionInfo->configuration());

	if (sgmScanConfig){

		sgmUserConfiguration_->storeToDb(AMDatabase::database("user"));
	}
}

