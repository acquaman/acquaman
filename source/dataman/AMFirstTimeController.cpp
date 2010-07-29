#include "AMFirstTimeController.h"
#include <QStringList>

AMFirstTimeController::AMFirstTimeController() {

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



#include <dataman/AMDatabaseDefinition.h>
#include <dataman/AMXASScan.h>
#include <dataman/AMRun.h>
#include <dataman/AMSample.h>
#include <dataman/AMExperiment.h>
#include <dataman/AMDetectorInfo.h>

/// create structures and tables for a new user database, from scratch
void AMFirstTimeController::databaseInitialization() {

	AMDatabaseDefinition::initializeDatabaseTables(AMDatabase::userdb());

	AMDbObject s1;
	AMDatabaseDefinition::registerType(&s1, AMDatabase::userdb());
	AMScan s2;
	AMDatabaseDefinition::registerType(&s2, AMDatabase::userdb());
	AMXASScan s3;
	AMDatabaseDefinition::registerType(&s3, AMDatabase::userdb());

	AMRun r;
	AMDatabaseDefinition::registerType(&r, AMDatabase::userdb());

	AMExperiment e;
	AMDatabaseDefinition::registerType(&e, AMDatabase::userdb());

	AMSample s;
	AMDatabaseDefinition::registerType(&s, AMDatabase::userdb());

	AMDetectorInfo d("prototypeDetector", "Generic Detector Info");
	AMDatabaseDefinition::registerType(&d, AMDatabase::userdb());

	AMSpectralOutputDetectorInfo sod("prototypeDetector", "Generic Spectral-output detector", 4);
	AMDatabaseDefinition::registerType(&sod, AMDatabase::userdb());

	AMFacility blank("", "[Other Facility]", ":/128x128/contents.png");
	AMDatabaseDefinition::registerType(&blank, AMDatabase::userdb());
	blank.storeToDb(AMDatabase::userdb());
	AMFacility sgm("SGM", "Canadian Light Source SGM Beamline", ":/clsIcon.png");
	sgm.storeToDb(AMDatabase::userdb());
	AMFacility als801("8.0.1", "Advanced Light Source Beamline 8.0.1", ":/alsIcon.png");
	als801.storeToDb(AMDatabase::userdb());

}

/// Check whether the user database is the most recent version, and migrate if required.
void AMFirstTimeController::databaseUpgrade() {
	/// \todo
}
