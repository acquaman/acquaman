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
#include "ui/AMDatamanAppBottomPanel.h"
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
#include <dataman/info/AMOldDetectorInfoSet.h>
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
#include "dataman/AM2DScan.h"
#include "dataman/AM3DScan.h"
#include "analysis/AM2DNormalizationAB.h"
#include "analysis/AM1DNormalizationAB.h"
#include "analysis/AM2DAdditionAB.h"
#include "analysis/AM3DAdditionAB.h"
#include "analysis/AM3DBinningAB.h"
#include "analysis/AM2DDeadTimeAB.h"
#include "analysis/AM3DDeadTimeAB.h"
#include "analysis/AMOrderReductionAB.h"

#include "dataman/AMDbUpgrade1Pt1.h"
#include "dataman/AMDbUpgrade1Pt2.h"

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

	isBadDatabaseDirectory_ = false;
	finishedSender_ = 0;
	resetFinishedSignal(this, SIGNAL(datamanStartupFinished()));

	// shutdown is called automatically from the destructor if necessary, but Qt recommends that clean-up be handled in the aboutToQuit() signal. MS Windows doesn't always let the main function finish during logouts.
	// HOWEVER, we're not doing this for now, since this change could cause some existing applications to crash on shutdown, because they're not ready for events to be delivered during their shutdown process.
	// connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));

	// Prepend the AM upgrade 1.1 to the list for the user database
	AMDbUpgrade *am1Pt1UserDb = new AMDbUpgrade1Pt1("user", this);
	prependDatabaseUpgrade(am1Pt1UserDb);

	// Append the AM upgrade 1.2 to the list for the user database
	AMDbUpgrade *am1Pt2UserDb = new AMDbUpgrade1Pt2("user", this);
	appendDatabaseUpgrade(am1Pt2UserDb);
}

bool AMDatamanAppController::startup() {

	//AMErrorMon::enableDebugNotifications(true);

	AM::registerTypes();

	if(!startupBeforeAnything())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_BEFORE_ANYTHING, "Problem with Acquaman startup: before any other startup routines.");

	splashScreen_ = new AMDatamanStartupSplashScreen();
	splashScreen_->show();

	AMErrorMon::subscribeToCode(AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, splashScreen_, "onErrorMonStartupCode");
	AMErrorMon::subscribeToCode(AMDATAMANAPPCONTROLLER_STARTUP_FINISHED, splashScreen_, "onErrorMonStartupFinished");
	AMErrorMon::subscribeToCode(AMDATAMANAPPCONTROLLER_STARTUP_SUBTEXT, splashScreen_, "onErrorMonDebug");
	AMErrorMon::subscribeToCode(AMDATAMANAPPCONTROLLER_STARTUP_MODECHANGE, splashScreen_, "onErrorMonChangeMode");

	if(!startupLoadSettings())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_LOADING_SETTING, "Problem with Acquaman startup: loading settings.");

	if(!startupLoadPlugins())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_LOADING_PLUGINS, "Problem with Acquaman startup: loading plugins.");

	if((isFirstTimeRun_ = startupIsFirstTime())) {
		if(!startupOnFirstTime())
			return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_LOADING_SETTING, "Problem with Acquaman startup: handling first-time user.");
	}
	else {
		if(!startupOnEveryTime())
			return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_HANDING_NON_FIRST_TIME_USER, "Problem with Acquaman startup: handling non-first-time user.");
	}

	if(!startupRegisterDatabases())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REGISTERING_DATABASES, "Problem with Acquaman startup: registering databases.");

	// Now that we have a database: populate initial settings, or just load user settings
	if(isFirstTimeRun_) {
		if(!startupPopulateNewDatabase())
			return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_POPULATING_NEW_USER_DATABASE, "Problem with Acquaman startup: populating new user database.");
	}
	else {
		if(!startupLoadFromExistingDatabase())
			return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REVIEWING_EXISTING_USER_DATABASE, "Problem with Acquaman startup: reviewing existing database.");
	}

	if(!startupRegisterExporters())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REGISTERING_EXPORTERS, "Problem with Acquaman startup: registering exporters.");

	if(!startupBeforeUserInterface())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_BEFORE_USER_INTERFACE, "Problem with Acquaman startup: prior to setting up the user interface.");

	if(!startupCreateUserInterface())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_SETTING_UP_USER_INTERFACE, "Problem with Acquaman startup: setting up the user interface.");

	if(!startupAfterUserInterface())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_AFTER_USER_INTERFACE, "Problem with Acquaman startup: after setting up the user interface.");

	if(!startupInstallActions())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_INSTALLING_MENU_ACTIONS, "Problem with Acquaman startup: installing menu actions.");

	if(!startupAfterEverything())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_AFTER_EVERYTHING, "Problem with Acquaman startup: after all other startup routines.");

	emit datamanStartupFinished();

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
	if(AMUserSettings::userSettingsStartupError)
		AMErrorMon::error(this, AMDATAMANAPPCONTROLLER_USER_SETTINGS_STARTUP_ERROR, "Acquaman encountered a problem reading the user settings file, maybe the permissions are set incorrectly.");

	QString userDatabaseFolder = AMUserSettings::userDataFolder;
	if(userDatabaseFolder.endsWith('/'))
		userDatabaseFolder.remove(userDatabaseFolder.count()-1, 1);
	if(userDatabaseFolder == QDir::homePath())
		isBadDatabaseDirectory_ = true;

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
	if(AMUserSettings::userSettingsStartupError){
		QMessageBox msgBox;
		msgBox.setText("Sorry! A serious error was detected on startup.");
		msgBox.setInformativeText("Acquaman saves some configuration information in a .ini file. Yours cannot be read. Please inform the beamline staff of this error.");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		if(msgBox.exec() == QDialog::Accepted)
			return false;
	}
	else{
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

		// check for and run any database upgrades we require...
		if(!startupDatabaseUpgrades())
			return false;

		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: First-Time Successful");
		qApp->processEvents();
	}
	return true;
}

bool AMDatamanAppController::startupOnEveryTime()
{
	if(!startupCreateDatabases())
		return false;

	// check for and run any database upgrades we require...
	if(!startupDatabaseUpgrades())
		return false;

	qApp->processEvents();

	return true;
}

bool AMDatamanAppController::startupCreateDatabases()
{
	// create the "user" database connection.
	AMDatabase* db = AMDatabase::createDatabase("user", AMUserSettings::userDataFolder + "/" + AMUserSettings::userDatabaseFilename);
	if(!db)
		return false;

	return true;
}

bool AMDatamanAppController::startupDatabaseUpgrades()
{
	QList<AMDbUpgrade *> firstTimeDbUpgrades;
	QList<AMDbUpgrade *> everyTimeDbUpgrades;

	for (int i = 0, size = databaseUpgradeCount(); i < size; i++){

		if (AMDatabase::database(databaseUpgradeAt(i)->databaseNameToUpgrade())->isEmpty())
			firstTimeDbUpgrades.append(databaseUpgradeAt(i));

		else
			everyTimeDbUpgrades.append(databaseUpgradeAt(i));
	}

	if (!onFirstTimeDatabaseUpgrade(firstTimeDbUpgrades)){

		AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRST_TIME_UPGRADES_FAILED, "At least one of the upgrades that were applied to new databases failed.");
		return false;
	}

	if (!onEveryTimeDatabaseUpgrade(everyTimeDbUpgrades)){

		AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_EVERY_TIME_UPGRADES_FAILED, "At least one of the upgrades that were applied to existing databases failed.");
		return false;
	}

	return true;
}

bool AMDatamanAppController::onFirstTimeDatabaseUpgrade(QList<AMDbUpgrade *> upgrades)
{
	// Loop over the database upgrades and make sure the upgrade table reflects the current starting state
	bool success = true;
	AMDbUpgrade *upgrade;

	for(int x = 0; x < upgrades.size(); x++){
		upgrade = upgrades.at(x);
		if(!upgrade->loadDatabaseFromName()){
			AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_LOAD_FAILURE, "Failure to load requested databases for initialization of upgrade table");
			return false;
		}
		// For the databases we are responsible for (not the beamline database) this needs to return false on the first time through ... otherwise things are going really bad
		if(upgrade->isResponsibleForUpgrade() && !upgrade->upgradeRequired()){
			AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_REQUIREMENT_FAILURE, "Failure in initialization of upgrade table, there must be tracking required for new databases that the user application is responsible for");
			return false;
		}

		// For the beamline database (and others like it) the upgrade better be done already
		if(!upgrade->isResponsibleForUpgrade() && upgrade->upgradeRequired()){
			AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_SHARED_DB_FAILURE, "Failure in initialization of upgrade table, an upgrade is not done on a (shared) database the user application is not responsible for");
			return false;
		}
		// Only upgrade things we're responsible for
		else if(upgrade->isResponsibleForUpgrade()){
			success &= upgrade->updateUpgradeTable(false, true);
			if(!success){
				AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_UPGRADE_TABLE_FAILURE, "Failure to write initialization for upgrade table");
				return false;
			}
		}
	}

	return true;
}

bool AMDatamanAppController::onEveryTimeDatabaseUpgrade(QList<AMDbUpgrade *> upgrades)
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
	for(int x = 0; x < upgrades.size(); x++){
		upgradeIsNecessary = false;
		databaseIsEmpty = false;
		upgrade = upgrades.at(x);
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
		if(success && upgrade->isResponsibleForUpgrade() && upgrade->upgradeRequired()){
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
		if(upgradeIsNecessary)
			upgradesCompleted++;
	}

	// If we completed some upgrades (or not) let the user know
	if(upgradesCompleted > 0)
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("Acquaman Startup: Database Upgrade Stage Successful, applied %1 upgrades").arg(upgradesCompleted));
	else
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("Acquaman Startup: Database Upgrade Stage Successful, no upgrades necessary"));

	return true;
}

bool AMDatamanAppController::startupRegisterDatabases()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Registering Databases");
	qApp->processEvents();
	AMDatabase* db = AMDatabase::database("user");
	if(!db) {
		return false;
	}

	bool success = true;

	success &= AMDbObjectSupport::s()->registerDatabase(db);


	success &= AMDbObjectSupport::s()->registerClass<AMDbObject>();
	success &= AMDbObjectSupport::s()->registerClass<AMScan>();
	success &= AMDbObjectSupport::s()->registerClass<AMXASScan>();
	success &= AMDbObjectSupport::s()->registerClass<AMFastScan>();
	success &= AMDbObjectSupport::s()->registerClass<AMXESScan>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DScan>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DScan>();

	success &= AMDbObjectSupport::s()->registerClass<AMRun>();
	success &= AMDbObjectSupport::s()->registerClass<AMExperiment>();
	success &= AMDbObjectSupport::s()->registerClass<AMSample>();
	success &= AMDbObjectSupport::s()->registerClass<AMFacility>();

	success &= AMDbObjectSupport::s()->registerClass<AMRawDataSource>();
	success &= AMDbObjectSupport::s()->registerClass<AMAnalysisBlock>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DExpressionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DSummingAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DDerivativeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMExternalScanDataSourceAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DSummingAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMDeadTimeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DNormalizationAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DNormalizationAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DAdditionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DAdditionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DBinningAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DDeadTimeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DDeadTimeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMOrderReductionAB>();

	success &= AMDbObjectSupport::s()->registerClass<AMOldDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<AMSpectralOutputDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<AMDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<AMControlInfo>();

	success &= AMDbObjectSupport::s()->registerClass<AMControlInfoList>();
	success &= AMDbObjectSupport::s()->registerClass<AMOldDetectorInfoSet>();
	success &= AMDbObjectSupport::s()->registerClass<AMDetectorInfoSet>();
	success &= AMDbObjectSupport::s()->registerClass<AMSamplePosition>();
	success &= AMDbObjectSupport::s()->registerClass<AMSamplePlate>();
	success &= AMDbObjectSupport::s()->registerClass<AMROIInfo>();
	success &= AMDbObjectSupport::s()->registerClass<AMROIInfoList>();

	success &= AMDbObjectSupport::s()->registerClass<AMExporterOptionGeneralAscii>();

	success &= AMDbObjectSupport::s()->registerClass<AMUser>();

	success &= AMDbObjectGeneralViewSupport::registerClass<AMDbObject, AMDbObjectGeneralView>();
	success &= AMDbObjectGeneralViewSupport::registerClass<AM2DScanConfiguration, AM2DScanConfigurationGeneralView>();

	return success;
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

	addBottomPanel();
	mw_->addBottomWidget(bottomPanel_);

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
	connect(dataView_, SIGNAL(fixCDF(QUrl)), this, SLOT(fixCDF(QUrl)));

	// When 'alias' links are clicked in the main window sidebar, we might need to notify some widgets of the details
	connect(mw_, SIGNAL(aliasItemActivated(QWidget*,QString,QVariant)), this, SLOT(onMainWindowAliasItemActivated(QWidget*,QString,QVariant)));
	/////////////////////////

	// Make connections:
	//////////////////////////////

	connect(mw_, SIGNAL(currentPaneChanged(QWidget*)), this, SLOT(onCurrentPaneChanged(QWidget*)));

	// show main window
	mw_->show();

	return true;
}

void AMDatamanAppController::addBottomPanel()
{
	AMDatamanAppBottomPanel *panel = new AMDatamanAppBottomPanel;
	connect(panel, SIGNAL(addExperimentButtonClicked()), this, SLOT(onAddButtonClicked()));
	bottomPanel_ = panel;
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

	exportGraphicsAction_ = new QAction("Export Plot...", mw_);
	exportGraphicsAction_->setStatusTip("Export the current plot to a PDF file.");
	connect(exportGraphicsAction_, SIGNAL(triggered()), this, SLOT(onActionExportGraphics()));

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
	fileMenu_->addSeparator();
	fileMenu_->addAction(exportGraphicsAction_);
	fileMenu_->addSeparator();
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

#include "dataman/AMScanEditorModelItem.h"

void AMDatamanAppController::onCurrentPaneChanged(QWidget *pane)
{
	for (int i = 0, size = scanEditorScanMapping_.size(); i < size; i++)
		if (pane == scanEditorScanMapping_.at(i).second)
			((AMScanEditorModelItem *)(mw_->windowPaneModel()->itemFromIndex(mw_->windowPaneModel()->indexForPane(pane))))->editorWasClicked();

	// This is okay because both AMScanView and AM2DScanView have export capabilities.
	exportGraphicsAction_->setEnabled(qobject_cast<AMGenericScanEditor *>(pane) != 0);
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

//#include "dataman/AMScanEditorModelItem.h"
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
	// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);

	AMScan* scan = AMScan::createFromDatabaseUrl(url, true);

	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);

	if(!scan) {
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

void AMDatamanAppController::fixCDF(const QUrl &url)
{
	Q_UNUSED(url)
	QMessageBox::information(0, "Unable to fix.", "This particular app controller can not fix CDF files.");
}

AMScan *AMDatamanAppController::scanFromEditor(AMGenericScanEditor *editor) const
{
	for (int i = 0, size = scanEditorScanMapping_.size(); i < size; i++)
		if (editor == scanEditorScanMapping_.at(i).second)
			return scanEditorScanMapping_.at(i).first;

	return 0;
}

AMGenericScanEditor *AMDatamanAppController::editorFromScan(AMScan *scan) const
{
	for (int i = 0, size = scanEditorScanMapping_.size(); i < size; i++)
		if (scan == scanEditorScanMapping_.at(i).first)
			return scanEditorScanMapping_.at(i).second;

	return 0;
}

#include "dataman/AMRunExperimentItems.h"

void AMDatamanAppController::onWindowPaneCloseButtonClicked(const QModelIndex& index) {

	// is this a scan editor to be deleted?
	/////////////////////////////////
	if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == scanEditorsParentItem_) {

		AMGenericScanEditor* editor = qobject_cast<AMGenericScanEditor*>(index.data(AM::WidgetRole).value<QWidget*>());

		if (editor){

			int index = -1;

			for (int i = 0, size = scanEditorScanMapping_.size(); i < size; i++)
				if (editor == scanEditorScanMapping_.at(i).second)
					index = i;

			// This must be valid if it isn't -1, no need to check against the size.
			if (index >= 0)
				scanEditorScanMapping_.removeAt(index);
		}

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

	// Add in any additional databases to look in for exporter options
	QStringList optionsDatabases = wizard->optionsDatabases();
	optionsDatabases.append(additionalExporterOptionsDatabases_);
	wizard->setOptionsDatabases(optionsDatabases);

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

AMGenericScanEditor *AMDatamanAppController::createNewScanEditor(bool use2DScanView)
{
	AMGenericScanEditor* editor = new AMGenericScanEditor(use2DScanView);
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(editor, this));
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

	AMScan *scan = 0;
	AMGenericScanEditor *temp2D = 0;

	// Just make a new scan editor as you need it, in the way you need it.
	if (openInIndividualEditors){

		// loop through all URLs
		foreach(QUrl url, urls) {

			scan = dropScanURL(url);

			if(scan){

				bool use2DScanView = (scan->scanRank() == 2 || scan->scanRank() == 3);
				editor = createNewScanEditor(use2DScanView);
				editor->addScan(scan);
				accepted = true;
			}
		}
	}

	// If the editor wasn't passed in, make one when you need it.  Either way, always make a new one if the editor needs to use the 2D scan view.
	else {

		// loop through all URLs
		foreach(QUrl url, urls) {

			scan = dropScanURL(url);

			if(scan){

				if (scan->scanRank() == 2 || scan->scanRank() == 3){

					temp2D = createNewScanEditor(true);
					temp2D->addScan(scan);
				}

				else{

					if (!editor)
						editor = createNewScanEditor();

					editor->addScan(scan);
				}

				accepted = true;
			}
		}
	}

	if (editor)
		mw_->setCurrentPane(editor);

	else if (temp2D)
		mw_->setCurrentPane(temp2D);

	return accepted;
}

#include "actions3/actions/AMNumberChangeAction.h"
#include "actions3/actions/AMNumberChangeActionInfo.h"
#include "actions3/AMListAction3.h"
#include <QListView>
#include <QStringListModel>
AMScan *AMDatamanAppController::dropScanURL(const QUrl &url)
{
	// scheme correct?
	if(url.scheme() != "amd") {
		return 0;
	}

	// Can we connect to the database?
	AMDatabase* db = AMDatabase::database(url.host());
	if(!db)
		return 0;
	// \bug This does not verify that the incoming scans came from the user database. In fact, it happily accepts scans from other databases. Check if we assume anywhere inside AMGenericScanEditor that we're using the AMDatabase::database("user") database. (If we do, this could cause problems when multiple databases exist.)

	QStringList path = url.path().split('/', QString::SkipEmptyParts);
	if(path.count() != 2)
		return 0;

	QString tableName = path.at(0);
	bool idOkay;
	int id = path.at(1).toInt(&idOkay);
	if(!idOkay || id < 1)
		return 0;

	// Only open scans for now (ie: things in the scans table)
	if(tableName != AMDbObjectSupport::s()->tableNameForClass<AMScan>())
		return 0;

	// Flag used to determine if the scan needs to overwrite the currently scanning status.
	bool overwriteNecessary = false;

	// Check if this scan is scanning... Use the currentlyScanning column stored in the database.
	QVariant isScanning = db->retrieve(id, tableName, "currentlyScanning");
	if(!isScanning.isValid())
		return 0;
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

			return 0;
		}
		// If this option is chosen, it will set currently scanning to false, and then allow regular opening of the scan.
		else if (stillScanningEnquiry.clickedButton() == overwriteCurrentlyScanningButton){

			overwriteNecessary = true;
		}
		else
			return 0;
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
			return 0;
		}
		else {
			// user chose to cancel.
			return 0;
		}
	}

	// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
	AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
	if(!dbo)
		return 0;

	// Is it a scan?
	AMScan* scan = qobject_cast<AMScan*>( dbo );
	if(!scan) {
		delete dbo;
		return 0;
	}

	// Change the scan in the database if necessary and then reload it.
	if (overwriteNecessary){

		scan->setScanController(0);
		scan->storeToDb(AMDatabase::database("user"));

		/// \todo DH: I'm sure I should just make a function to do things like this, but for now I'm just duplicating code because it's easy.
		delete dbo;

		dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
		if(!dbo)
			return 0;

		// Is it a scan?
		scan = qobject_cast<AMScan*>( dbo );
		if(!scan) {
			delete dbo;
			return 0;
		}
	}

	return scan;
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

bool AMDatamanAppController::startupAfterUserInterface()
{
	splashScreen_->activateWindow();
	splashScreen_->raise();
	return true;
}

bool AMDatamanAppController::startupAfterEverything()
{
	splashScreen_->activateWindow();
	splashScreen_->raise();
	return true;
}

void AMDatamanAppController::onActionExportGraphics()
{
	AMGenericScanEditor* scanEditor = qobject_cast<AMGenericScanEditor*>(mw_->currentPane());

	if(scanEditor) {
		scanEditor->exportGraphicsToFile();
	}
	else {
		// This can't happen with the current code.  Only accessible from the QAction from the file drop down menu.  Takes into account whether the current pane is a scan editor already.
		AMErrorMon::alert(this, -1111, "To export graphics, you must have a plot open in a scan editor.");
	}
}


void AMDatamanAppController::getUserDataFolderFromDialog(bool presentAsParentFolder)
{
	// Get the current userData folder.
	AMUserSettings::load();
	QString initialFolder = AMUserSettings::userDataFolder;
	initialFolder = QDir::fromNativeSeparators(initialFolder);

	// If the user is supposed to choose the parent folder instead of the actual user data folder:
	if(presentAsParentFolder) {
		while(initialFolder.endsWith("/"))
			initialFolder.chop(1);

		if(initialFolder.endsWith("/userData"))
			initialFolder.chop(9);
	}

	QString newFolder = QFileDialog::getExistingDirectory(0, "Choose the folder for your Acquaman data...", initialFolder, QFileDialog::ShowDirsOnly);

	if(newFolder.isEmpty())
		return;	// user cancelled; do nothing.

	newFolder = QDir::fromNativeSeparators(newFolder);
	newFolder.append("/");

	// If the user is supposed to choose the parent folder instead of the actual user data folder:
	if(presentAsParentFolder) {
		if (!newFolder.endsWith("/userData")){
			QDir makeNewDir(newFolder);
			makeNewDir.mkdir("userData");
			makeNewDir.cd("userData");
			newFolder = makeNewDir.absolutePath() + "/";
		}
	}

	if (newFolder != AMUserSettings::userDataFolder){
		AMUserSettings::userDataFolder = newFolder;
		AMUserSettings::save();
	}
}
