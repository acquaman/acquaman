/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
#include "ui/dataman/AMSampleManagementWidget.h"
#include "ui/acquaman/AMScanConfigurationViewHolder.h"
#include "ui/SGM/SGMXASScanConfigurationView.h"
#include "ui/SGM/SGMFastScanConfigurationView.h"
#include "ui/SGM/SGMSidebar.h"
#include "acquaman/AMScanController.h"
#include "ui/beamline/AMDetectorView.h"
#include "ui/beamline/AMSingleControlDetectorView.h"
#include "ui/beamline/MCPDetectorView.h"
#include "ui/beamline/PGTDetectorView.h"
#include "ui/beamline/OceanOptics65000DetectorView.h"

#include "ui/AMMainWindow.h"
#include "ui/AMWorkflowManagerView.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMRun.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "ui/AMStartScreen.h"

#include "application/AMAppControllerSupport.h"

#include "ui/util/SGM/SGMSettingsMasterView.h"
#include "util/SGM/SGMSettings.h"
#include "util/SGM/SGMDacqConfigurationFile.h"
#include "util/SGM/SGMPluginsLocation.h"
#include "application/AMPluginsManager.h"
#include "util/SGM/SGMPeriodicTable.h"
#include "ui/util/AMGithubIssueSubmissionView.h"

SGMAppController::SGMAppController(QObject *parent) :
	AMAppController(parent)
{
}

bool SGMAppController::startup() {

	// Initialize AMBeamline::bl() as an SGMBeamline::sgm() instance. FIRST!
	//SGMBeamline::sgm();

	SGMSettings::s()->load();

	if(AMAppController::startup()) {
		if(!setupSGMDatabase())
			return false;
		if(!startupSGMInstallActions())
			return false;
		if(!setupSGMPeriodicTable())
			return false;

		SGMBeamline::sgm();

		AMDbObjectSupport::s()->registerClass<MCPDetectorInfo>();
		AMDbObjectSupport::s()->registerClass<PGTDetectorInfo>();
		AMDbObjectSupport::s()->registerClass<OceanOptics65000DetectorInfo>();
		AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration>();
		AMDbObjectSupport::s()->registerClass<SGMFastScanConfiguration>();

		AMDetectorViewSupport::registerClass<AMSingleControlBriefDetectorView, AMSingleControlDetector>();
		AMDetectorViewSupport::registerClass<MCPBriefDetectorView, MCPDetector>();
		AMDetectorViewSupport::registerClass<MCPDetailedDetectorView, MCPDetector>();
		AMDetectorViewSupport::registerClass<PGTBriefDetectorView, PGTDetector>();
		AMDetectorViewSupport::registerClass<PGTDetailedDetectorView, PGTDetector>();
		AMDetectorViewSupport::registerClass<OceanOptics65000BriefDetectorView, OceanOptics65000Detector>();
		AMDetectorViewSupport::registerClass<OceanOptics65000DetailedDetectorView, OceanOptics65000Detector>();

		// Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big ; )
		////////////////////////////////////////

		AMRun existingRun;
		if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
			// no run yet... let's create one.
			AMRun firstRun("SGM", 3);	/// \todo For now, we know that 5 is the ID of the REIXS facility, but this is a hardcoded hack.
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "SGMDefault");

		AMExporterOptionGeneralAscii *sgmDefault;
		// Don't have one called "SGMDefault", so make one. If we have one, retreive it and check it.
		sgmDefault = new AMExporterOptionGeneralAscii();
		if(matchIDs.count() != 0)
			sgmDefault->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));
		sgmDefault->setName("SGMDefault");
		//sgmDefault->setFileName("$name_$exportIndex.txt");
		sgmDefault->setFileName("$name_$fsIndex.txt");
		sgmDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\nGrating: $scanConfiguration[grating%enumConvert]\nHarmonic: $scanConfiguration[harmonic%enumConvert]\nExit Slit Gap: $scanConfiguration[exitSlitGap%double%2] um");
		sgmDefault->setHeaderIncluded(true);
		sgmDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
		sgmDefault->setColumnHeaderIncluded(true);
		sgmDefault->setColumnHeaderDelimiter("==========");
		sgmDefault->setSectionHeader("");
		sgmDefault->setSectionHeaderIncluded(true);
		sgmDefault->setIncludeAllDataSources(false);
		if(sgmDefault->dataSources().count() > 0 && sgmDefault->dataSources().at(0) == "EnergyFeedback")
			sgmDefault->removeDataSourceAt(0);
		sgmDefault->ensureDataSource("I0", false, AMExporterOptionGeneral::CombineInColumnsMode, true);
		sgmDefault->ensureDataSource("EnergyFeedback", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("Photodiode", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
		sgmDefault->ensureDataSource("TEY", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
		sgmDefault->ensureDataSource("TFY", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
		sgmDefault->ensureDataSource("TEYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
		sgmDefault->ensureDataSource("TFYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
		sgmDefault->ensureDataSource("PFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("IPFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("SDD", false, AMExporterOptionGeneral::SeparateFilesMode, false);
		sgmDefault->ensureDataSource("OceanOptics65000", false, AMExporterOptionGeneral::SeparateFilesMode, false);
		sgmDefault->ensureDataSource("PLY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmDefault->ensureDataSource("PLYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		//sgmDefault->setSeparateSectionFileName("$name_$dataSetName_$exportIndex.txt");
		sgmDefault->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.txt");
		sgmDefault->storeToDb(AMDatabase::database("user"));

		matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "SGMDefault");

		if(matchIDs.count() > 0){
			AMAppControllerSupport::registerClass<SGMXASScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(matchIDs.at(0));
			AMAppControllerSupport::registerClass<SGMFastScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(matchIDs.at(0));
		}

		// Show the splash screen, to let the user pick their current run. (It will delete itself when closed)
		AMStartScreen* startScreen = new AMStartScreen(0);
		startScreen->show();



		// Create panes in the main window:
		////////////////////////////////////

		mw_->insertHeading("Beamline Control", 0);
		//////////
		samplePositionView_ = new AMSampleManagementWidget(new SGMSampleManipulatorView(),
								   QUrl("http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg"),
								   "Sample Camera",
								   SGMBeamline::sgm()->currentSamplePlate(),
								   SGMBeamline::sgm()->sampleManipulator());
		mw_->addPane(samplePositionView_, "Beamline Control", "SGM Sample Position", ":/system-software-update.png");
		connect(samplePositionView_, SIGNAL(newSamplePlateSelected(AMSamplePlate*)), SGMBeamline::sgm(), SLOT(setCurrentSamplePlate(AMSamplePlate*)));

		connect(SGMBeamline::sgm(), SIGNAL(currentSamplePlateChanged(AMSamplePlate*)), workflowManagerView_, SLOT(setCurrentSamplePlate(AMSamplePlate*)));

		//sampleTransferView_ = new SGMSampleTransferView();
		//mw_->addPane(sampleTransferView_, "Beamline Control", "SGM Sample Transfer", ":/system-software-update.png");

		sgmScalerView_ = 0;
		connect(SGMBeamline::sgm()->rawScaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onSGMScalerConnected(bool)));

		mw_->insertHeading("Experiment Setup", 1);
		//////////
		xasScanConfigurationView_ = 0; //NULL
		xasScanConfigurationHolder_ = new AMScanConfigurationViewHolder(workflowManagerView_);
		mw_->addPane(xasScanConfigurationHolder_, "Experiment Setup", "SGM XAS Scan", ":/utilities-system-monitor.png");

		fastScanConfigurationView_ = 0; //NULL
		fastScanConfigurationHolder_ = new AMScanConfigurationViewHolder(workflowManagerView_);
		mw_->addPane(fastScanConfigurationHolder_, "Experiment Setup", "SGM Fast Scan", ":/utilities-system-monitor.png");

		connect(xasScanConfigurationHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));
		connect(fastScanConfigurationHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));

		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerCreated()), this, SLOT(onCurrentScanControllerCreated()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerDestroyed()), this, SLOT(onCurrentScanControllerDestroyed()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerStarted()), this, SLOT(onCurrentScanControllerStarted()));

		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineConnected()));

		/*! \todo: hook up bottom-bar signals to the active scan controller.
 void MainWindow::onScanControllerReady(AMScanController *scanController){
  qDebug() << "\n\nScan controller is ready\n\n";
  connect(bottomBar_, SIGNAL(pauseScanIssued()), scanController, SLOT(pause()));
  connect(bottomBar_, SIGNAL(stopScanIssued()), scanController, SLOT(cancel()));
  connect(scanController, SIGNAL(progress(double,double)), bottomBar_, SLOT(updateScanProgress(double,double)));
 }
 */
		additionalIssueTypesAndAssignees_.append("I think it's an SGM specific issue", "davidChevrier");

		sgmSidebar_ = new SGMSidebar();
		mw_->addRightWidget(sgmSidebar_);

		mw_->move(0, 0);

		return true;
	}
	else
		return false;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}


bool SGMAppController::startupRegisterDatabases()
{
	if(!AMAppController::startupRegisterDatabases())
		return false;

	AMDatabase* dbSGM = AMDatabase::createDatabase("SGMBeamline", SGMSettings::s()->SGMDataFolder() + "/" + SGMSettings::s()->SGMDatabaseFilename());
	if(!dbSGM) {
		AMErrorMon::alert(this, -701, "Error creating the SGM Database. Please report this problem to the Acquaman developers.");
		return false;
	}

	if(!AMDbObjectSupport::s()->registerDatabase(dbSGM)) {
		AMErrorMon::alert(this, -702, "Error registering the SGM Database. Please report this problem to the Acquaman developers.");
		return false;
	}
	return true;
}

void SGMAppController::onCurrentPaneChanged(QWidget *pane) {
	Q_UNUSED(pane)
}

void SGMAppController::onSGMBeamlineConnected(){
	if(SGMBeamline::sgm()->isConnected() && !xasScanConfigurationView_ && !fastScanConfigurationView_){
		SGMXASScanConfiguration *sxsc = new SGMXASScanConfiguration(this);
		sxsc->xasRegions()->setEnergyControl(SGMBeamline::sgm()->energy());
		sxsc->regions()->setDefaultTimeControl(SGMBeamline::sgm()->scalerIntegrationTime());
		double goodEnergy = 10 * floor(SGMBeamline::sgm()->energy()->value() / 10);
		sxsc->addRegion(0, goodEnergy, 1, goodEnergy+10, 1);
		xasScanConfigurationView_ = new SGMXASScanConfigurationView(sxsc);
		xasScanConfigurationHolder_->setView(xasScanConfigurationView_);

		SGMFastScanConfiguration *sfsc = new SGMFastScanConfiguration(this);
		fastScanConfigurationView_ = new SGMFastScanConfigurationView(sfsc);
		fastScanConfigurationHolder_->setView(fastScanConfigurationView_);
	}
}

void SGMAppController::onSGMScalerConnected(bool connected){
	if(connected && !sgmScalerView_){
		sgmScalerView_ = new CLSSIS3820ScalerView(SGMBeamline::sgm()->scaler());
		mw_->addPane(sgmScalerView_, "Beamline Control", "SGM Scaler", ":/system-software-update.png");
	}
}

#include "dataman/AMScanEditorModelItem.h"
#include "ui/dataman/AMGenericScanEditor.h"

void SGMAppController::onCurrentScanControllerCreated(){
	connect(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController(), SIGNAL(progress(double,double)), this, SLOT(onProgressUpdated(double,double)));

}

void SGMAppController::onCurrentScanControllerDestroyed(){
}

void SGMAppController::onCurrentScanControllerStarted(){
	openScanInEditorAndTakeOwnership(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
}

void SGMAppController::onActionSGMSettings(){
	if(!sgmSettingsMasterView_)
		sgmSettingsMasterView_ = new SGMSettingsMasterView();
	sgmSettingsMasterView_->show();
}

bool SGMAppController::startupSGMInstallActions(){
	QAction *sgmSettingAction = new QAction("SGM Beamline Settings...", mw_);
	sgmSettingAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_T));
	sgmSettingAction->setStatusTip("View or Change SGM Beamline Settings");
	connect(sgmSettingAction, SIGNAL(triggered()), this, SLOT(onActionSGMSettings()));

	sgmSettingsMasterView_ = 0;

	fileMenu_->addSeparator();
	fileMenu_->addAction(sgmSettingAction);

	return true;
}

bool SGMAppController::setupSGMDatabase(){
	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	QList<int> matchIDs;

	success &= AMDbObjectSupport::s()->registerClass<SGMDacqConfigurationFile>();
	SGMDacqConfigurationFile *configFile;
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "PGTXEOLAmmeter");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("PGTXEOLAmmeter");
		configFile->setConfigurationFileName("pgtxeolAmmeter.cfg");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "PGTXEOLScaler");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("PGTXEOLScaler");
		configFile->setConfigurationFileName("pgtxeolScaler.cfg");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "PGTAmmeter");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("PGTAmmeter");
		configFile->setConfigurationFileName("pgtAmmeter.cfg");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "PGTScaler");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("PGTScaler");
		configFile->setConfigurationFileName("pgtScaler.cfg");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "XEOLAmmeter");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("XEOLAmmeter");
		configFile->setConfigurationFileName("xeolAmmeter.cfg");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "XEOLScaler");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("XEOLScaler");
		configFile->setConfigurationFileName("xeolScaler.cfg");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "DefaultAmmeter");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("DefaultAmmeter");
		configFile->setConfigurationFileName("defaultEnergyAmmeter.cfg");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "DefaultScaler");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("DefaultScaler");
		configFile->setConfigurationFileName("defaultEnergyScaler.cfg");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "FastScaler");
	if(matchIDs.count() == 0){
		configFile = new SGMDacqConfigurationFile();
		configFile->setName("FastScaler");
		configFile->setConfigurationFileName("Scalar_Fast.config");
		configFile->setConfigurationFilePath("/home/sgm/beamline/programming/acquaman/devConfigurationFiles");
		success &= configFile->storeToDb(dbSGM);
	}

	success &= AMDbObjectSupport::s()->registerClass<SGMPluginsLocation>();
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

bool SGMAppController::setupSGMPeriodicTable(){
	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	QList<int> matchIDs;

	success &= AMDbObjectSupport::s()->registerClass<SGMEnergyPosition>();
	success &= AMDbObjectSupport::s()->registerClass<SGMScanInfo>();
	success &= AMDbObjectSupport::s()->registerClass<SGMElementInfo>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanSettings>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanParameters>();

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
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 400.0, -278404, -109034, 1.37942, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 415.0, -268341, -105051, 5.99793, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 430.0, -258981, -101191, 15.5151, 0);
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

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 400);
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
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 700.0, -159088, -42422, 554.784, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 720.0, -154667, -38483, 615.12, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 740.0, -150491, -34593, 676.653, 0);
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

	return success;
}
