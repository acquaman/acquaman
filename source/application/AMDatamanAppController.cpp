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

#include "AMDatamanAppController.h"

#include <QMenuBar>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QStringBuilder>
#include <QFileDialog>
#include <QFrame>

#include "acquaman.h"

#include "acquaman/AMScanConfiguration.h"
#include "acquaman/AMStepScanConfiguration.h"
#include "acquaman/AMTimedRegionScanConfiguration.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/AMGenericContinuousScanConfiguration.h"
#include "acquaman/AMXRFScanConfiguration.h"

#include "application/AMPluginsManager.h"

#include "beamline/camera/AMCameraConfiguration.h"
#include "beamline/camera/AMRotationalOffset.h"
#include "beamline/camera/AMBeamConfiguration.h"
#include "beamline/camera/AMSampleCameraBrowser.h"

#include "dataman/AMImportController.h"
#include "dataman/AMScan.h"
#include "dataman/AMSample.h"
#include "dataman/AMSamplePlate.h"
#include "dataman/AMScanAxis.h"
#include "dataman/AMScanAxisRegion.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMSMAKExporter.h"
#include "dataman/export/AMExporter2DAscii.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

#include "dataman/database/AMDbUpgrade.h"
#include "dataman/databaseUpgrades/AMDbUpgrade1Pt1.h"
#include "dataman/databaseUpgrades/AMDbUpgrade1Pt2.h"
#include "dataman/databaseUpgrades/AMDbUpgrade1Pt3.h"
#include "dataman/databaseUpgrades/AMDbUpgrade1Pt4.h"
#include "dataman/databaseUpgrades/AMDbUpgrade1Pt5.h"
#include "dataman/databaseUpgrades/AMDbUpgrade1Pt6.h"

#include "ui/AMMainWindow.h"
#include "ui/AMDatamanAppBottomPanel.h"
#include "ui/AMDatamanStartupSplashScreen.h"
#include "ui/AMScanEditorsCloseView.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/dataman/AMDbObjectGeneralView.h"
#include "ui/dataman/AMDbObjectGeneralViewSupport.h"
#include "ui/dataman/AMFirstTimeWidget.h"
#include "ui/dataman/AMScanDataView.h"
#include "ui/dataman/AMRunExperimentInsert.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/util/AMSettingsView.h"
#include "ui/util/AMGithubIssueSubmissionView.h"
#include "ui/util/AMAboutDialog.h"
#include "ui/util/AMDirectorySynchronizerDialog.h"
#include "ui/util/AMMessageBoxWTimeout.h"

#include "util/AMErrorMonitor.h"
#include "util/AMSettings.h"

// Necessary for registering database types:
////////////////////////////
#include "dataman/AMXASScan.h"
#include "dataman/AMFastScan.h"
#include "dataman/AMXESScan.h"
#include "dataman/AM2DScan.h"
#include "dataman/AM3DScan.h"
#include "dataman/AMRun.h"
#include "dataman/AMUser.h"
#include "dataman/AMExperiment.h"
#include "dataman/AMSamplePre2013.h"
#include "dataman/AMSamplePlatePre2013.h"
#include "dataman/AMRegionOfInterest.h"
#include "dataman/info/AMControlInfoList.h"
#include "dataman/info/AMOldDetectorInfoSet.h"
#include "dataman/info/AMDetectorInfoSet.h"
#include "dataman/info/AMSpectralOutputDetectorInfo.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterOptionSMAK.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DSummingAB.h"
#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DIntegralAB.h"
#include "analysis/AM2DNormalizationAB.h"
#include "analysis/AM1DNormalizationAB.h"
#include "analysis/AM1DCalibrationAB.h"
#include "analysis/AM2DAdditionAB.h"
#include "analysis/AM3DAdditionAB.h"
#include "analysis/AM3DBinningAB.h"
#include "analysis/AM2DDeadTimeAB.h"
#include "analysis/AM3DDeadTimeAB.h"
#include "analysis/AMOrderReductionAB.h"
#include "analysis/AM1DDeadTimeAB.h"
#include "analysis/AM2DDeadTimeCorrectionAB.h"
#include "analysis/AM3DDeadTimeCorrectionAB.h"
#include "analysis/AMnDDeadTimeAB.h"
#include "analysis/AMRegionOfInterestAB.h"
#include "analysis/AMNormalizationAB.h"
#include "analysis/AM0DAccumulatorAB.h"
#include "analysis/AM1DTimedDataAB.h"
#include "analysis/AM1DKSpaceCalculatorAB.h"
#include "analysis/AM3DNormalizationAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "analysis/AMAdditionAB.h"
#include "analysis/AMnDDeadTimeAB.h"


AMDatamanAppController::AMDatamanAppController(QObject *parent) :
	QObject(parent)
{
	isStarting_ = true;
	isShuttingDown_ = false;
	storageWarningCount_ = 0;

	overrideCloseCheck_ = false;

	defaultUseLocalStorage_ = false;
	firstTimeError_ = false;

	isBadDatabaseDirectory_ = false;
	finishedSender_ = 0;
	resetFinishedSignal(this, SIGNAL(datamanStartupFinished()));

	// Prepend the AM upgrade 1.1 to the list for the user database
	AMDbUpgrade *am1Pt1UserDb = new AMDbUpgrade1Pt1("user", this);
	prependDatabaseUpgrade(am1Pt1UserDb);
	AMDbUpgrade *am1Pt1ActionsDb = new AMDbUpgrade1Pt1("actions", this);
	appendDatabaseUpgrade(am1Pt1ActionsDb);
	AMDbUpgrade *am1Pt1ScanActionsDb = new AMDbUpgrade1Pt1("scanActions", this);
	appendDatabaseUpgrade(am1Pt1ScanActionsDb);

	// Append the AM upgrade 1.2 to the list for the user database
	AMDbUpgrade *am1Pt2UserDb = new AMDbUpgrade1Pt2("user", this);
	appendDatabaseUpgrade(am1Pt2UserDb);
	AMDbUpgrade *am1Pt2ActionsDb = new AMDbUpgrade1Pt2("actions", this);
	appendDatabaseUpgrade(am1Pt2ActionsDb);
	AMDbUpgrade *am1Pt2ScanActionsDb = new AMDbUpgrade1Pt2("scanActions", this);
	appendDatabaseUpgrade(am1Pt2ScanActionsDb);

	// Append the AM upgrade 1.3 to the list for the user, actions, and scanActions database.
	AMDbUpgrade *am1Pt3UserDb = new AMDbUpgrade1Pt3("user", this);
	appendDatabaseUpgrade(am1Pt3UserDb);
	AMDbUpgrade *am1Pt3ActionsDb = new AMDbUpgrade1Pt3("actions", this);
	appendDatabaseUpgrade(am1Pt3ActionsDb);
	AMDbUpgrade *am1Pt3ScanActionsDb = new AMDbUpgrade1Pt3("scanActions", this);
	appendDatabaseUpgrade(am1Pt3ScanActionsDb);

	// Append the AM upgrade 1.4 to the list for the user database
	AMDbUpgrade *am1Pt4UserDb = new AMDbUpgrade1Pt4("user", this);
	appendDatabaseUpgrade(am1Pt4UserDb);
	AMDbUpgrade *am1Pt4ActionsDb = new AMDbUpgrade1Pt4("actions", this);
	appendDatabaseUpgrade(am1Pt4ActionsDb);
	AMDbUpgrade *am1Pt4ScanActionsDb = new AMDbUpgrade1Pt4("scanActions", this);
	appendDatabaseUpgrade(am1Pt4ScanActionsDb);

	// Append the AM upgrade 1.5 to the list for the user database
	AMDbUpgrade *am1Pt5UserDb = new AMDbUpgrade1Pt5("user", this);
	appendDatabaseUpgrade(am1Pt5UserDb);
	AMDbUpgrade *am1Pt5ActionsDb = new AMDbUpgrade1Pt5("actions", this);
	appendDatabaseUpgrade(am1Pt5ActionsDb);
	AMDbUpgrade *am1Pt5ScanActionsDb = new AMDbUpgrade1Pt5("scanActions", this);
	appendDatabaseUpgrade(am1Pt5ScanActionsDb);

	// Append the AM upgrade 1.5 to the list for the user database
	AMDbUpgrade *am1Pt6UserDb = new AMDbUpgrade1Pt6("user", this);
	appendDatabaseUpgrade(am1Pt6UserDb);
	AMDbUpgrade *am1Pt6ActionsDb = new AMDbUpgrade1Pt6("actions", this);
	appendDatabaseUpgrade(am1Pt6ActionsDb);
	AMDbUpgrade *am1Pt6ScanActionsDb = new AMDbUpgrade1Pt6("scanActions", this);
	appendDatabaseUpgrade(am1Pt6ScanActionsDb);
}

bool AMDatamanAppController::startup() {

	/*
	AMErrorMon::enableDebugNotifications(true);
	*/

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

	isFirstTimeRun_ = startupIsFirstTime();
	if(firstTimeError_)
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_STARTUP_ERROR_ISFIRSTTIME, "Problem with Acquaman startup: error detected while determining if first-time user.");

	if(isFirstTimeRun_) {
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

	if(!startupCheckExportDirectory())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_CANT_CREATE_EXPORT_FOLDER, "Problem with Acquaman startup: checking export data directory.");

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
	if(!s.contains("userDataFolder") && s.contains("remoteDataFolder")) {
		QMessageBox::warning(0, "Local Storage Problem?", "Acquaman has detected a problem with your local storage.\n\nIt appears that synchronization was done at some point, but the local storage information has since been removed from the configuration.\nPlease stop and contact your beamline's Acquaman Developer for assistance.", QMessageBox::Ok);
		firstTimeError_ = true;
	}
	else if(!s.contains("remoteDataFolder") && s.contains("userDataFolder") && s.value("userDataFolder").toString().startsWith("/AcquamanLocalData")){
		QMessageBox::warning(0, "Local Storage Problem?", "Acquaman has detected a problem with your local storage.\n\nIt appears that synchronization was done at some point, but the remote storage information has since been manually edited incorrectly.\nPlease stop and contact your beamline's Acquaman Developer for assistance.", QMessageBox::Ok);
		firstTimeError_ = true;
	}
	else if(!s.contains("userDataFolder")) {
		isFirstTime = true;
	}
	else if(s.contains("userDataFolder") && s.contains("remoteDataFolder")){
		QDir remoteDataDir(AMUserSettings::remoteDataFolder);
		if(!remoteDataDir.exists())
			isFirstTime = true;

		QString filename = AMUserSettings::remoteDataFolder + "/" + AMUserSettings::userDatabaseFilename;
		QFile dbFile(filename);
		if(!dbFile.exists())
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
		AMFirstTimeWizard *firstTimeWizard = new AMFirstTimeWizard(AMUserSettings::userBasedDataStorage);

		firstTimeWizard->setField("useLocalStorage", QVariant(defaultUseLocalStorage_));

		// We're pretty forceful here... The user needs to accept this dialog.
		if(firstTimeWizard->exec() != QDialog::Accepted) {
			firstTimeWizard->deleteLater();
			return false;
		}

		bool userLocalStorage = firstTimeWizard->field("useLocalStorage").toBool();
		QString userDataFolder = firstTimeWizard->field("userDataFolder").toString();
		QString userName = firstTimeWizard->field("userName").toString();

		firstTimeWizard->deleteLater();

		if(userLocalStorage){
			AMUserSettings::remoteDataFolder = userDataFolder;
			if(!AMUserSettings::remoteDataFolder.endsWith('/'))
				AMUserSettings::remoteDataFolder.append("/");

			QString localUserDataFolder = AMUserSettings::remoteDataFolder;
			AMUserSettings::userDataFolder = localUserDataFolder.section('/', 2, -1).prepend("/AcquamanLocalData/");
		}
		else{
			AMUserSettings::userDataFolder = userDataFolder;
			if(!AMUserSettings::userDataFolder.endsWith('/'))
				AMUserSettings::userDataFolder.append("/");
		}

		AMUserSettings::save();

		// Attempt to create user's data folder, only if it doesn't exist:
		QDir userDataDir(AMUserSettings::userDataFolder);
		if(!userDataDir.exists()) {
			AMErrorMon::information(0, 0, "Creating new user data folder: "  + AMUserSettings::userDataFolder);
			if(!userDataDir.mkpath(AMUserSettings::userDataFolder)) {
				AMErrorMon::error(0, 0, "Could not create user data folder " + AMUserSettings::userDataFolder);
				return false;
			}
		}

		if(userLocalStorage){
			// Attempt to create user's data folder, only if it doesn't exist:
			QDir remoteDataDir(AMUserSettings::remoteDataFolder);
			if(!remoteDataDir.exists()) {
				AMErrorMon::information(0, 0, "Creating new remote data folder: "  + AMUserSettings::remoteDataFolder);
				if(!remoteDataDir.mkpath(AMUserSettings::remoteDataFolder)) {
					AMErrorMon::error(0, 0, "Could not create remote data folder " + AMUserSettings::remoteDataFolder);
					return false;
				}
			}
		}

		// Find out the user's name:
		AMUser::user()->setName( userName );

		if(!startupCreateDatabases())
			return false;

		// check for and run any database upgrades we require...
		if(!startupDatabaseUpgrades())
			return false;

		if(userLocalStorage){
			QDir userDataFolder(AMUserSettings::userDataFolder);
			QStringList dataFolderFilters;
			dataFolderFilters << "*.db";
			QFileInfoList allDatabaseFiles = userDataFolder.entryInfoList(dataFolderFilters);
			for(int x = 0, size = allDatabaseFiles.count(); x < size; x++)
				QFile::copy(allDatabaseFiles.at(x).absoluteFilePath(), QString("%1/%2").arg(AMUserSettings::remoteDataFolder).arg(allDatabaseFiles.at(x).fileName()));
		}

		if(usingLocalStorage()) {

			storageInfo_ = AMStorageInfo(AMUserSettings::userDataFolder);

			// start timer for updates every 1 minute
			timerIntervalID_ = startTimer(60000);
		}

		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: First-Time Successful");
		qApp->processEvents();
	}
	return true;
}

bool AMDatamanAppController::startupOnEveryTime()
{
	if(!usingLocalStorage() && defaultUseLocalStorage_){
		int retVal = QMessageBox::question(0, "Use Local Storage?", "Acquaman has detected that you are not using local storage.\nLocal storage can significantly improve speed and reliability.\n If you wish to use local storage select \"Yes\" and your data will automatically be synchronized to the network for long term storage.\n\n Use local storage?", QMessageBox::Yes, QMessageBox::No);
		if(retVal == QMessageBox::Yes){
			QString currentUserDataFolder = AMUserSettings::userDataFolder;
			QString localBaseDirectory = "/AcquamanLocalData";

			QString dataFolderExtension = currentUserDataFolder;
			if(currentUserDataFolder.contains("/home"))
				dataFolderExtension.remove("/home");
			else if(currentUserDataFolder.contains("/experiments"))
				dataFolderExtension.remove("/experiments");
			else
				dataFolderExtension.remove(currentUserDataFolder.section('/', 0, 1));

			QString newUserDataFolder = localBaseDirectory+dataFolderExtension;
			QString newRemoteDataFolder = currentUserDataFolder;

			AMUserSettings::userDataFolder = newUserDataFolder;
			AMUserSettings::remoteDataFolder = newRemoteDataFolder;
			AMUserSettings::save();
		}
	}

	if(!startupBackupDataDirectory())
		return AMErrorMon::errorAndReturn(this, AMDATAMANAPPCONTROLLER_DATA_DIR_BACKUP_ERROR, "Problem with Acquaman startup: backing up data directory.");

	if(!startupCreateDatabases())
		return false;

	// check for and run any database upgrades we require...
	if(!startupDatabaseUpgrades())
		return false;

	if(usingLocalStorage()) {

		storageInfo_ = AMStorageInfo(AMUserSettings::userDataFolder);

		// start timer for updates every 1 minute
		timerIntervalID_ = startTimer(60000);
	}

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

bool AMDatamanAppController::startupBackupDataDirectory()
{
	if(AMUserSettings::remoteDataFolder.length() > 0)
	{
		AMDirectorySynchronizerDialog *synchronizer = new AMDirectorySynchronizerDialog(AMUserSettings::userDataFolder, AMUserSettings::remoteDataFolder, "Local", "Network");

		QStringList excludePatterns;
		excludePatterns.append("*.db.bk.*");
		excludePatterns.append("*.BACKUPS");
		excludePatterns.append("*.db-journal");
		excludePatterns.append("*exportData*");
		for(int x = 0, size = excludePatterns.size(); x < size; x++)
			synchronizer->appendExcludePattern(excludePatterns.at(x));

		return synchronizer->autoStart();
	}
	return true;
}

bool AMDatamanAppController::startupCheckExportDirectory()
{
	QDir exportDir;
	if(!AMUserSettings::remoteDataFolder.isEmpty())
		exportDir.setCurrent(AMUserSettings::remoteDataFolder);
	else
		exportDir.setCurrent(AMUserSettings::userDataFolder);

	if (exportDir.absolutePath().contains("/userData"))
		exportDir.cdUp();

	if(!exportDir.entryList(QDir::AllDirs).contains("exportData")){
		if(!exportDir.mkdir("exportData"))
			return false;
		AMUser::user()->setLastExportDestination(QString("%1/exportData").arg(exportDir.absolutePath()));
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
			AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_REQUIREMENT_FAILURE, QString("Failure in initialization of upgrade table, there must be tracking required for new databases that the user application is responsible for. Database: %1 Upgrade: %2.").arg(upgrade->databaseNameToUpgrade()).arg(upgrade->upgradeToTag()));
			return false;
		}

		// For the beamline database (and others like it) the upgrade better be done already
		if(!upgrade->isResponsibleForUpgrade() && upgrade->upgradeRequired()){
			AMErrorMon::alert(0, AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_SHARED_DB_FAILURE, QString("Failure in initialization of upgrade table, an upgrade is not done on a (shared) database [%1 -> %2] the user application is not responsible for").arg(upgrade->databaseNameToUpgrade()).arg(upgrade->upgradeToTag()));
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
	int lastErrorCode = -1;

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
					lastErrorString = QString("Failure to create or find backup directory for upgrade %1 in database %2").arg(upgrade->upgradeToTag()).arg(upgrade->databaseNameToUpgrade());
					lastErrorCode = AMDATAMANAPPCONTROLLER_DB_UPGRADE_BACKUPS_DIRECTORY_NOT_FOUND;
					success = false;
				}
			}
			if(success && !databaseBackupDir.cd(backupsSubFolder)){
				databaseBackupDir.mkdir(backupsSubFolder);
				if(!databaseBackupDir.cd(backupsSubFolder)){
					lastErrorString = QString("Failure to create or find backup sub directory for upgrade %1 in database %2 as %3").arg(upgrade->upgradeToTag()).arg(upgrade->databaseNameToUpgrade()).arg(backupsSubFolder);
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
					lastErrorString = QString("Upgrade run failed for upgrade %1 on database %2").arg(upgrade->upgradeToTag()).arg(upgrade->databaseNameToUpgrade());
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

AMScanConfigurationView* AMDatamanAppController::createScanConfigurationViewFromDb(const QUrl &url)
{
	// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);

	AMScan* scan = AMScan::createFromDatabaseUrl(url, true);

	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);

	if(!scan) {
		return 0;
	}

	// Does the scan have a configuration?
	AMScanConfiguration* scanConfiguration = scan->scanConfiguration();
	if(!scanConfiguration) {
		scan->deleteLater();
		return 0;
	}
	// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
	scanConfiguration = scanConfiguration->createCopy();
	scan->deleteLater();
	if(!scanConfiguration)
		return 0;

	AMScanConfigurationView *configurationView = scanConfiguration->createView();
	if(!configurationView) {
		scanConfiguration->deleteLater();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -401, "Unable to create view from the scan configuration loaded from the database.  Contact Acquaman developers."));
		return 0;
	}

	return configurationView;
}

void AMDatamanAppController::launchScanConfigurationFromDb(const QUrl &url)
{
	AMScanConfigurationView *configurationView = createScanConfigurationViewFromDb(url);
	if (configurationView) {
		configurationView->setEnabled(false);
		configurationView->setAttribute(Qt::WA_DeleteOnClose, true);
		configurationView->show();
	}
}

QString AMDatamanAppController::getStylesheet() const
{
	// Go through list of stylesheets to be applied,
	// composing a 'master' sheet.

	QString stylesheet;

	// AMToolButton

	QFile qss1(":/AMToolButton.qss");

	if (qss1.open(QFile::ReadOnly))
		stylesheet.append(QString("\n\n%1").arg(QLatin1String(qss1.readAll())));

	qss1.close();

	// AMDeadTimeButton

	QFile qss2(":/AMDeadTimeButton.qss");

	if (qss2.open(QFile::ReadOnly))
		stylesheet.append(QString("\n\n%1").arg(QLatin1String(qss2.readAll())));

	qss2.close();

	// Return master sheet.

	return stylesheet;
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

	success &= AMDbObjectSupport::s()->registerClass<AMStepScanConfiguration>();
	success &= AMDbObjectSupport::s()->registerClass<AMTimedRegionScanConfiguration>();
	success &= AMDbObjectSupport::s()->registerClass<AMGenericStepScanConfiguration>();
	success &= AMDbObjectSupport::s()->registerClass<AMGenericContinuousScanConfiguration>();
    success &= AMDbObjectSupport::s()->registerClass<AMXRFScanConfiguration>();

	success &= AMDbObjectSupport::s()->registerClass<AMRun>();
	success &= AMDbObjectSupport::s()->registerClass<AMExperiment>();
	success &= AMDbObjectSupport::s()->registerClass<AMSamplePre2013>();
	success &= AMDbObjectSupport::s()->registerClass<AMFacility>();

	success &= AMDbObjectSupport::s()->registerClass<AMRawDataSource>();
	success &= AMDbObjectSupport::s()->registerClass<AMAnalysisBlock>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DExpressionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DSummingAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DDerivativeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DIntegralAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DSummingAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMDeadTimeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DNormalizationAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DNormalizationAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DCalibrationAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DAdditionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DAdditionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DBinningAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DDeadTimeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DDeadTimeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMOrderReductionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DDeadTimeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM2DDeadTimeCorrectionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DDeadTimeCorrectionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM0DAccumulatorAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DTimedDataAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DKSpaceCalculatorAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM3DNormalizationAB>();
	success &= AMDbObjectSupport::s()->registerClass<AM1DDarkCurrentCorrectionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMnDDeadTimeAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMNormalizationAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMAdditionAB>();
	success &= AMDbObjectSupport::s()->registerClass<AMnDDeadTimeAB>();

	success &= AMDbObjectSupport::s()->registerClass<AMScanAxis>();
	success &= AMDbObjectSupport::s()->registerClass<AMScanAxisRegion>();
	success &= AMDbObjectSupport::s()->registerClass<AMScanAxisEXAFSRegion>();

	success &= AMDbObjectSupport::s()->registerClass<AMOldDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<AMSpectralOutputDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<AMDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<AMControlInfo>();

	success &= AMDbObjectSupport::s()->registerClass<AMControlInfoList>();
	success &= AMDbObjectSupport::s()->registerClass<AMOldDetectorInfoSet>();
	success &= AMDbObjectSupport::s()->registerClass<AMDetectorInfoSet>();
	success &= AMDbObjectSupport::s()->registerClass<AMSamplePositionPre2013>();
	success &= AMDbObjectSupport::s()->registerClass<AMSamplePlatePre2013>();
	success &= AMDbObjectSupport::s()->registerClass<AMRegionOfInterest>();
	success &= AMDbObjectSupport::s()->registerClass<AMRegionOfInterestAB>();

	success &= AMDbObjectSupport::s()->registerClass<AMExporterOptionGeneralAscii>();
	success &= AMDbObjectSupport::s()->registerClass<AMExporterOptionSMAK>();
	success &= AMDbObjectSupport::s()->registerClass<AMExporterOptionXDIFormat>();

	success &= AMDbObjectSupport::s()->registerClass<AMUser>();

	success &= AMDbObjectSupport::s()->registerClass<AMCameraConfiguration>();
	success &= AMDbObjectSupport::s()->registerClass<AMRotationalOffset>();
	success &= AMDbObjectSupport::s()->registerClass<AMBeamConfiguration>();
	success &= AMDbObjectSupport::s()->registerClass<AMSample>();
	success &= AMDbObjectSupport::s()->registerClass<AMSamplePlate>();
	success &= AMDbObjectSupport::s()->registerClass<AMSampleCameraURL>();

	success &= AMDbObjectGeneralViewSupport::registerClass<AMDbObject, AMDbObjectGeneralView>();

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

	// Also on first time only: populate default data.
	startupPopulateNewUserDBTables(db);

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
	AMExportController::registerExporter<AMExporterXDIFormat>();
	AMExportController::registerExporter<AMSMAKExporter>();
	AMExportController::registerExporter<AMExporter2DAscii>();

	return true;
}

bool AMDatamanAppController::startupCreateUserInterface()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Populating User Interface");
	qApp->processEvents();
	settingsMasterView_ = 0;
	issueSubmissionView_ = 0;

	scanEditorCloseView_ = 0;

	// Apply stylesheets.
	applyStylesheets();

	//Create the main tab window:

	mw_ = new AMMainWindow();
	mw_->setWindowTitle("Acquaman");
	connect(mw_, SIGNAL(itemCloseButtonClicked(QModelIndex)), this, SLOT(onWindowPaneCloseButtonClicked(QModelIndex)));
	connect(mw_, SIGNAL(itemRightClicked(QModelIndex,QPoint)), this, SLOT(onWindowPaneRightClicked(QModelIndex,QPoint)));
	mw_->installEventFilter(this);

	addBottomPanel();
	mw_->addBottomWidget(bottomPanel_);

	// Create panes in the main window:
	////////////////////////////////////

	// A heading for the scan editors
	scanEditorsParentItem_ = mw_->windowPaneModel()->headingItem("Open Scans");

	// Make a dataview widget and add it under two links/headings: "Runs" and "Experiments". See AMMainWindowModel for more information.
	////////////////////////////////////
	dataView_ = new AMScanDataView(AMDatabase::database("user"));
	dataView_->setWindowTitle("Data");

	QStandardItem* dataViewItem = new QStandardItem();
	dataViewItem->setData(qVariantFromValue((QWidget*)dataView_), AM::WidgetRole);
	dataViewItem->setFlags(Qt::ItemIsEnabled);	// enabled, but should not be selectable
	QFont font = QFont("Lucida Grande", 10, QFont::Bold);
	font.setCapitalization(QFont::AllUppercase);
	dataViewItem->setFont(font);
	dataViewItem->setData(QBrush(QColor::fromRgb(100, 109, 125)), Qt::ForegroundRole);
	dataViewItem->setData(true, AMWindowPaneModel::DockStateRole);
	//dataViewItem->setData(true, AMWindowPaneModel::IsVisibleRole);

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
	QAction *saveAllAction = new QAction("Save All Scans", mw_);
	saveAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S)); //+ Qt::SHIFT
	saveAllAction->setStatusTip("Save all unsaved scans without prompting.");
	connect(saveAllAction, SIGNAL(triggered()), this, SLOT(saveAll()));

	QAction *forceQuitAction = new QAction("Force Quit Acquaman", mw_);
	forceQuitAction->setStatusTip("Acquaman is behaving poorly, force a quit and loose any unsaved changes or currently running scans");
	connect(forceQuitAction, SIGNAL(triggered()), this, SLOT(forceQuitAcquaman()));


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

	QAction* amShowAboutPageAction = new QAction("About...", mw_);
	amShowAboutPageAction->setStatusTip("About Acquaman");
	connect(amShowAboutPageAction, SIGNAL(triggered()), this, SLOT(onShowAboutPage()));

	QAction* amCloseScanEditorsAction = new QAction("Close Scan Editors", mw_);
	amCloseScanEditorsAction->setStatusTip("Close One or More Open Scan Editors");
	connect(amCloseScanEditorsAction, SIGNAL(triggered()), this, SLOT(openScanEditorsCloseView()));

	exportGraphicsAction_ = new QAction("Export Plot...", mw_);
	exportGraphicsAction_->setStatusTip("Export the current plot to a PDF file.");
	connect(exportGraphicsAction_, SIGNAL(triggered()), this, SLOT(onActionExportGraphics()));

	printGraphicsAction_ = new QAction("Print Plot...", mw_);
	printGraphicsAction_->setStatusTip("Print the current plot.");
	connect(printGraphicsAction_, SIGNAL(triggered()), this, SLOT(onActionPrintGraphics()));

	QAction *openOtherDatabaseAction = new QAction("Open existing database", mw_);
	openOtherDatabaseAction->setStatusTip("Open another database.");
	connect(openOtherDatabaseAction, SIGNAL(triggered()), this, SLOT(onOpenOtherDatabaseClicked()));

	//install menu bar, and add actions
	//////////////////////////////////////
#ifdef Q_WS_MAC
	menuBar_ = new QMenuBar(0);
	internalStorageRemainingBar_ = 0;
	storageWarningLabel_ = 0;
#else
	// Construct the menu and, if using local storage, the space remaining progress bar
	menuBar_ = new QMenuBar();
	if(usingLocalStorage()) {

		QHBoxLayout* topWidgetLayout = new QHBoxLayout();
		topWidgetLayout->setContentsMargins(0,0,0,0);

		QSizePolicy menuBarSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		menuBarSizePolicy.setHorizontalStretch(10);
		menuBar_->setSizePolicy(menuBarSizePolicy);

		topWidgetLayout->addWidget(menuBar_);

		QFrame* storageWidgetFrame = new QFrame();

		QSizePolicy storageWidgetSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		storageWidgetSizePolicy.setHorizontalStretch(2);
		storageWidgetFrame->setSizePolicy(storageWidgetSizePolicy);

		QHBoxLayout* storageWidgetLayout = new QHBoxLayout();
		storageWidgetLayout->setContentsMargins(0,0,0,0);

		storageWidgetFrame->setLayout(storageWidgetLayout);
		topWidgetLayout->addWidget(storageWidgetFrame);


		storageWarningLabel_ = new QLabel();
		storageWarningLabel_->setPixmap(QIcon(":/dialog-warning.png").pixmap(22,22));
		storageWarningLabel_->setVisible(false);
		storageWarningLabel_->setToolTip(QString("Storage space running low"));

		storageWidgetLayout->addWidget(storageWarningLabel_);


		internalStorageRemainingBar_ = new QProgressBar();
		internalStorageRemainingBar_->setFormat(QString("Space Used: %p%"));

		storageWidgetLayout->addWidget(internalStorageRemainingBar_);


		QFrame* topWidgetFrame = new QFrame();
		topWidgetFrame->setLayout(topWidgetLayout);


		mw_->addTopWidget(topWidgetFrame);

		updateStorageProgressBar();
	} else {

		internalStorageRemainingBar_ = 0;
		storageWarningLabel_ = 0;
		mw_->addTopWidget(menuBar_);
	}


#endif

	fileMenu_ = menuBar_->addMenu("File");
	fileMenu_->addAction(saveAllAction);
	fileMenu_->addSeparator();
	fileMenu_->addAction(importLegacyFilesAction);
	fileMenu_->addAction(importAcquamanDatabaseAction);
	fileMenu_->addSeparator();
	fileMenu_->addAction(exportGraphicsAction_);
	fileMenu_->addAction(printGraphicsAction_);
	fileMenu_->addSeparator();
	fileMenu_->addAction(amSettingsAction);
	fileMenu_->addSeparator();
	fileMenu_->addAction(openOtherDatabaseAction);
	fileMenu_->addSeparator();
	fileMenu_->addAction(forceQuitAction);

	viewMenu_ = menuBar_->addMenu("View");
	viewMenu_->addAction(amCloseScanEditorsAction);

	helpMenu_ = menuBar_->addMenu("Help");
	helpMenu_->addAction(amIssueSubmissionAction);
	helpMenu_->addAction(amShowAboutPageAction);

	return true;
}


void AMDatamanAppController::shutdown() {

	isShuttingDown_ = true;

	// destroy the main window. This will delete everything else within it.
	mw_->deleteLater();

	// Close down connection to the user Database
	AMDatabase::deleteDatabase("user");

	foreach (AMDatabase *database, otherOpenDatabases_)
		AMDatabase::deleteDatabase(database->connectionName());

	otherOpenDatabases_.clear();
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

void AMDatamanAppController::applyStylesheets()
{
	qApp->setStyleSheet( getStylesheet() );
}

#include "dataman/AMScanEditorModelItem.h"

void AMDatamanAppController::onCurrentPaneChanged(QWidget *pane)
{
	for (int i = 0, size = scanEditorScanMapping_.size(); i < size; i++)
		if (pane == scanEditorScanMapping_.at(i).second)
			((AMScanEditorModelItem *)(mw_->windowPaneModel()->itemFromIndex(mw_->windowPaneModel()->indexForPane(pane))))->editorWasClicked();

	// This is okay because both AMScanView and AM2DScanView have export capabilities.
	exportGraphicsAction_->setEnabled(qobject_cast<AMGenericScanEditor *>(pane) != 0);
	printGraphicsAction_->setEnabled(qobject_cast<AMGenericScanEditor *>(pane) != 0);

}

void AMDatamanAppController::onMainWindowAliasItemActivated(QWidget *target, const QString &key, const QVariant &value) {

	if(target == dataView_) {
		if(key == "Runs")
			dataView_->showRun(value.toInt());
		if(key == "Experiments")
			dataView_->showExperiment(value.toInt());
	}
}

void AMDatamanAppController::onNewExperimentAdded(const QModelIndex &index) {
	mw_->expandHeadingIndex(index.parent()); //Do this to show people where it ended up...
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

void AMDatamanAppController::onDataViewItemsActivated(const QList<QUrl>& itemUrls)
{
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
	/// \todo bad code, improve this with better architecture and functionality in expItem.  Don't like trusting dynamic_cast, there's no guarantee that someone didn't put a non-AMExperimentModelItem into the model under experimentsParentItem_.
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

void AMDatamanAppController::onWindowPaneRightClicked(const QModelIndex &index, const QPoint &globalPosition){
	if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == scanEditorsParentItem_) {
		AMGenericScanEditor *editor = qobject_cast<AMGenericScanEditor*>(index.data(AM::WidgetRole).value<QWidget*>());
		if(!editor)
			return;

		QMenu *menu = new QMenu();
		menu->addAction("Close Options...");
		QAction *temp = menu->exec(globalPosition);

		if (temp){
			openScanEditorsCloseView(editor);
			temp->deleteLater();
		}
		menu->deleteLater();
	}
}

void AMDatamanAppController::onIssueSubmissionViewFinished(){
	if(!issueSubmissionView_)
		return;

	disconnect(issueSubmissionView_, SIGNAL(finished()), this, SLOT(onIssueSubmissionViewFinished()));
	issueSubmissionView_->deleteLater();
	issueSubmissionView_ = 0;
}

void AMDatamanAppController::onStartupFinished(){
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_FINISHED, "Acquaman Startup: Finished");
}

void AMDatamanAppController::onShowAboutPage()
{
	AMAboutDialog* aboutPage = new AMAboutDialog();
	aboutPage->setModal(true);
	aboutPage->exec();

	aboutPage->deleteLater();
}

void AMDatamanAppController::openScanEditorsCloseView(AMGenericScanEditor *editor)
{
	if(scanEditorCloseView_)
		scanEditorCloseView_->close();

	if(scanEditorsParentItem_->rowCount() != 0){
		scanEditorCloseView_ = new AMScanEditorsCloseView(scanEditorsParentItem_, this, editor);
		scanEditorCloseView_->show();
		connect(scanEditorCloseView_, SIGNAL(closeRequested(QList<AMGenericScanEditor*>)), this, SLOT(onScanEditorsCloseViewCloseRequested(QList<AMGenericScanEditor*>)));
		connect(scanEditorCloseView_, SIGNAL(closed()), this, SLOT(onScanEditorsCloseViewClosed()));
	}
	else
		QMessageBox::information(0, "No Scan Editors To Open", "There are no scan editors open at this time", QMessageBox::Ok);
}

void AMDatamanAppController::onScanEditorsCloseViewCloseRequested(QList<AMGenericScanEditor *> editorsToClose)
{

	AMGenericScanEditor * scanEditor;
	QList<AMGenericScanEditor*> availableEditors_;
	for(int x = 0, size = scanEditorsParentItem_->rowCount(); x < size; x++){
		scanEditor = qobject_cast<AMGenericScanEditor*>(scanEditorsParentItem_->child(x, 0)->data(AM::WidgetRole).value<QWidget*>());
		if(scanEditor)
			availableEditors_.append(scanEditor);
	}

	for(int x = 0, size = editorsToClose.count(); x < size; x++){
		if(availableEditors_.contains(editorsToClose.at(x)))
			closeScanEditor(editorsToClose.at(x));
	}
}

void AMDatamanAppController::onScanEditorsCloseViewClosed()
{
	disconnect(scanEditorCloseView_, SIGNAL(closeRequested(QList<AMGenericScanEditor*>)), this, SLOT(onScanEditorsCloseViewCloseRequested(QList<AMGenericScanEditor*>)));
	disconnect(scanEditorCloseView_, SIGNAL(closed()), this, SLOT(onScanEditorsCloseViewClosed()));

	scanEditorCloseView_->deleteLater();
	scanEditorCloseView_ = 0;
}

#include "dataman/export/AMExportController.h"
#include "ui/dataman/AMExportWizard.h"

void AMDatamanAppController::onDataViewItemsExported(const QList<QUrl> &itemUrls)
{
	if (anyOpenScansModified()){

		QMessageBox shouldSave;
		shouldSave.setText("Save changes to open scans before exporting?");
		shouldSave.setInformativeText("Unsaved changes will not export.");
		shouldSave.setStandardButtons(QMessageBox::Cancel | QMessageBox::No | QMessageBox::SaveAll);
		shouldSave.setDefaultButton(QMessageBox::SaveAll);
		shouldSave.setEscapeButton(QMessageBox::Cancel);
		int ret = shouldSave.exec();
		switch (ret) {

		case QMessageBox::SaveAll:

			saveAll();
			break;

		case QMessageBox::No:

			// Do nothing.
			break;

		case QMessageBox::Cancel:

			// Abort action.
			return;
		}
	}

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
	AMScanEditorModelItem *newItem = new AMScanEditorModelItem(editor, this);
	scanEditorsParentItem_->appendRow(newItem);
	emit scanEditorCreated(editor);
	return editor;
}

bool AMDatamanAppController::canCloseScanEditors() const
{
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

bool AMDatamanAppController::usingLocalStorage() const
{
	return !(AMUserSettings::remoteDataFolder.isEmpty());
}

bool AMDatamanAppController::defaultUseLocalStorage() const{
	return defaultUseLocalStorage_;
}

void AMDatamanAppController::setDefaultUseLocalStorage(bool defaultUseLocalStorage){
	defaultUseLocalStorage_ = defaultUseLocalStorage;
}

bool AMDatamanAppController::eventFilter(QObject* o, QEvent* e)
{
	if(o == mw_ && e->type() == QEvent::Close && !overrideCloseCheck_) {
		if(!canCloseScanEditors()) {
			e->ignore();
			return true;
		}

		if(AMUserSettings::remoteDataFolder.length() > 0)		{
			QStringList arguments;
			arguments << "-i" << "--delayedStart=3" << "--noWarningTimeout";

			QString pathToAM = QApplication::instance()->applicationFilePath().section('/', 0, -2);
			QProcess::startDetached(QString("%1/CLSNetworkDirectorySynchronizer").arg(pathToAM), arguments);
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
	if( !urls.count() )
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
		dbo->deleteLater();
		return 0;
	}

	// Change the scan in the database if necessary and then reload it.
	if (overwriteNecessary){

		scan->setScanController(0);
		scan->storeToDb(AMDatabase::database("user"));

		/// \todo DH: I'm sure I should just make a function to do things like this, but for now I'm just duplicating code because it's easy.
		dbo->deleteLater();

		dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
		if(!dbo)
			return 0;

		// Is it a scan?
		scan = qobject_cast<AMScan*>( dbo );
		if(!scan) {
			dbo->deleteLater();
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

void AMDatamanAppController::onActionPrintGraphics()
{
	AMGenericScanEditor* scanEditor = qobject_cast<AMGenericScanEditor*>(mw_->currentPane());

	if(scanEditor) {
		scanEditor->printGraphics();
	}
	else {
		// This can't happen with the current code.  Only accessible from the QAction from the file drop down menu.  Takes into account whether the current pane is a scan editor already.
		AMErrorMon::alert(this, -1111, "To export graphics, you must have a plot open in a scan editor.");
	}
}

void AMDatamanAppController::forceQuitAcquaman(){
	overrideCloseCheck_ = true;
	mw_->close();
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
		return;	// user cancelled, do nothing.

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

void AMDatamanAppController::saveAll()
{
	for (int i = 0, count = scanEditorCount(); i < count; i++) {

		AMGenericScanEditor* editor = scanEditorAt(i);

		if(editor){

			for (int i = 0, scanCount = editor->scanCount(); i < scanCount; i++)
			{
				AMScan* scan =  editor->scanAt(i);

				if(scan->modified() && scan->database())
					scan->storeToDb(scan->database());

				else if (scan->modified())
					scan->storeToDb(AMDatabase::database("user"));
			}
		}
	}
}

bool AMDatamanAppController::anyOpenScansModified() const
{
	for (int i = 0, count = scanEditorCount(); i < count; i++) {

		AMGenericScanEditor* editor = scanEditorAt(i);

		if(editor)
			for (int i = 0, scanCount = editor->scanCount(); i < scanCount; i++)
				if (editor->scanAt(i)->modified())
					return true;
	}

	return false;
}

void AMDatamanAppController::onOpenOtherDatabaseClicked()
{
	QString otherFileName = QFileDialog::getOpenFileName(0, "Open other database...", ".", "*.db");

	if (!otherFileName.isEmpty()){

		QStringList otherFileNameList = otherFileName.split("/");
		QString otherDatabaseName = otherFileNameList.at(otherFileNameList.indexOf("userData")-1).toLower();

		AMDatabase *otherDatabase = AMDatabase::createDatabase(otherDatabaseName, otherFileName, true);
		otherOpenDatabases_ << otherDatabase;
		AMScanDataView *newScanDataView = new AMScanDataView(otherDatabase);

		// Make a dataview widget and add it under two links/headings: "Runs" and "Experiments". See AMMainWindowModel for more information.
		////////////////////////////////////

		newScanDataView->setWindowTitle(otherDatabaseName);

		QStandardItem* dataViewItem = new QStandardItem();
		dataViewItem->setData(qVariantFromValue((QWidget*)newScanDataView), AM::WidgetRole);
		dataViewItem->setFlags(Qt::ItemIsEnabled);	// enabled, but should not be selectable
		QFont font = QFont("Lucida Grande", 10, QFont::Bold);
		font.setCapitalization(QFont::AllUppercase);
		dataViewItem->setFont(font);
		dataViewItem->setData(QBrush(QColor::fromRgb(100, 109, 125)), Qt::ForegroundRole);
		dataViewItem->setData(true, AMWindowPaneModel::DockStateRole);

		mw_->windowPaneModel()->appendRow(dataViewItem);

		QStandardItem *runsParentItem = new QStandardItem(QIcon(":/22x22/lock.png"), "Runs");
		mw_->windowPaneModel()->initAliasItem(runsParentItem, dataViewItem, "Runs", -1);
		dataViewItem->appendRow(runsParentItem);

		new AMRunExperimentInsert(otherDatabase, runsParentItem, 0, this);

		// connect the activated signal from the dataview to our own slot
		connect(newScanDataView, SIGNAL(selectionActivated(QList<QUrl>)), this, SLOT(onDataViewItemsActivated(QList<QUrl>)));
		connect(newScanDataView, SIGNAL(selectionActivatedSeparateWindows(QList<QUrl>)), this, SLOT(onDataViewItemsActivatedSeparateWindows(QList<QUrl>)));
		connect(newScanDataView, SIGNAL(selectionExported(QList<QUrl>)), this, SLOT(onDataViewItemsExported(QList<QUrl>)));
		connect(newScanDataView, SIGNAL(launchScanConfigurationsFromDb(QList<QUrl>)), this, SLOT(onLaunchScanConfigurationsFromDb(QList<QUrl>)));
	}
}

void AMDatamanAppController::timerEvent(QTimerEvent *)
{
	updateStorageProgressBar();
}

void AMDatamanAppController::updateStorageProgressBar()
{
	if(usingLocalStorage() && storageInfo_.isValid()) {

		storageInfo_.refresh();
		double storageUsed = double(storageInfo_.bytesTotal() - storageInfo_.bytesAvailable());
		double percentageUsed = (storageUsed / storageInfo_.bytesTotal()) * 100;

		if(percentageUsed > 85) {

			if(storageWarningLabel_) {
				storageWarningLabel_->setVisible(true);
			}

			// Only show the warning every 60 minutes
			if(storageWarningCount_ % 60 == 0) {
				double percentageRemaining = 100 - percentageUsed;
				AMErrorMon::alert(this,
				                  AMDATAMANAPPCONTROLLER_LOCAL_STORAGE_RUNNING_LOW, QString("Warning: Local storage space is at %1%. Please inform the beamline staff.").arg(percentageRemaining),
				                  true);				
			}

			storageWarningCount_++;

		} else {

			if (storageWarningLabel_) {

				storageWarningLabel_->setVisible(false);
			}

			storageWarningCount_ = 0;
		}

		if(internalStorageRemainingBar_) {

			internalStorageRemainingBar_->setValue(int(percentageUsed));
		}
	}
}

