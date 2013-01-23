/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "ui/SGM/SGMSampleTransferView.h"
#include "ui/SGM/SGMSampleManipulatorView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/CLS/CLSSynchronizedDwellTimeView.h"
#include "ui/dataman/AMSampleManagementWidget.h"
#include "ui/acquaman/AMScanConfigurationViewHolder.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/SGM/SGMXASScanConfigurationView.h"
#include "ui/SGM/SGMFastScanConfigurationView.h"
#include "ui/SGM/SGMSidebar.h"
#include "ui/SGM/SGMAdvancedControlsView.h"
#include "acquaman/AMScanController.h"
#include "ui/beamline/AMOldDetectorView.h"
#include "ui/beamline/AMSingleControlDetectorView.h"
#include "ui/SGM/SGMMCPDetectorView.h"
#include "ui/CLS/CLSPGTDetectorView.h"
#include "ui/CLS/CLSOceanOptics65000DetectorView.h"
#include "ui/CLS/CLSAmptekSDD123DetectorView.h"
#include "ui/beamline/AMDetectorView.h"

#include "ui/AMMainWindow.h"
#include "ui/AMWorkflowManagerView.h"
#include "actions3/AMActionRunner3.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMRun.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"

#include "application/AMAppControllerSupport.h"

#include "ui/util/SGM/SGMSettingsMasterView.h"
#include "util/SGM/SGMSettings.h"
#include "util/SGM/SGMDacqConfigurationFile.h"
#include "util/SGM/SGMPluginsLocation.h"
#include "application/AMPluginsManager.h"
#include "util/SGM/SGMPeriodicTable.h"
#include "ui/util/AMGithubIssueSubmissionView.h"
#include "ui/AMDatamanStartupSplashScreen.h"

#include "beamline/CLS/CLSProcServManager.h"

#include "dataman/SGM/SGMDbUpgrade1Pt1.h"
#include "dataman/AMDbUpgrade1Pt1.h"
#include "dataman/AMDbUpgrade1Pt2.h"

#include "ui/SGM/SGMPeriodicTableView.h"

SGMAppController::SGMAppController(QObject *parent) :
	AMAppController(parent)
{
	if(!resetFinishedSignal(SGMBeamline::sgm(), SIGNAL(beamlineInitialized())))
		AMErrorMon::alert(this, SGMAPPCONTROLLER_COULD_NOT_RESET_FINISHED_SIGNAL, "Could not reset the finish signal to the SGM Beamline");

	// Prepend the SGM upgrade 1.1 to the list for both the user database and the SGM Beamline database
	AMDbUpgrade *sgm1Pt1SGMDb = new SGMDbUpgrade1Pt1("SGMBeamline", this);

	// Don't need to do SGMBeamline ... that's not the user's responsibility unless we're SGM or fawkes
	QString userName = QDir::fromNativeSeparators(QDir::homePath()).section("/", -1);
	if( !(userName == "sgm" || userName == "fawkes") )
		sgm1Pt1SGMDb->setIsResponsibleForUpgrade(false);


	prependDatabaseUpgrade(sgm1Pt1SGMDb);
	AMDbUpgrade *sgm1Pt1UserDb = new SGMDbUpgrade1Pt1("user", this);
	prependDatabaseUpgrade(sgm1Pt1UserDb);
	AMDbUpgrade *sgm1Pt1ActionsDb = new SGMDbUpgrade1Pt1("actions", this);
	prependDatabaseUpgrade(sgm1Pt1ActionsDb);

	// Append the AM upgrade 1.1 to the list for the SGMBeamline database
	AMDbUpgrade *am1Pt1UserDb = new AMDbUpgrade1Pt1("SGMBeamline", this);
	appendDatabaseUpgrade(am1Pt1UserDb);

	// Append the AM upgrade 1.2 to the list for the SGMBeamline database
	AMDbUpgrade *am1Pt2UserDb = new AMDbUpgrade1Pt2("SGMBeamline", this);
	appendDatabaseUpgrade(am1Pt2UserDb);

	// Add the SGM Beamline database as a source of exporter options
	additionalExporterOptionsDatabases_.append("SGMBeamline");

	checkedBadStartupSettings_ = false;
}

bool SGMAppController::startup() {
	SGMSettings::s()->load();

	// Run all of the Acquaman App startup routines. Some of these are reimplemented in this class.
	if(!AMAppController::startup())
		return false;

	//splashScreen_->setNumberOfStages(splashScreen_->numberOfStages()+1);

	// Places the SGM specific actions in the menu bar
	if(!startupSGMInstallActions())
		return false;

	// Creates the SGM Beamline object
	SGMBeamline::sgm();
	connect(SGMBeamline::sgm(), SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)), this, SLOT(onSGMBeamlineDetectorAvailabilityChanged(AMOldDetector*,bool)));
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("SGM Startup: Waiting for detectors"));
	onSGMBeamlineDetectorAvailabilityChanged(0, false);

	// Retrieve the current run or create one if there is none
	AMRun existingRun;
	if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
		// no run yet... let's create one.
		AMRun firstRun("SGM", 3);	/// \todo For now, we know that 3 is the ID of the SGM facility, but this is a hardcoded hack.
		firstRun.storeToDb(AMDatabase::database("user"));
	}

	procServs_.append(new CLSProcServManager("IOC1611-427", 10004, "Scaler", this));
	procServs_.append(new CLSProcServManager("VIOC1611-446", 10028, "Coordination AddOns", this));

	// Set up the GUI portions of the SGMAcquamanAppController
	if(!setupSGMViews())
		return false;

	// Center the completed GUI on the screen
	mw_->move(0, 0);

	return true;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

bool SGMAppController::startupCreateDatabases(){
	// Create the standard databases in AMAppController (user db)
	if(!AMAppController::startupCreateDatabases())
		return false;

	// Create the SGM beamline database
	AMDatabase* dbSGM = AMDatabase::createDatabase("SGMBeamline", SGMSettings::s()->SGMDataFolder() + "/" + SGMSettings::s()->SGMDatabaseFilename());
	if(!dbSGM) {
		AMErrorMon::alert(this, -701, "Error creating the SGM Database. Please report this problem to the Acquaman developers.");
		return false;
	}

	return true;
}

bool SGMAppController::startupRegisterDatabases(){
	// Call the AMAppController database registrations
	if(!AMAppController::startupRegisterDatabases())
		return false;

	// Grab the SGM Beamline database
	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	// Register the SGM Beamline database
	if(!AMDbObjectSupport::s()->registerDatabase(dbSGM)) {
		AMErrorMon::alert(this, -702, "Error registering the SGM Database. Please report this problem to the Acquaman developers.");
		return false;
	}

	bool success = true;

	// Register the detector and scan classes
	success &= AMDbObjectSupport::s()->registerClass<SGMMCPDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<CLSPGTDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<CLSOceanOptics65000DetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<CLSAmptekSDD123DetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanConfiguration>();

	// Register the detectors to their views
	success &= AMOldDetectorViewSupport::registerClass<AMSingleControlBriefDetectorView, AMSingleControlDetector>();
	success &= AMOldDetectorViewSupport::registerClass<AMSingleReadOnlyControlBriefDetectorView, AMSingleReadOnlyControlDetector>();
	success &= AMOldDetectorViewSupport::registerClass<CLSPGTBriefDetectorView, CLSPGTDetector>();
	success &= AMOldDetectorViewSupport::registerClass<CLSPGTDetailedDetectorView, CLSPGTDetector>();
	success &= AMOldDetectorViewSupport::registerClass<CLSOceanOptics65000BriefDetectorView, CLSOceanOptics65000Detector>();
	success &= AMOldDetectorViewSupport::registerClass<CLSOceanOptics65000DetailedDetectorView, CLSOceanOptics65000Detector>();
	success &= AMOldDetectorViewSupport::registerClass<CLSAmptekSDD123BriefDetectorView, CLSAmptekSDD123Detector>();
	success &= AMOldDetectorViewSupport::registerClass<CLSAmptekSDD123DetailedDetectorView, CLSAmptekSDD123Detector>();

	// Register the configuration file and file loader plugin supports
	success &= AMDbObjectSupport::s()->registerClass<SGMDacqConfigurationFile>();
	success &= AMDbObjectSupport::s()->registerClass<SGMPluginsLocation>();

	// Register the SGM periodic table support classes
	success &= AMDbObjectSupport::s()->registerClass<SGMEnergyPosition>();
	success &= AMDbObjectSupport::s()->registerClass<SGMScanInfo>();
	success &= AMDbObjectSupport::s()->registerClass<SGMElementInfo>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanSettings>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanParameters>();

	return success;
}

bool SGMAppController::startupPopulateNewDatabase(){
	// Call the AMAppController database population methods
	if(!AMAppController::startupPopulateNewDatabase())
		return false;

	bool success = true;

	// Call the SGM setup functions. These are done both for new and existing databases, as these functions check for existance before creating and sometimes alter the existing if need be
	success &= setupSGMConfigurationFiles();
	success &= setupSGMPlugins();
	success &= setupSGMPeriodicTable();
	success &= setupSGMExporterOptions();

	return success;
}

bool SGMAppController::startupLoadFromExistingDatabase(){
	// Call the AMAppController database loading methods
	if(!AMAppController::startupLoadFromExistingDatabase())
		return false;

	bool success = true;

	// Call the SGM setup functions. These are done both for new and existing databases, as these functions check for existance before creating and sometimes alter the existing if need be
	success &= setupSGMConfigurationFiles();
	success &= setupSGMPlugins();
	success &= setupSGMPeriodicTable();
	success &= setupSGMExporterOptions();

	return success;
}

bool SGMAppController::startupAfterEverything(){
	if(isBadDatabaseDirectory_)
		QMessageBox::warning(0, "Poor Choice of Database Folder", "The folder choosen for the user database is really not a good one.\n\nAcquaman will work, but some features may experience problems.\nThis can be fixed, please contact Tom or David to fix this.");

	return AMAppController::startupAfterEverything();
}

void SGMAppController::onCurrentPaneChanged(QWidget *pane) {
	Q_UNUSED(pane)
}

void SGMAppController::onSGMBeamlineConnected(){
	if(SGMBeamline::sgm()->isConnected() && SGMBeamline::sgm()->isReady() && !xasScanConfigurationView_ && !fastScanConfigurationView_){
		SGMXASScanConfiguration *sxsc = new SGMXASScanConfiguration(this);
		sxsc->xasRegions()->setEnergyControl(SGMBeamline::sgm()->energy());
		sxsc->regions()->setDefaultTimeControl(SGMBeamline::sgm()->scalerIntegrationTime());
		double goodEnergy = 10 * floor(SGMBeamline::sgm()->energy()->value() / 10);
		sxsc->addRegion(0, goodEnergy, 1, goodEnergy+10, 1);
		xasScanConfigurationView_ = new SGMXASScanConfigurationView(sxsc);
		xasScanConfigurationHolder3_->setView(xasScanConfigurationView_);
		connect(xasScanConfigurationHolder3_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));

		SGMFastScanConfiguration *sfsc = new SGMFastScanConfiguration(this);
		fastScanConfigurationView_ = new SGMFastScanConfigurationView(sfsc);
		fastScanConfigurationHolder3_->setView(fastScanConfigurationView_);
		connect(fastScanConfigurationHolder3_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));
	}
	else if(!SGMBeamline::sgm()->isConnected() && !xasScanConfigurationView_ && !fastScanConfigurationView_){
		//do nothing
	}
	else if(SGMBeamline::sgm()->isConnected() && !xasScanConfigurationHolder3_->isEnabled() && !fastScanConfigurationHolder3_->isEnabled()){
		xasScanConfigurationHolder3_->setEnabled(true);
		fastScanConfigurationHolder3_->setEnabled(true);
	}
	else if(!SGMBeamline::sgm()->isConnected() && xasScanConfigurationView_->isVisible() && fastScanConfigurationView_->isVisible()){
		xasScanConfigurationHolder3_->setEnabled(false);
		fastScanConfigurationHolder3_->setEnabled(false);
	}

	if(!checkedBadStartupSettings_){
		checkedBadStartupSettings_ = true;
		QString badStartupSettingsMessage = "The beamline seems to have some odd settings, did a fast scan fail?\n\n";

		if(!SGMBeamline::sgm()->gratingVelocity()->withinTolerance(10000))
			badStartupSettingsMessage.append(QString("Grating Velocity %1 versus 10000\n").arg(SGMBeamline::sgm()->gratingVelocity()->value()));
		if(!SGMBeamline::sgm()->gratingBaseVelocity()->withinTolerance(0))
			badStartupSettingsMessage.append(QString("Grating Base Velocity %2 versus 0\n").arg(SGMBeamline::sgm()->gratingBaseVelocity()->value()));
		if(!SGMBeamline::sgm()->gratingAcceleration()->withinTolerance(5000))
			badStartupSettingsMessage.append(QString("Grating Acceleration %3 versus 5000\n").arg(SGMBeamline::sgm()->gratingAcceleration()->value()));
		if(!SGMBeamline::sgm()->rawScaler()->isConnected())
			badStartupSettingsMessage.append("Scaler is not connected\n");
		else if(SGMBeamline::sgm()->rawScaler()->scansPerBuffer() != 1)
			badStartupSettingsMessage.append(QString("Scaler time %1\nScaler Scan per buffer %2 versus 1\nScaler Total Scans %3 versus 1").arg(SGMBeamline::sgm()->rawScaler()->dwellTime()).arg(SGMBeamline::sgm()->rawScaler()->scansPerBuffer()).arg(SGMBeamline::sgm()->rawScaler()->totalScans()));

		if(badStartupSettingsMessage != "The beamline seems to have some odd settings, did a fast scan fail?\n\n"){
			badStartupSettingsMessage.append("\nPress Ok to automatically return to nominal values.");
			QMessageBox badStartupSettingsChoice;
			badStartupSettingsChoice.setInformativeText(badStartupSettingsMessage);
			badStartupSettingsChoice.setText(QString("Questionable Beamline Settings"));
			badStartupSettingsChoice.setIcon(QMessageBox::Question);
			badStartupSettingsChoice.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			badStartupSettingsChoice.setDefaultButton(QMessageBox::Ok);
			badStartupSettingsChoice.setEscapeButton(QMessageBox::Cancel);

			if(badStartupSettingsChoice.exec() == QMessageBox::Ok){
				SGMBeamline::sgm()->gratingVelocity()->move(10000);
				SGMBeamline::sgm()->gratingBaseVelocity()->move(0);
				SGMBeamline::sgm()->gratingAcceleration()->move(5000);
				SGMBeamline::sgm()->rawScaler()->setDwellTime(1.0);
				SGMBeamline::sgm()->rawScaler()->setScansPerBuffer(1);
				SGMBeamline::sgm()->rawScaler()->setTotalScans(1);
				SGMBeamline::sgm()->undulatorTracking()->move(1);
				SGMBeamline::sgm()->exitSlitTracking()->move(1);
			}
		}
	}
}

void SGMAppController::onSGMScalerConnected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->rawScaler() && SGMBeamline::sgm()->rawScaler()->isConnected() && !sgmScalerView_){
		sgmScalerView_ = new CLSSIS3820ScalerView(SGMBeamline::sgm()->scaler());
		mw_->addPane(sgmScalerView_, "Beamline Control", "SGM Scaler", ":/system-software-update.png", true);
	}
}

void SGMAppController::onSGMSynchronizedDwellTimeConnected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->synchronizedDwellTime() && SGMBeamline::sgm()->synchronizedDwellTime()->isConnected() && !sgmSynchronizedDwellTimeView_){
		sgmSynchronizedDwellTimeView_ = new CLSSynchronizedDwellTimeView(SGMBeamline::sgm()->synchronizedDwellTime());
		mw_->addPane(sgmSynchronizedDwellTimeView_, "Beamline Control", "SGM Sync Dwell", ":/system-software-update.png", true);
		sgmSynchronizedDwellTimeView_->setAdvancedViewVisible(true);
	}
}

void SGMAppController::onSGMPGTSDDConnected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->pgtDetector() && SGMBeamline::sgm()->pgtDetector()->isConnected() && !pgtSDDView_){
		pgtSDDView_ = AMOldDetectorViewSupport::createDetailedDetectorView(SGMBeamline::sgm()->pgtDetector());
		mw_->addPane(pgtSDDView_, "Beamline Detectors", "SGM PGT", ":/system-software-update.png");
	}
}

void SGMAppController::onSGMOceanOpticsSpectrometerConnected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->oos65000Detector() && SGMBeamline::sgm()->oos65000Detector()->isConnected() && !oceanOpticsSpectrometerView_){
		oceanOpticsSpectrometerView_ = AMOldDetectorViewSupport::createDetailedDetectorView(SGMBeamline::sgm()->oos65000Detector());
		mw_->addPane(oceanOpticsSpectrometerView_, "Beamline Detectors", "SGM QE65000", ":/system-software-update.png");
	}
}

void SGMAppController::onSGMAmptekSDD1Connected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->amptekSDD1() && SGMBeamline::sgm()->amptekSDD1()->isConnected() && ! amptekSDD1View_){
		amptekSDD1View_ = AMOldDetectorViewSupport::createDetailedDetectorView(SGMBeamline::sgm()->amptekSDD1());
		mw_->addPane(amptekSDD1View_, "Beamline Detectors", "SGM Amptek1", ":/system-software-update.png");
	}
}

void SGMAppController::onSGMAmptekSDD2Connected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->amptekSDD2() && SGMBeamline::sgm()->amptekSDD2()->isConnected() && ! amptekSDD2View_){
		amptekSDD2View_ = AMOldDetectorViewSupport::createDetailedDetectorView(SGMBeamline::sgm()->amptekSDD2());
		mw_->addPane(amptekSDD2View_, "Beamline Detectors", "SGM Amptek2", ":/system-software-update.png");
	}
}

void SGMAppController::onSGMNewAmptekSDD1Connected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->newAmptekSDD1() && SGMBeamline::sgm()->newAmptekSDD1()->isConnected() && !newAmptekSDD1View_){
		newAmptekSDD1View_ = new AMDetectorView(SGMBeamline::sgm()->newAmptekSDD1());
		mw_->addPane(newAmptekSDD1View_, "Beamline Detectors", "NEW SGM Amptek1", ":/system-software-update.png");
	}
}

#include "dataman/AMScanEditorModelItem.h"
#include "ui/dataman/AMGenericScanEditor.h"

void SGMAppController::onCurrentScanControllerCreated(){
}

void SGMAppController::onCurrentScanControllerDestroyed(){
}

void SGMAppController::onCurrentScanControllerStarted(AMScanAction *action){
	connect(AMActionRunner3::workflow(), SIGNAL(currentActionProgressChanged(double,double)), this, SLOT(onProgressUpdated(double,double)));

	AMScan *scan = action->controller()->scan();
	openScanInEditor(scan);

	SGMXASScanConfiguration *xasConfig = qobject_cast<SGMXASScanConfiguration *>(scan->scanConfiguration());
	if(xasConfig){
		if(xasConfig->allDetectorConfigurations().isActiveNamed("teyScaler") || xasConfig->allDetectorConfigurations().isActiveNamed("teyPico"))
			scanEditorAt(scanEditorCount()-1)->setExclusiveDataSourceByName("TEYNorm");
		else if(xasConfig->allDetectorConfigurations().isActiveNamed("tfyScaler") || xasConfig->allDetectorConfigurations().isActiveNamed("tfyPico"))
			scanEditorAt(scanEditorCount()-1)->setExclusiveDataSourceByName("TFYNorm");
		else
			scanEditorAt(scanEditorCount()-1)->setExclusiveDataSourceByName("I0");
		return;
	}

	SGMFastScanConfiguration *fastConfig = qobject_cast<SGMFastScanConfiguration *>(scan->scanConfiguration());
	if(fastConfig){
		scanEditorAt(scanEditorCount()-1)->setExclusiveDataSourceByName("TEY");
		return;
	}
}

void SGMAppController::onCurrentScanControllerFinished(AMScanAction *action){
	Q_UNUSED(action)
	disconnect(AMActionRunner3::workflow(), SIGNAL(currentActionProgressChanged(double,double)), this, SLOT(onProgressUpdated(double,double)));
}

void SGMAppController::onActionSGMSettings(){
	if(!sgmSettingsMasterView_)
		sgmSettingsMasterView_ = new SGMSettingsMasterView();
	sgmSettingsMasterView_->show();
}

void SGMAppController::onActionProcServManager(){
	if(!procServsView_)
		procServsView_ = new CLSProcServManagerView(procServs_);
	procServsView_->show();
}

void SGMAppController::onSGMBeamlineDetectorAvailabilityChanged(AMOldDetector *detector, bool isAvailable){
	Q_UNUSED(detector)
	Q_UNUSED(isAvailable)
	QStringList waitingForDetectorNames;
	QList<AMOldDetector*> possibleDetectors = SGMBeamline::sgm()->possibleDetectorsForSet(SGMBeamline::sgm()->allDetectors());
	for(int x = 0; x < possibleDetectors.count(); x++)
		if(!possibleDetectors.at(x)->isConnected())
			waitingForDetectorNames.append(possibleDetectors.at(x)->toInfo()->description());

	QString waitingDetectors = QString("Waiting for:\n %1").arg(waitingForDetectorNames.join("\n"));

	if(lastWaitingDetectors_ != waitingDetectors){
		lastWaitingDetectors_ = waitingDetectors;
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MODECHANGE,  "Waiting");
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_SUBTEXT,  lastWaitingDetectors_);
		qApp->processEvents();
	}
}

bool SGMAppController::startupSGMInstallActions(){
	QAction *sgmSettingAction = new QAction("SGM Beamline Settings...", mw_);
	sgmSettingAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_T));
	sgmSettingAction->setStatusTip("View or Change SGM Beamline Settings");
	connect(sgmSettingAction, SIGNAL(triggered()), this, SLOT(onActionSGMSettings()));

	sgmSettingsMasterView_ = 0;

	QAction *sgmProcServAction = new QAction("SGM Proc Servs...", mw_);
	sgmProcServAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_Y));
	sgmProcServAction->setStatusTip("Restart stuck servers");
	connect(sgmProcServAction, SIGNAL(triggered()), this, SLOT(onActionProcServManager()));

	procServsView_ = 0;

	fileMenu_->addSeparator();
	fileMenu_->addAction(sgmSettingAction);
	fileMenu_->addAction(sgmProcServAction);

	return true;
}

bool SGMAppController::setupSGMConfigurationFiles()
{
	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	QList<int> matchIDs;
	SGMDacqConfigurationFile *configFile;
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "FastScaler");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("FastScaler");
		configFile->setConfigurationFileName("Scalar_Fast.config");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}

	return success;
}

bool SGMAppController::setupSGMPlugins()
{
	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	QList<int> matchIDs;

	SGMPluginsLocation *fileLoaderPluginsLocation, *analysisBlockPluginsLocation;
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "FileLoaders");
	fileLoaderPluginsLocation = new SGMPluginsLocation();
	if(matchIDs.count() == 0){
		fileLoaderPluginsLocation->setName("FileLoaders");
		fileLoaderPluginsLocation->setPluginFolderPath("/home/sgm/beamline/programming/acquaman/plugins/FileLoaders");
		success &= fileLoaderPluginsLocation->storeToDb(dbSGM);
	}
	else
		success &= fileLoaderPluginsLocation->loadFromDb(dbSGM, matchIDs.at(0));
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "AnalysisBlocks");
	analysisBlockPluginsLocation = new SGMPluginsLocation();
	if(matchIDs.count() == 0){
		analysisBlockPluginsLocation->setName("AnalysisBlocks");
		analysisBlockPluginsLocation->setPluginFolderPath("/home/sgm/beamline/programming/acquaman/plugins/AnalysisBlocks");
		success &= analysisBlockPluginsLocation->storeToDb(dbSGM);
	}
	else
		success &= analysisBlockPluginsLocation->loadFromDb(dbSGM, matchIDs.at(0));
	if(success)
		AMPluginsManager::s()->loadApplicationPlugins(fileLoaderPluginsLocation->pluginFolderPath(), analysisBlockPluginsLocation->pluginFolderPath());

	return success;
}

bool SGMAppController::setupSGMExporterOptions(){
	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "SGMDefault");

	if(!dbSGM->isReadOnly()){
		AMExporterOptionGeneralAscii *sgmDefault;
		// Don't have one called "SGMDefault", so make one. If we have one, retreive it and check it.
		sgmDefault = new AMExporterOptionGeneralAscii();
		if(matchIDs.count() != 0)
			sgmDefault->loadFromDb(dbSGM, matchIDs.at(0));
		sgmDefault->setName("SGMDefault");
		sgmDefault->setFileName("$name_$fsIndex.txt");
		sgmDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\nGrating: $scanConfiguration[grating%enumConvert]\nHarmonic: $scanConfiguration[harmonic%enumConvert]\nExit Slit Gap: $scanConfiguration[exitSlitGap%double%2] um");
		sgmDefault->setHeaderIncluded(true);
		sgmDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
		sgmDefault->setColumnHeaderIncluded(true);
		sgmDefault->setColumnHeaderDelimiter("==========");
		sgmDefault->setSectionHeader("");
		sgmDefault->setSectionHeaderIncluded(true);
		//sgmDefault->setIncludeAllDataSources(false);
		if(sgmDefault->dataSources().count() > 0 && sgmDefault->dataSources().at(0) == "EnergyFeedback")
			sgmDefault->removeDataSourceAt(0);
		sgmDefault->ensureDataSource("I0", false, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("EnergyFeedback", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("Photodiode", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("TEY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("TFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("TEYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("TFYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("PFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("IPFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("SDD", false, AMExporterOptionGeneral::SeparateFilesMode, false);
		sgmDefault->ensureDataSource("OceanOptics65000", false, AMExporterOptionGeneral::SeparateFilesMode, false);
		sgmDefault->ensureDataSource("PLY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("PLYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.txt");
		sgmDefault->setIncludeAllDataSources(true);
		sgmDefault->setFirstColumnOnly(true);
		sgmDefault->setSeparateHigherDimensionalSources(true);
		sgmDefault->storeToDb(dbSGM);
	}
	else{
		//Should I do a check of some sort to make sure this is up to date?
	}

	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "SGMDefault");

	if(matchIDs.count() > 0){
		success &= AMAppControllerSupport::registerClass<SGMXASScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(matchIDs.at(0), dbSGM->connectionName());
		success &= AMAppControllerSupport::registerClass<SGMFastScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(matchIDs.at(0), dbSGM->connectionName());
	}
	else
		return false;

	return success;
}

bool SGMAppController::setupSGMPeriodicTable(){
	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	QList<int> matchIDs;

	QString elementSymbol;
	QString elementName;
	QString elementEdge;

	// CARBON SETTINGS
	elementSymbol = "C";
	elementName = "Carbon";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 270.0, -397720, -149991, 286.63, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 295.0, -377497, -140470, 200.46, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 320.0, -348005, -133061, 100.54, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 40000, 5.0, 200, 6000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 5800, 20.0, 200, 970);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// NITROGEN SETTINGS
	elementSymbol = "N";
	elementName = "Nitrogen";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 380.0, -293057, -115327, 1.37942, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 410.0, -268341, -105051, 3.88, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 440.0, -253095, -99461, 15.5151, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 10000, 5.0, 200, 1500);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 3800, 20.0, 800, 825);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// OXYGEN SETTINGS
	elementSymbol = "O";
	elementName = "Oxygen";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 530.0, -210116, -77335, 166.586, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 545.0, -204333, -73988, 198.591, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 560.0, -198860, -70692, 232.356, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 10000, 5.0, 200, 1600);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1100, 20.0, 800, 330);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// CALCIUM SETTINGS
	elementSymbol = "Ca";
	elementName = "Calcium";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 345.0, -322785, -125202, 1.0, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 355.0, -313695, -122224, 1.0, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 365.0, -305100, -119343, 1.0, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// TITANIUM SETTINGS
	elementSymbol = "Ti";
	elementName = "Titanium";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 450.0, -247469, -96777, 1.0, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 465.0, -239490, -93072, 1.0, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 480.0, -232004, -89479, 21.226, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// CHROMIUM SETTINGS
	elementSymbol = "Cr";
	elementName = "Chromium";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 565.0, -197101, -70227, 189.973, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 585.0, -190363, -65922, 238.105, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 605.0, -184068, -61689, 288.669, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// IRON SETTINGS
	elementSymbol = "Fe";
	elementName = "Iron";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 690.0, -295012, -46140, 70.26, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 720.0, -282721, -40231, 70.39, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 750.0, -271411, -34383, 77.68, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 2400, 20.0, 600, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// COBALT SETTINGS
	elementSymbol = "Co";
	elementName = "Cobalt";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 750.0, -271410, -34592, 77.70, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 780.0, -260974, -28793, 91.09, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 820.0, -248243, -21152, 116.99, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		//SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		//success &= fs5Settings.storeToDb(dbSGM);
		//SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		//success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 2000, 20.0, 800, 700);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		//elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// NICKEL SETTINGS
	elementSymbol = "Ni";
	elementName = "Nickel";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 840.0, -242330, -15364, 78.917, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 865.0, -235326, -10614, 101.285, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 890.0, -228716, -5858, 126.123, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// COPPER SETTINGS
	elementSymbol = "Cu";
	elementName = "Copper";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 920.0, -221259, 1300, 218.584, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 950.0, -214272, 3318, 230.292, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 970.0, -209854, 9900, 242.299, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 3000, 5.0, 200, 1000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1100, 20.0, 800, 510);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// ZINC SETTINGS
	elementSymbol = "Zn";
	elementName = "Zinc";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1010.0, -201543, 17796, 326.886, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1035.0, -196675, 22723, 362.103, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1060.0, -192036, 27678, 398.628, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 6000, 5.0, 200, 3500);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 850, 20.0, 800, 600);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// SODIUM SETTINGS
	elementSymbol = "Na";
	elementName = "Sodium";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1050.0, -193868, 25085, 329.87, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1080.0, -188479, 31109, 374.72, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1110.0, -183388, 37235, 421.201, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 24000, 5.0, 200, 4000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 4800, 20.0, 200, 820);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// MAGNESIUM SETTINGS
	elementSymbol = "Mg";
	elementName = "Magnesium";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1290.0, -249736, -101803, 91.21, 2);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1315.0, -239101, -99682, 105.38, 2);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1340.0, -234638, -97582, 120.65, 2);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 24000, 5.0, 200, 4000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 4800, 20.0, 200, 820);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// ALUMINUM SETTINGS
	elementSymbol = "Al";
	elementName = "Aluminum";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1550.0, -202852, -80942, 283.82, 2);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1580.0, -198998, -78694, 311.23, 2);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1610.0, -195288, -76434, 339.47, 2);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 24000, 5.0, 200, 4000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 800, 20.0, 200, 250);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// SILICON SETTINGS
	elementSymbol = "Si";
	elementName = "Silicon";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1830.0, -171811, -60656, 567.57, 2);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1860.0, -169041, -58574, 601.08, 2);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1890.0, -166359, -56493, 635.07, 2);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		QPair<QString, QString> edgeEnergyPair = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, qMakePair(elementEdge, edgeEnergyPair.second.toDouble()), epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 24000, 5.0, 200, 4000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 580, 20.0, 200, 220);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	return success;
}

bool SGMAppController::setupSGMViews(){
	// Create panes in the main window:
	mw_->insertHeading("Beamline Control", 0);
	samplePositionView_ = new AMSampleManagementWidget(new SGMSampleManipulatorView(),
							   QUrl("http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg"),
							   "Sample Camera",
							   SGMBeamline::sgm()->currentSamplePlate(),
							   SGMBeamline::sgm()->sampleManipulator());
	mw_->addPane(samplePositionView_, "Beamline Control", "SGM Sample Position", ":/system-software-update.png");
	connect(samplePositionView_, SIGNAL(newSamplePlateSelected(AMSamplePlate*)), SGMBeamline::sgm(), SLOT(setCurrentSamplePlate(AMSamplePlate*)));

	// Jan 11, 2013: I don't think this is necessary at all anymore
	//connect(SGMBeamline::sgm(), SIGNAL(currentSamplePlateChanged(AMSamplePlate*)), workflowManagerView_, SLOT(setCurrentSamplePlate(AMSamplePlate*)));

	SGMAdvancedControls_ = new SGMAdvancedControlsView();
	mw_->addPane(SGMAdvancedControls_, "Beamline Control", "SGM Advanced Controls", ":/system-software-update.png");

	//sampleTransferView_ = new SGMSampleTransferView();
	//mw_->addPane(sampleTransferView_, "Beamline Control", "SGM Sample Transfer", ":/system-software-update.png");

	sgmScalerView_ = 0;
	connect(SGMBeamline::sgm()->rawScaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onSGMScalerConnected(bool)));
	onSGMScalerConnected(false);
	sgmSynchronizedDwellTimeView_ = 0;
	connect(SGMBeamline::sgm()->synchronizedDwellTime(), SIGNAL(connected(bool)), this, SLOT(onSGMSynchronizedDwellTimeConnected(bool)));
	onSGMSynchronizedDwellTimeConnected(false);

	mw_->insertHeading("Beamline Detectors", 1);

	pgtSDDView_ = 0;
	connect(SGMBeamline::sgm()->pgtDetector()->signalSource(), SIGNAL(connected(bool)), this, SLOT(onSGMPGTSDDConnected(bool)));
	onSGMPGTSDDConnected(false);
	oceanOpticsSpectrometerView_ = 0;
	connect(SGMBeamline::sgm()->oos65000Detector()->signalSource(), SIGNAL(connected(bool)), this, SLOT(onSGMOceanOpticsSpectrometerConnected(bool)));
	onSGMOceanOpticsSpectrometerConnected(false);
	amptekSDD1View_ = 0;
	connect(SGMBeamline::sgm()->amptekSDD1()->signalSource(), SIGNAL(connected(bool)), this, SLOT(onSGMAmptekSDD1Connected(bool)));
	onSGMAmptekSDD1Connected(false);
	amptekSDD2View_ = 0;
	connect(SGMBeamline::sgm()->amptekSDD2()->signalSource(), SIGNAL(connected(bool)), this, SLOT(onSGMAmptekSDD2Connected(bool)));
	onSGMAmptekSDD2Connected(false);

	newAmptekSDD1View_ = 0;
	connect(SGMBeamline::sgm()->newAmptekSDD1(), SIGNAL(connected(bool)), this, SLOT(onSGMNewAmptekSDD1Connected(bool)));
	onSGMNewAmptekSDD1Connected(false);

	mw_->sidebar()->setExpanded(mw_->windowPaneModel()->headingItem("Beamline Detectors")->index(), false);

	mw_->insertHeading("Experiment Setup", 2);
	xasScanConfigurationView_ = 0; //NULL
	xasScanConfigurationHolder3_ = new AMScanConfigurationViewHolder3();
	mw_->addPane(xasScanConfigurationHolder3_, "Experiment Setup", "SGM XAS Scan", ":/utilities-system-monitor.png");

	fastScanConfigurationView_ = 0; //NULL
	fastScanConfigurationHolder3_ = new AMScanConfigurationViewHolder3();
	mw_->addPane(fastScanConfigurationHolder3_, "Experiment Setup", "SGM Fast Scan", ":/utilities-system-monitor.png");

	connect(xasScanConfigurationHolder3_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));
	connect(fastScanConfigurationHolder3_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));

	connect(AMActionRunner3::workflow(), SIGNAL(scanActionStarted(AMScanAction*)), this, SLOT(onCurrentScanControllerStarted(AMScanAction*)));
	connect(AMActionRunner3::workflow(), SIGNAL(scanActionFinished(AMScanAction*)), this, SLOT(onCurrentScanControllerFinished(AMScanAction*)));

	connect(SGMBeamline::sgm(), SIGNAL(beamlineInitialized()), this, SLOT(onSGMBeamlineConnected()));
	connect(SGMBeamline::sgm(), SIGNAL(criticalConnectionsChanged()), this, SLOT(onSGMBeamlineConnected()));
	connect(SGMBeamline::sgm(), SIGNAL(beamlineReadyChanged()), this, SLOT(onSGMBeamlineConnected()));
	onSGMBeamlineConnected();

	additionalIssueTypesAndAssignees_.append("I think it's an SGM specific issue", "davidChevrier");

	sgmSidebar_ = new SGMSidebar();
	mw_->addRightWidget(sgmSidebar_);

//	SGMPeriodicTableView *sgmPeriodicTableView = new SGMPeriodicTableView(SGMPeriodicTable::sgmTable());
//	sgmPeriodicTableView->show();
	SGMFastScanParametersModificationWizard *fastScanWizard = new SGMFastScanParametersModificationWizard();
	fastScanWizard->show();

	return true;
}
