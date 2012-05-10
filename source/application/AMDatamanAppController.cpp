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


#include "AMDatamanAppController.h"

#include "util/AMSettings.h"
#include "acquaman.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbUpgrade.h"
#include "dataman/AMImportController.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"

#include "ui/AMMainWindow.h"
#include "ui/AMWorkflowManagerView.h"
#include "ui/AMBottomBar.h"
#include "ui/dataman/AMDataViewWithActionButtons.h"
#include "ui/dataman/AMRunExperimentInsert.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/util/AMSettingsView.h"
#include "ui/util/AMGithubIssueSubmissionView.h"
#include "ui/AMDatamanStartupSplashScreen.h"

#include "application/AMPluginsManager.h"

#include "util/AMErrorMonitor.h"

#include "ui/dataman/AMFirstTimeWidget.h"

#include <QMenuBar>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QStringBuilder>

#include "util/AMSettings.h"
#include "dataman/AMScan.h"
#include "acquaman/AMScanConfiguration.h"

// Necessary for registering database types:
////////////////////////////
#include <dataman/AMXASScan.h>
#include <dataman/AMFastScan.h>
#include <dataman/AMRun.h>
#include <dataman/AMSample.h>
#include <dataman/AMExperiment.h>
#include <dataman/info/AMControlInfoList.h>
#include <dataman/info/AMDetectorInfoSet.h>
#include <dataman/AMSamplePlate.h>
#include <dataman/info/AMSpectralOutputDetectorInfo.h>
#include "dataman/AMUser.h"
#include "dataman/AMXESScan.h"
#include "dataman/info/AMROIInfo.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AMExternalScanDataSourceAB.h"
#include "analysis/AM1DSummingAB.h"
#include "analysis/AMDeadTimeAB.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "actions2/AMActionInfo.h"
#include "actions2/AMActionLog.h"
#include "actions2/actions/AMWaitActionInfo.h"
#include "actions2/actions/AMControlMoveActionInfo.h"
#include "actions2/actions/AMScanControllerActionInfo.h"
#include "dataman/AM2DScan.h"
#include "analysis/AM2DNormalizationAB.h"

#include "actions3/AMLoopActionInfo3.h"
#include "actions3/actions/AMNumberChangeAction.h"
#include "actions3/AMActionInfo3.h"
#include "actions3/AMActionLog3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMScanAction.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "ui/dataman/AMDbObjectGeneralView.h"
#include "ui/dataman/AMDbObjectGeneralViewSupport.h"
#include "acquaman/AM2DScanConfiguration.h"
#include "ui/dataman/AM2DScanConfigurationGeneralView.h"


AMDatamanAppController::AMDatamanAppController(QObject *parent) :
	QObject(parent)
{
	isStarting_ = true;
	isShuttingDown_ = false;

	finishedSender_ = 0;
	resetFinishedSignal(this, SIGNAL(datamanStartupFinished()));

	// shutdown is called automatically from the destructor if necessary, but Qt recommends that clean-up be handled in the aboutToQuit() signal. MS Windows doesn't always let the main function finish during logouts.
	// HOWEVER, we're not doing this for now, since this change could cause some existing applications to crash on shutdown, because they're not ready for events to be delivered during their shutdown process.
	// connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));
}

bool AMDatamanAppController::startup() {

	//AMErrorMon::enableDebugNotifications(true);

	AM::registerTypes();

	splashScreen_ = new AMDatamanStartupSplashScreen();
	splashScreen_->show();

	AMErrorMon::subscribeToCode(AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, splashScreen_, "onErrorMonStartupCode");
	AMErrorMon::subscribeToCode(AMDATAMANAPPCONTROLLER_STARTUP_FINISHED, splashScreen_, "onErrorMonStartupFinished");
	AMErrorMon::subscribeToCode(AMDATAMANAPPCONTROLLER_STARTUP_SUBTEXT, splashScreen_, "onErrorMonDebug");
	AMErrorMon::subscribeToCode(AMDATAMANAPPCONTROLLER_STARTUP_MODECHANGE, splashScreen_, "onErrorMonChangeMode");

	if(!startupLoadSettings()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_LOADING_SETTING, "Problem with Acquaman startup: loading settings."); return false; }

	if(!startupLoadPlugins()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_LOADING_PLUGINS, "Problem with Acquaman startup: loading plugins."); return false; }

	if((isFirstTimeRun_ = startupIsFirstTime())) {
		if(!startupOnFirstTime()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_LOADING_SETTING, "Problem with Acquaman startup: handling first-time user."); return false; }
	}
	else {
		if(!startupOnEveryTime()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_HANDING_NON_FIRST_TIME_USER, "Problem with Acquaman startup: handling non-first-time user."); return false; }
	}

	if(!startupRegisterDatabases()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REGISTERING_DATABASES, "Problem with Acquaman startup: registering databases."); return false; }

	// Now that we have a database: populate initial settings, or just load user settings
	if(isFirstTimeRun_) {
		if(!startupPopulateNewDatabase()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_POPULATING_NEW_USER_DATABASE, "Problem with Acquaman startup: populating new user database."); return false; }
	}
	else {
		if(!startupLoadFromExistingDatabase()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REVIEWING_EXISTING_USER_DATABASE, "Problem with Acquaman startup: reviewing existing database."); return false; }
	}

	if(!startupRegisterExporters()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REGISTERING_EXPORTERS, "Problem with Acquaman startup: registering exporters."); return false; }

	if(!startupCreateUserInterface()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_SETTING_UP_USER_INTERFACE, "Problem with Acquaman startup: setting up the user interface."); return false; }
	splashScreen_->raise();
	splashScreen_->activateWindow();

	if(!startupInstallActions()) { AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_INSTALLING_MENU_ACTIONS, "Problem with Acquaman startup: installing menu actions."); return false; }

	emit datamanStartupFinished();
	// show main window
	mw_->show();
	splashScreen_->raise();
	splashScreen_->activateWindow();

	isStarting_ = false;
	return true;
}

bool AMDatamanAppController::startupLoadSettings()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Loading Settings");
	qApp->processEvents();
	// Load settings from disk:
	AMSettings::s()->load();
	AMUserSettings::load();
	return true;
}

bool AMDatamanAppController::startupLoadPlugins()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Loading Plugins");
	qApp->processEvents();
	// Load plugins:
	AMPluginsManager::s()->loadApplicationPlugins();
	return true;
}

bool AMDatamanAppController::startupIsFirstTime()
{
	bool isFirstTime = false;

	// check for missing user settings:
	QSettings s(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");
	if(!s.contains("userDataFolder")) {
		isFirstTime = true;
	}

	else {

		// check for existence of user data folder:
		QDir userDataDir(AMUserSettings::userDataFolder);
		if(!userDataDir.exists()) {
			isFirstTime = true;
		}

		// check for existence of database:
		QString filename = AMUserSettings::userDataFolder + "/" + AMUserSettings::userDatabaseFilename;
		QFile dbFile(filename);
		if(!dbFile.exists()) {
			isFirstTime = true;
		}
	}

	return isFirstTime;
}

bool AMDatamanAppController::startupOnFirstTime()
{
	AMFirstTimeWizard ftw;

	// We're pretty forceful here... The user needs to accept this dialog.
	if(ftw.exec() != QDialog::Accepted)
		return false;

	AMUserSettings::userDataFolder = ftw.field("userDataFolder").toString();
	if(!AMUserSettings::userDataFolder.endsWith('/'))
		AMUserSettings::userDataFolder.append("/");

	AMUserSettings::save();

	// Attempt to create user's data folder, only if it doesn't exist:
	QDir userDataDir(AMUserSettings::userDataFolder);
	if(!userDataDir.exists()) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Information, 0, "Creating new user data folder: "  + AMUserSettings::userDataFolder));
		if(!userDataDir.mkpath(AMUserSettings::userDataFolder)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, 0, "Could not create user data folder " + AMUserSettings::userDataFolder));
			return false;
		}
	}

	// Find out the user's name:
	AMUser::user()->setName( ftw.field("userName").toString() );

	if(!startupCreateDatabases())
		return false;

	// Loop over the database upgrades and make sure the upgrade table reflects the current starting state
	bool success = true;
	AMDbUpgrade *upgrade;
	for(int x = 0; x < databaseUpgrades_.count(); x++){
		upgrade = databaseUpgrades_.at(x);
		if(!upgrade->loadDatabaseFromName()){
			AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_LOAD_FAILURE, "Failure to load requested databases for initialization of upgrade table");
			return false;
		}
		// This needs to return false on the first time through ... otherwise things are going really bad
		if(!upgrade->upgradeRequired()){
			AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_REQUIREMENT_FAILURE, "Failure in initialization of upgrade table, there must be tracking required for new databases");
			return false;
		}
		success &= upgrade->updateUpgradeTable(false, true);
		if(!success){
			AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_UPGRADE_TABLE_FAILURE, "Failure to write initialization for upgrade table");
			return false;
		}
	}

	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: First-Time Successful");
	qApp->processEvents();
	return true;
}

bool AMDatamanAppController::startupOnEveryTime()
{
	if(!startupCreateDatabases())
		return false;

	// check for and run any database upgrades we require...
	if(!startupDatabaseUpgrades()) return false;
	qApp->processEvents();

	return true;
}

bool AMDatamanAppController::startupCreateDatabases()
{
	// create the "user" database connection.
	AMDatabase* db = AMDatabase::createDatabase("user", AMUserSettings::userDataFolder + "/" + AMUserSettings::userDatabaseFilename);
	if(!db)
		return false;

	// Create the Actions database
	AMDatabase *dbActions = AMDatabase::createDatabase("actions", AMUserSettings::userDataFolder%"/actionsData.db" );
	if(!dbActions)
		return false;

	return true;
}

bool AMDatamanAppController::startupDatabaseUpgrades()
{
	bool success = true;
	bool atLeastOneDatabaseAccessible = false;
	bool databaseIsEmpty;
	bool upgradeIsNecessary;
	int upgradesCompleted = 0;
	AMDbUpgrade *upgrade;
	QString backupsSubFolder = QDateTime::currentDateTime().toString("MMMddyyy_hhmmss");
	QDir databaseBackupDir;
	QString lastErrorString;
	int lastErrorCode;

	// Loop over the database upgrades and apply them if necessary
	for(int x = 0; x < databaseUpgrades_.count(); x++){
		upgradeIsNecessary = false;
		databaseIsEmpty = false;
		upgrade = databaseUpgrades_.at(x);
		QString pathToDatabase;
		QString databaseName;

		// Load up the database in question. Figure out the path information if that was successful.
		if(!upgrade->loadDatabaseFromName()){
			lastErrorString = QString("Failure to load requested databases for upgrade %1").arg(upgrade->upgradeToTag());
			lastErrorCode = AMDATAMANAPPCONTROLLER_DB_UPGRADE_LOAD_FAILURE;
			success = false;
		}
		else{
			pathToDatabase = upgrade->databaseToUpgrade()->dbAccessString().replace("//", "/").section("/", 0, -2);
			databaseName = upgrade->databaseToUpgrade()->dbAccessString().replace("//", "/").section("/", -1);
			atLeastOneDatabaseAccessible = true;
		}

		// Make a note if the database is empty
		if(success && upgrade->databaseToUpgrade()->isEmpty())
			databaseIsEmpty = true;

		// Only apply upgrades that are required
		if(success && upgrade->upgradeRequired()){
			AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("Acquaman Startup: Starting Database Advanced Upgrade %1").arg(upgrade->upgradeToTag()));
			qApp->processEvents();

			// Back up the database in question if haven't done so already for this set of upgrades. Also make the folders if necessary.
			databaseBackupDir.setPath(pathToDatabase);
			if(!databaseBackupDir.cd(".BACKUPS")){
				databaseBackupDir.mkdir(".BACKUPS");
				if(!databaseBackupDir.cd(".BACKUPS")){
					lastErrorString = QString("Failure to create or find backup directory for upgrade %1").arg(upgrade->upgradeToTag());
					lastErrorCode = AMDATAMANAPPCONTROLLER_DB_UPGRADE_BACKUPS_DIRECTORY_NOT_FOUND;
					success = false;
				}
			}
			if(success && !databaseBackupDir.cd(backupsSubFolder)){
				databaseBackupDir.mkdir(backupsSubFolder);
				if(!databaseBackupDir.cd(backupsSubFolder)){
					lastErrorString = QString("Failure to create or find backup sub directory for upgrade %1").arg(upgrade->upgradeToTag());
					lastErrorCode = AMDATAMANAPPCONTROLLER_DB_UPGRADE_BACKUPS_SUBDIRECTORY_NOT_FOUND;
					success = false;
				}
			}
			QStringList backupFilter;
			backupFilter << QString("%1.before*").arg(databaseName);
			if(success && databaseBackupDir.entryList(backupFilter).count() == 0){
				if(!QFile::copy(pathToDatabase%"/"%databaseName, databaseBackupDir.absolutePath()%"/"%databaseName%".before"%upgrade->upgradeToTag())){
					lastErrorString = QString("Failure to backup requested databases for upgrade %1").arg(upgrade->upgradeToTag());
					lastErrorCode = AMDATAMANAPPCONTROLLER_DB_UPGRADE_BACKUP_FAILURE;
					success = false;
				}
			}

			// Actually call the upgrade if it's necessary
			if(success && upgrade->upgradeNecessary()){
				upgradeIsNecessary = true;
				if(!upgrade->upgrade()){
					lastErrorString = QString("Upgrade run failed for upgrade %1").arg(upgrade->upgradeToTag());
					lastErrorCode = AMDATAMANAPPCONTROLLER_DB_UPGRADE_UPGRADE_FAILURE;
					success = false;
				}
			}

			// Modify the upgrade table to reflect the changes ... marking as necessary or not. We can also label those upgrades that were done when the database is newly created using the empty flag.
			if(success && !upgrade->updateUpgradeTable(upgradeIsNecessary, databaseIsEmpty)){
					lastErrorString = QString("Failure to apply upgrade %1 or to update the upgrade table").arg(upgrade->upgradeToTag());
					lastErrorCode = AMDATAMANAPPCONTROLLER_DB_UPGRADE_UPGRADE_TABLE_FAILURE;
					success = false;
			}
		}
		if(!success && atLeastOneDatabaseAccessible){
			// Restore from backups in case of failure. We must restore all of the databases that we tried to upgrade. Only do this if we managed to load up any of the databases.
			QStringList backupDatabaseFilter;
			backupDatabaseFilter << "*.before*";
			QStringList backupDatabases = databaseBackupDir.entryList(backupDatabaseFilter);
			for(int y = 0; y < backupDatabases.count(); y++){
				QString databaseBaseName = backupDatabases.at(y);
				databaseBaseName.remove(QRegExp("\\.before.*"));
				QFile::remove(pathToDatabase%"/"%databaseBaseName);
				QFile::copy(databaseBackupDir.absolutePath()%"/"%backupDatabases.at(y), pathToDatabase%"/"%databaseBaseName);
			}
			AMErrorMon::alert(0, lastErrorCode, lastErrorString);
			return false;
		}
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("Acquaman Startup: Finalizing Database Advanced Upgrade %1").arg(upgrade->upgradeToTag()));
		qApp->processEvents();
		upgradesCompleted++;
	}

	// If we completed some upgrades (or not) let the user know
	if(upgradesCompleted > 0)
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("Acquaman Startup: Database Upgrade Stage Successful, applied %1 upgrades").arg(upgradesCompleted));
	else
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("Acquaman Startup: Database Upgrade Stage Successful, no upgrades necessary"));
	return true;
}

#include "util/SGM/SGMSettings.h"
bool AMDatamanAppController::startupRegisterDatabases()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Registering Databases");
	qApp->processEvents();
	AMDatabase* db = AMDatabase::database("user");
	if(!db) {
		return false;
	}

	AMDbObjectSupport::s()->registerDatabase(db);

	// Grab the Action database
	AMDatabase *dbActions = AMDatabase::database("actions");
	if(!dbActions)
		return false;

	// Register the Actions database
	if(!AMDbObjectSupport::s()->registerDatabase(dbActions)) {
		//AMErrorMon::alert(this, -702, "Error registering the SGM Database. Please report this problem to the Acquaman developers.");
		return false;
	}

	AMDbObjectSupport::s()->registerClass<AMDbObject>();
	AMDbObjectSupport::s()->registerClass<AMScan>();
	AMDbObjectSupport::s()->registerClass<AMXASScan>();
	AMDbObjectSupport::s()->registerClass<AMFastScan>();
	AMDbObjectSupport::s()->registerClass<AMXESScan>();
	AMDbObjectSupport::s()->registerClass<AM2DScan>();

	AMDbObjectSupport::s()->registerClass<AMRun>();
	AMDbObjectSupport::s()->registerClass<AMExperiment>();
	AMDbObjectSupport::s()->registerClass<AMSample>();
	AMDbObjectSupport::s()->registerClass<AMFacility>();

	AMDbObjectSupport::s()->registerClass<AMRawDataSource>();
	AMDbObjectSupport::s()->registerClass<AMAnalysisBlock>();
	AMDbObjectSupport::s()->registerClass<AM1DExpressionAB>();
	AMDbObjectSupport::s()->registerClass<AM2DSummingAB>();
	AMDbObjectSupport::s()->registerClass<AM1DDerivativeAB>();
	AMDbObjectSupport::s()->registerClass<AMExternalScanDataSourceAB>();
	AMDbObjectSupport::s()->registerClass<AM1DSummingAB>();
	AMDbObjectSupport::s()->registerClass<AMDeadTimeAB>();
	AMDbObjectSupport::s()->registerClass<AM2DNormalizationAB>();

	AMDbObjectSupport::s()->registerClass<AMDetectorInfo>();
	AMDbObjectSupport::s()->registerClass<AMSpectralOutputDetectorInfo>();
	AMDbObjectSupport::s()->registerClass<AMControlInfo>();

	AMDbObjectSupport::s()->registerClass<AMControlInfoList>();
	AMDbObjectSupport::s()->registerClass<AMDetectorInfoSet>();
	AMDbObjectSupport::s()->registerClass<AMSamplePosition>();
	AMDbObjectSupport::s()->registerClass<AMSamplePlate>();
	AMDbObjectSupport::s()->registerClass<AMROIInfo>();
	AMDbObjectSupport::s()->registerClass<AMROIInfoList>();

	AMDbObjectSupport::s()->registerClass<AMExporterOptionGeneralAscii>();

	AMDbObjectSupport::s()->registerClass<AMUser>();

	AMDbObjectSupport::s()->registerClass<AMActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMActionLog>();
	AMDbObjectSupport::s()->registerClass<AMWaitActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMControlMoveActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMScanControllerActionInfo>();

	AMDbObjectSupport::s()->registerClass<AMActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMListActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMSequentialListActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMParallelListActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMLoopActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMActionLog3>();
	AMDbObjectSupport::s()->registerClass<AMNumberChangeActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMControlMoveActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMScanActionInfo>();

	AMDbObjectGeneralViewSupport::registerClass<AMDbObject, AMDbObjectGeneralView>();
	AMDbObjectGeneralViewSupport::registerClass<AM2DScanConfiguration, AM2DScanConfigurationGeneralView>();

	return true;
}

bool AMDatamanAppController::startupPopulateNewDatabase()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Populating Databases for First-Time");
	qApp->processEvents();
	AMDatabase* db = AMDatabase::database("user");
	if(!db)
		return false;

	// insert the user into the database, since they are new here.
	AMUser::user()->storeToDb(db);

	// Also on first time only: create facilities.
	AMFacility blank("", "[Other Facility]", ":/128x128/contents.png");
	blank.storeToDb(db);
	AMFacility als801("8.0.1", "Advanced Light Source Beamline 8.0.1", ":/alsIcon.png");
	als801.storeToDb(db);
	AMFacility sgm("SGM", "CLS SGM Beamline", ":/clsIcon.png");
	sgm.storeToDb(db);
	AMFacility vespers("VESPERS", "CLS VESPERS Beamline", ":/clsIcon.png");
	vespers.storeToDb(db);
	AMFacility reixs("REIXS", "CLS REIXS Beamline", ":/clsIcon.png");
	reixs.storeToDb(db);

	return true;
}

bool AMDatamanAppController::startupLoadFromExistingDatabase()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Loading Databases");
	qApp->processEvents();
	AMUser::user()->loadFromDb(AMDatabase::database("user"), 1);
	return true;
}

bool AMDatamanAppController::startupRegisterExporters()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Registering Exporters");
	qApp->processEvents();
	// Install exporters
	AMExportController::registerExporter<AMExporterGeneralAscii>();
	AMExportController::registerExporter<AMExporterAthena>();

	return true;
}

bool AMDatamanAppController::startupCreateUserInterface()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Populating User Interface");
	qApp->processEvents();
	settingsMasterView_ = 0;
	issueSubmissionView_ = 0;

	//Create the main tab window:
	mw_ = new AMMainWindow();
	mw_->setWindowTitle("Acquaman");
	connect(mw_, SIGNAL(itemCloseButtonClicked(QModelIndex)), this, SLOT(onWindowPaneCloseButtonClicked(QModelIndex)));
	mw_->installEventFilter(this);

	bottomBar_ = new AMBottomBar();
	mw_->addBottomWidget(bottomBar_);
	connect(bottomBar_, SIGNAL(addButtonClicked()), this, SLOT(onAddButtonClicked()));
	connect(bottomBar_, SIGNAL(pauseScanIssued()), this, SIGNAL(pauseScanIssued()));
	connect(bottomBar_, SIGNAL(stopScanIssued()), this, SIGNAL(stopScanIssued()));

	// Create panes in the main window:
	////////////////////////////////////

	// A heading for the scan editors
	scanEditorsParentItem_ = mw_->windowPaneModel()->headingItem("Open Scans");


	// Make a dataview widget and add it under two links/headings: "Runs" and "Experiments". See AMMainWindowModel for more information.
	////////////////////////////////////
	dataView_ = new AMDataViewWithActionButtons();
	dataView_->setWindowTitle("Data");

	QStandardItem* dataViewItem = new QStandardItem();
	dataViewItem->setData(qVariantFromValue((QWidget*)dataView_), AM::WidgetRole);
	dataViewItem->setFlags(Qt::ItemIsEnabled);	// enabled, but should not be selectable
	QFont font = QFont("Lucida Grande", 10, QFont::Bold);
	font.setCapitalization(QFont::AllUppercase);
	dataViewItem->setFont(font);
	dataViewItem->setData(QBrush(QColor::fromRgb(100, 109, 125)), Qt::ForegroundRole);
	dataViewItem->setData(true, AMWindowPaneModel::DockStateRole);

	mw_->windowPaneModel()->appendRow(dataViewItem);

	runsParentItem_ = new QStandardItem(QIcon(":/22x22/view_calendar_upcoming_days.png"), "Runs");
	mw_->windowPaneModel()->initAliasItem(runsParentItem_, dataViewItem, "Runs", -1);
	dataViewItem->appendRow(runsParentItem_);
	experimentsParentItem_ = new QStandardItem(QIcon(":/applications-science.png"), "Experiments");
	mw_->windowPaneModel()->initAliasItem(experimentsParentItem_, dataViewItem, "Experiments", -1);
	dataViewItem->appendRow(experimentsParentItem_);

	// Hook into the sidebar and add Run and Experiment links below these headings.
	runExperimentInsert_ = new AMRunExperimentInsert(AMDatabase::database("user"), runsParentItem_, experimentsParentItem_, this);
	connect(runExperimentInsert_, SIGNAL(newExperimentAdded(QModelIndex)), this, SLOT(onNewExperimentAdded(QModelIndex)));

	// connect the activated signal from the dataview to our own slot
	connect(dataView_, SIGNAL(selectionActivated(QList<QUrl>)), this, SLOT(onDataViewItemsActivated(QList<QUrl>)));
	connect(dataView_, SIGNAL(selectionActivatedSeparateWindows(QList<QUrl>)), this, SLOT(onDataViewItemsActivatedSeparateWindows(QList<QUrl>)));
	connect(dataView_, SIGNAL(selectionExported(QList<QUrl>)), this, SLOT(onDataViewItemsExported(QList<QUrl>)));
	connect(dataView_, SIGNAL(launchScanConfigurationsFromDb(QList<QUrl>)), this, SLOT(onLaunchScanConfigurationsFromDb(QList<QUrl>)));

	// When 'alias' links are clicked in the main window sidebar, we might need to notify some widgets of the details
	connect(mw_, SIGNAL(aliasItemActivated(QWidget*,QString,QVariant)), this, SLOT(onMainWindowAliasItemActivated(QWidget*,QString,QVariant)));
	/////////////////////////

	// Make connections:
	//////////////////////////////

	connect(mw_, SIGNAL(currentPaneChanged(QWidget*)), this, SLOT(onCurrentPaneChanged(QWidget*)));

	return true;
}

bool AMDatamanAppController::startupInstallActions()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Populating User Menus");
	qApp->processEvents();
	// make/install actions:
	/////////////////////////////////
	QAction* importLegacyFilesAction = new QAction("Import Legacy Files...", mw_);
	importLegacyFilesAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_L));
	importLegacyFilesAction->setStatusTip("Import raw data files (not collected with Acquaman) into the library");
	connect(importLegacyFilesAction, SIGNAL(triggered()), this, SLOT(onActionImportLegacyFiles()));

	QAction* importAcquamanDatabaseAction = new QAction("Import Acquaman Bundle...", mw_);
	importAcquamanDatabaseAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_I));
	importAcquamanDatabaseAction->setStatusTip("Import an Acquaman Bundle from another computer or from a beamline run");
	connect(importAcquamanDatabaseAction, SIGNAL(triggered()), this, SLOT(onActionImportAcquamanDatabase()));


	QAction* amSettingsAction = new QAction("Acquaman Settings", mw_);
	amSettingsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_S));
	amSettingsAction->setStatusTip("View or Change Settings");
	connect(amSettingsAction, SIGNAL(triggered()), this, SLOT(onActionSettings()));

	QAction* amIssueSubmissionAction = new QAction("Report an Issue", mw_);
	amIssueSubmissionAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_B));
	amIssueSubmissionAction->setStatusTip("Report an issue to the Acquaman Developers");
	connect(amIssueSubmissionAction, SIGNAL(triggered()), this, SLOT(onActionIssueSubmission()));

	//install menu bar, and add actions
	//////////////////////////////////////
#ifdef Q_WS_MAC
	menuBar_ = new QMenuBar(0);
#else
	menuBar_ = new QMenuBar();
	mw_->addTopWidget(menuBar_);
#endif

	fileMenu_ = menuBar_->addMenu("File");
	fileMenu_->addAction(importLegacyFilesAction);
	fileMenu_->addAction(importAcquamanDatabaseAction);
	fileMenu_->addAction(amSettingsAction);

	helpMenu_ = menuBar_->addMenu("Help");
	helpMenu_->addAction(amIssueSubmissionAction);

	return true;
}


void AMDatamanAppController::shutdown() {

	isShuttingDown_ = true;

	// destroy the main window. This will delete everything else within it.
	delete mw_;

	// Close down connection to the user Database
	AMDatabase::deleteDatabase("user");

}


void AMDatamanAppController::onActionImportLegacyFiles() {

	new AMImportController();

}

void AMDatamanAppController::onActionSettings()
{
	if(!settingsMasterView_)
		settingsMasterView_ = new AMSettingsMasterView();

	settingsMasterView_->show();
}

void AMDatamanAppController::onActionIssueSubmission()
{
	if(!issueSubmissionView_){

		issueSubmissionView_ = new AMGithubIssueSubmissionView();

		for(int x = 0; x < additionalIssueTypesAndAssignees_.count(); x++)
			issueSubmissionView_->addIssueType(additionalIssueTypesAndAssignees_.at(x), additionalIssueTypesAndAssignees_.keyAt(x));

		connect(issueSubmissionView_, SIGNAL(finished()), this, SLOT(onIssueSubmissionViewFinished()));
	}

	issueSubmissionView_->show();
	issueSubmissionView_->raise();
	issueSubmissionView_->activateWindow();
}

void AMDatamanAppController::onCurrentPaneChanged(QWidget *pane) {
	Q_UNUSED(pane)
}

void AMDatamanAppController::onMainWindowAliasItemActivated(QWidget *target, const QString &key, const QVariant &value) {

	if(target == dataView_) {
		if(key == "Runs")
			dataView_->dataView()->showRun(value.toInt());
		if(key == "Experiments")
			dataView_->dataView()->showExperiment(value.toInt());
	}
}

void AMDatamanAppController::onNewExperimentAdded(const QModelIndex &index) {
	mw_->sidebar()->expand(index.parent()); //Do this to show people where it ended up...
	//mw_->sidebar()->setCurrentIndex(index);
}



#include "dataman/AMExperiment.h"
void AMDatamanAppController::onAddButtonClicked() {

	static QString lastExperimentName;
	// For now, we simply create a new experiment. Later on, this could pop up a menu to create a new experiment, run, sample plate, whatever...
	bool ok;
	QString text = QInputDialog::getText(mw_, "Create new experiment",
										 "Experiments help you organize groups of related scans.\n\nNew experiment name:",
										 QLineEdit::Normal,
										 lastExperimentName, &ok);
	if (ok && !text.isEmpty()) {
		AMExperiment e(text);
		lastExperimentName = text;
		e.storeToDb(AMDatabase::database("user"));
	}
}

void AMDatamanAppController::onProgressUpdated(double elapsed, double total){
	bottomBar_->updateScanProgress(elapsed, total);
}

#include "dataman/AMScanEditorModelItem.h"

void AMDatamanAppController::onDataViewItemsActivated(const QList<QUrl>& itemUrls) {

	dropScanURLs(itemUrls);
}

void AMDatamanAppController::onDataViewItemsActivatedSeparateWindows(const QList<QUrl> &itemUrls)
{
	dropScanURLs(itemUrls, 0, true);
}

void AMDatamanAppController::onLaunchScanConfigurationsFromDb(const QList<QUrl> &urls)
{
	if (urls.isEmpty())
		return;

	for (int i = 0; i < urls.size(); i++)
		launchScanConfigurationFromDb(urls.at(i));
}

#include "ui/acquaman/AMScanConfigurationView.h"

void AMDatamanAppController::launchScanConfigurationFromDb(const QUrl &url)
{
	// scheme correct?
	if (url.scheme() != "amd")
		return;

	// Scan configurations only come from the user databases currently.
	AMDatabase *db = AMDatabase::database("user");
	if (!db)
		return;

	QStringList path = url.path().split('/', QString::SkipEmptyParts);
	if(path.count() != 2)
		return;

	QString tableName = path.at(0);
	bool idOkay;
	int id = path.at(1).toInt(&idOkay);
	if(!idOkay || id < 1)
		return;

	// Only open scans for now (ie: things in the scans table)
	if(tableName != AMDbObjectSupport::s()->tableNameForClass<AMScan>())
		return;

	// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);
	// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
	AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
	if(!dbo)
		return;
	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);

	// Is it a scan?
	AMScan* scan = qobject_cast<AMScan*>( dbo );
	if(!scan) {
		delete dbo;
		return;
	}

	// Does the scan have a configuration?
	AMScanConfiguration* config = scan->scanConfiguration();
	if(!config) {
		scan->release();
		return;
	}
	// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
	config = config->createCopy();
	scan->release();
	if(!config)
		return;

	AMScanConfigurationView *view = config->createView();
	if(!view) {
		delete config;
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -401, "Unable to create view from the scan configuration loaded from the database.  Contact Acquaman developers."));
		return;
	}

	view->setEnabled(false);
	view->setAttribute(Qt::WA_DeleteOnClose, true);
	view->show();
}

#include "dataman/AMRunExperimentItems.h"

void AMDatamanAppController::onWindowPaneCloseButtonClicked(const QModelIndex& index) {

	// is this a scan editor to be deleted?
	/////////////////////////////////
	if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == scanEditorsParentItem_) {
		AMGenericScanEditor* editor = qobject_cast<AMGenericScanEditor*>(index.data(AM::WidgetRole).value<QWidget*>());
		closeScanEditor(editor);
	}

	// is this an experiment asking to be deleted?
	/// \todo bad code; improve this with better architecture and functionality in expItem.  Don't like trusting dynamic_cast; there's no guarantee that someone didn't put a non-AMExperimentModelItem into the model under experimentsParentItem_.
	else if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == experimentsParentItem_) {

		AMExperimentModelItem* expItem = dynamic_cast<AMExperimentModelItem*>(mw_->windowPaneModel()->itemFromIndex(index));
		if(!expItem)
			return;

		AMExperiment experiment(expItem->id(), expItem->database());

		QMessageBox deleteConfirmation(mw_);
		deleteConfirmation.setText(QString("Are you sure you want to delete the experiment '%1'?").arg(experiment.name()));
		deleteConfirmation.setInformativeText("The scans in this experiment will remain, but they will no longer be associated with the experiment. You can't undo this action.");
		deleteConfirmation.setIcon(QMessageBox::Question);
		deleteConfirmation.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		deleteConfirmation.setDefaultButton(QMessageBox::Ok);
		deleteConfirmation.setEscapeButton(QMessageBox::Cancel);

		if(deleteConfirmation.exec() == QMessageBox::Ok) {
			AMExperiment::deleteExperiment(experiment.id(), expItem->database());
		}
	}
}

void AMDatamanAppController::onIssueSubmissionViewFinished(){
	if(!issueSubmissionView_)
		return;

	disconnect(issueSubmissionView_, SIGNAL(finished()), this, SLOT(onIssueSubmissionViewFinished()));
	delete issueSubmissionView_;
	issueSubmissionView_ = 0;
}

void AMDatamanAppController::onStartupFinished(){
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_FINISHED, "Acquaman Startup: Finished");
}

#include "dataman/export/AMExportController.h"
#include "ui/dataman/AMExportWizard.h"

void AMDatamanAppController::onDataViewItemsExported(const QList<QUrl> &itemUrls)
{
	// will delete itself when finished
	AMExportController* exportController = new AMExportController(itemUrls);
	AMExportWizard* wizard = new AMExportWizard(exportController);
	wizard->show();
}

int AMDatamanAppController::scanEditorCount() const
{
	return scanEditorsParentItem_->rowCount();
}

AMGenericScanEditor * AMDatamanAppController::scanEditorAt(int index) const
{
	if(index<0 || index>=scanEditorCount())
		return 0;

	return qobject_cast<AMGenericScanEditor*>(scanEditorsParentItem_->child(index)->data(AM::WidgetRole).value<QWidget*>()); // seriously...
}

bool AMDatamanAppController::closeScanEditor(int index)
{
	return closeScanEditor(scanEditorAt(index));
}

bool AMDatamanAppController::closeScanEditor(AMGenericScanEditor* editor)
{
	if(editor && editor->canCloseEditor()) {
		mw_->deletePane(editor);
		return true;
	}
	else {
		return false;
	}
}

AMGenericScanEditor * AMDatamanAppController::createNewScanEditor()
{
	AMGenericScanEditor* editor = new AMGenericScanEditor();
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(editor, this, ":/applications-science.png"));
	emit scanEditorCreated(editor);
	return editor;
}

AMGenericScanEditor *AMDatamanAppController::createNewScanEditor(bool use2DScanView)
{
	AMGenericScanEditor* editor = new AMGenericScanEditor(use2DScanView);
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(editor, this, ":/applications-science.png"));
	emit scanEditorCreated(editor);
	return editor;
}

bool AMDatamanAppController::canCloseScanEditors() const
{
	//	bool canCloseEditors = true;
	//	for(int i=0, count = scanEditorCount(); i<count; i++) {
	//		AMGenericScanEditor* editor = scanEditorAt(i);
	//		if(editor) canCloseEditors &= editor->canCloseEditor();
	//	}
	//	return canCloseEditors;

	// Do we need to check all, or is it okay to stop as soon as we find one that doesn't allow closing?
	for(int i=0, count = scanEditorCount(); i<count; i++) {
		AMGenericScanEditor* editor = scanEditorAt(i);
		if(editor) {
			mw_->setCurrentPane(editor);
			if(!editor->canCloseEditor())
				return false;
		}
	}
	return true;
}

//void AMDatamanAppController::processEventsFor(int ms)
//{
//	QTime t;
//	t.start();
//	while(t.elapsed() <ms) {
//		qApp->sendPostedEvents();
//		qApp->processEvents();
//	}
//}

bool AMDatamanAppController::eventFilter(QObject* o, QEvent* e)
{
	if(o == mw_ && e->type() == QEvent::Close) {
		if(!canCloseScanEditors()) {
			e->ignore();
			return true;
		}
		// They got away with closing the main window. We should quit the application
		qApp->quit();	//note that this might already be in progress, if an application quit was what triggered this close event.  No harm in asking twice...

	}
	// anything else, allow unfiltered
	return QObject::eventFilter(o,e);
}

bool AMDatamanAppController::resetFinishedSignal(QObject *sender, const char *signal){
	QString normalizedSignalName = QString("%1").arg(signal).remove(0,1);
	if(!sender || sender->metaObject()->indexOfSignal(normalizedSignalName.toAscii()) < 0)
		return false;
	if(finishedSender_)
		disconnect(finishedSender_, 0, this, SLOT(onStartupFinished()));
	finishedSender_ = sender;
	return connect(sender, signal, this, SLOT(onStartupFinished()));
}

AMGenericScanEditor * AMDatamanAppController::isScanOpenForEditing(int id, AMDatabase *db)
{
	for(int i=0, editorCount=scanEditorCount(); i<editorCount; i++) {
		AMGenericScanEditor* editor = scanEditorAt(i);
		if(editor) {
			for(int j=0, scanCount=editor->scanCount(); j<scanCount; j++) {
				AMScan* scan = editor->scanAt(j);
				if(scan->id() == id && scan->database() == db)
					return editor;
			}
		}
	}
	return 0;
}

bool AMDatamanAppController::dropScanURLs(const QList<QUrl> &urls, AMGenericScanEditor *editor, bool openInIndividualEditors)
{
	if(	!urls.count() )
		return false;

	bool accepted = false;
	bool madeNewEditor = false;

	// where are they going?
	if(openInIndividualEditors) {
		editor = 0;	// create new one for each
	}
	else {
		if(!editor) {
			editor = createNewScanEditor();
			madeNewEditor = true;
		}
	}

	// loop through all URLs
	foreach(QUrl url, urls) {
		if(dropScanURL(url, editor))
			accepted = true;
	}

	if(madeNewEditor) {	// if we made a single new editor for these...
		if(accepted) {
			mw_->setCurrentPane(editor);
		}
		else {
			mw_->deletePane(editor);	// no need for this guy; he's empty.
		}
	}

	return accepted;
}

#include "actions3/actions/AMNumberChangeAction.h"
#include "actions3/actions/AMNumberChangeActionInfo.h"
#include "actions3/AMListAction3.h"
#include <QListView>
#include <QStringListModel>
bool AMDatamanAppController::dropScanURL(const QUrl &url, AMGenericScanEditor *editor)
{
	// scheme correct?
	if(url.scheme() != "amd") {
		return false;
	}

	// Can we connect to the database?
	AMDatabase* db = AMDatabase::database(url.host());
	if(!db)
		return false;
	// \bug This does not verify that the incoming scans came from the user database. In fact, it happily accepts scans from other databases. Check if we assume anywhere inside AMGenericScanEditor that we're using the AMDatabase::database("user") database. (If we do, this could cause problems when multiple databases exist.)

	QStringList path = url.path().split('/', QString::SkipEmptyParts);
	if(path.count() != 2)
		return false;

	QString tableName = path.at(0);
	bool idOkay;
	int id = path.at(1).toInt(&idOkay);
	if(!idOkay || id < 1)
		return false;

	// Only open scans for now (ie: things in the scans table)
	if(tableName != AMDbObjectSupport::s()->tableNameForClass<AMScan>())
		return false;

	// Flag used to determine if the scan needs to overwrite the currently scanning status.
	bool overwriteNecessary = false;

	// Check if this scan is scanning... Use the currentlyScanning column stored in the database.
	QVariant isScanning = db->retrieve(id, tableName, "currentlyScanning");
	if(!isScanning.isValid())
		return false;
	if(isScanning.toBool()) {
		QList<QVariant> nameAndNumber = db->retrieve(id, tableName, QStringList() << "name" << "number");
		QMessageBox stillScanningEnquiry;
		stillScanningEnquiry.setWindowTitle("This scan is still acquiring.");
		stillScanningEnquiry.setText(QString("The scan '%1' (#%2) is currently still acquiring data, so you can't open multiple copies of it yet.  <b><i>Note: You may overwrite the currently scanning state of your scan by choosing \"Overwrite Scanning Status\".  This should be done only if a scan is locked.<i></b>")
									 .arg(nameAndNumber.at(0).toString())
									 .arg(nameAndNumber.at(1).toString()));
		stillScanningEnquiry.setIcon(QMessageBox::Question);
		QPushButton* showMeNowButton = stillScanningEnquiry.addButton("Show me the scan", QMessageBox::AcceptRole);
		stillScanningEnquiry.addButton(QMessageBox::Cancel);
		QPushButton *overwriteCurrentlyScanningButton = stillScanningEnquiry.addButton("Overwrite Scanning Status", QMessageBox::ActionRole);
		stillScanningEnquiry.setDefaultButton(showMeNowButton);
		stillScanningEnquiry.exec();
		if(stillScanningEnquiry.clickedButton() == showMeNowButton) {
			AMGenericScanEditor* otherEditor = isScanOpenForEditing(id, db);
			if(otherEditor)
				mw_->setCurrentPane(otherEditor);

			return false;
		}
		// If this option is chosen, it will set currently scanning to false, and then allow regular opening of the scan.
		else if (stillScanningEnquiry.clickedButton() == overwriteCurrentlyScanningButton){

			overwriteNecessary = true;
		}
		else
			return false;
	}

	// Check if this scan is already open
	AMGenericScanEditor* otherEditor = isScanOpenForEditing(id, db);
	if(otherEditor) {
		QList<QVariant> nameAndNumber = db->retrieve(id, tableName, QStringList() << "name" << "number");
		QMessageBox alreadyOpenEnquiry;
		alreadyOpenEnquiry.setWindowTitle("This scan is already open.");
		alreadyOpenEnquiry.setText(QString("A copy of the scan '%1' (#%2) is already open. Are you sure you want to open multiple copies of it?")
								   .arg(nameAndNumber.at(0).toString())
								   .arg(nameAndNumber.at(1).toString()));
		alreadyOpenEnquiry.setInformativeText("If you open multiple copies of a scan, changes that you make in one view will not show up in the other view. Therefore, you should be careful to only save the version you want to keep.");
		alreadyOpenEnquiry.setIcon(QMessageBox::Question);
		QPushButton* showMeNowButton = alreadyOpenEnquiry.addButton("Show me the open scan", QMessageBox::NoRole);
		QPushButton* openAgainButton = alreadyOpenEnquiry.addButton("Open another copy", QMessageBox::YesRole);
		alreadyOpenEnquiry.addButton(QMessageBox::Cancel);
		alreadyOpenEnquiry.setDefaultButton(showMeNowButton);
		alreadyOpenEnquiry.exec();
		if(alreadyOpenEnquiry.clickedButton() == openAgainButton) {
			// do nothing... we'll proceed
		}
		else if(alreadyOpenEnquiry.clickedButton() == showMeNowButton) {
			mw_->setCurrentPane(otherEditor);
			return false;
		}
		else {
			// user chose to cancel.
			return false;
		}
	}

	// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
	AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
	if(!dbo)
		return false;

	// Is it a scan?
	AMScan* scan = qobject_cast<AMScan*>( dbo );
	if(!scan) {
		delete dbo;
		return false;
	}

	// Change the scan in the database if necessary and then reload it.
	if (overwriteNecessary){

		scan->setScanController(0);
		scan->storeToDb(AMDatabase::database("user"), true);

		/// \todo DH: I'm sure I should just make a function to do things like this, but for now I'm just duplicating code because it's easy.
		delete dbo;

		dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
		if(!dbo)
			return false;

		// Is it a scan?
		scan = qobject_cast<AMScan*>( dbo );
		if(!scan) {
			delete dbo;
			return false;
		}
	}

	// success!
	if (scan->scanRank() == 2){

		if (editor)
			closeScanEditor(editor);

		editor = createNewScanEditor(true);
	}

	else if(!editor) {
		editor = createNewScanEditor();
	}

	editor->addScan(scan);

	return true;
}

#include "dataman/import/AMScanDatabaseImportController.h"
#include "ui/dataman/AMScanDatabaseImportWizard.h"

void AMDatamanAppController::onActionImportAcquamanDatabase()
{
	// will delete itself when finished
	AMScanDatabaseImportController* importController = new AMScanDatabaseImportController();
	AMScanDatabaseImportWizard* wizard = new AMScanDatabaseImportWizard(importController);
	wizard->show();
}
