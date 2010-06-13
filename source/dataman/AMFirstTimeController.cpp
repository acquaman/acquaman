#include "AMFirstTimeController.h"


AMFirstTimeController::AMFirstTimeController() {

	bool isFirstTime;

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
		QString filename = AMUserSettings::userDataFolder + AMUserSettings::userDatabaseFilename;
		QFile dbFile(filename);
		if(!dbFile.exists()) {
			isFirstTime = true;
		}
	}


	if(isFirstTime)
		onFirstTime();

	databaseUpgrade();
}

#include <QApplication>

void AMFirstTimeController::onFirstTime() {

	AMFirstTimeWizard ftw;
	if(ftw.exec() != QDialog::Accepted) {
		// figure out how to quit the main program from here.  We might not be inside the application run loop yet.
	}

	AMUserSettings::userName = ftw.field("userName").toString();
	AMUserSettings::userDataFolder = ftw.field("userDataFolder").toString();
	if(!AMUserSettings::userDataFolder.endsWith('/'))
		AMUserSettings::userDataFolder.append("/");

	AMUserSettings::save();

	// Attempt to create user's data folder, only if it doesn't exist:
	QDir userDataDir(AMUserSettings::userDataFolder);
	if(!userDataDir.exists()) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, 0, "Creating new user data folder: "  + AMUserSettings::userDataFolder));
		if(!userDataDir.mkpath(AMUserSettings::userDataFolder))
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, 0, "Could not create user data folder " + AMUserSettings::userDataFolder));
	}

	// initialize the database
	QString filename = AMUserSettings::userDataFolder + AMUserSettings::userDatabaseFilename;
	QFile dbFile(filename);
	if(!dbFile.exists())
		databaseInitialization();
	else
		databaseUpgrade();


}

/// create structures and tables for a new user database, from scratch
void AMFirstTimeController::databaseInitialization() {

	/// \todo Handle situations where the proper tables exist, but aren't in the right format.

	AMDatabase::userdb()->ensureTable("ObjectTypes", QString("type,columnNames").split(','), QString("TEXT,TEXT").split(','));
	AMDatabase::userdb()->ensureTable("Objects", QString("type,dateTime,name,number,runId").split(','), QString("TEXT,TEXT,TEXT,INTEGER,INTEGER").split(','));
	AMDatabase::userdb()->ensureTable("Runs", QString("dateTime,name").split(','), QString("TEXT,TEXT").split(','));
	AMDatabase::userdb()->ensureTable("Experiments", QString("dateTime,name").split(','), QString("TEXT,TEXT").split(','));
	AMDatabase::userdb()->ensureTable("ExperimentEntries", QString("objectId,experimentId").split(','), QString("INTEGER,INTEGER").split(','));

	AMDatabase::userdb()->ensureTable("DatabaseInformation", QString("key,value").split(','), QString("TEXT,TEXT").split(','));

	/// \todo: generalize insertOrUpdate() to work with any table. then insertOrUpdate userName and databaseVersion into DatabaseInformation table.

}

/// Check whether the user database is the most recent version, and migrate if required.
void AMFirstTimeController::databaseUpgrade() {
	/// \todo
}
