#include "AMFirstTimeController.h"
#include <QStringList>

bool AMFirstTimeController::isFirstTime() {

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

	return isFirstTime;
}


#include <QApplication>
#include "dataman/AMUser.h"

bool AMFirstTimeController::onFirstTime() {

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
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, 0, "Creating new user data folder: "  + AMUserSettings::userDataFolder));
		if(!userDataDir.mkpath(AMUserSettings::userDataFolder)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, 0, "Could not create user data folder " + AMUserSettings::userDataFolder));
			return false;
		}
	}

	/// Find out the user's name:
	AMUser::user()->setName( ftw.field("userName").toString() );

	// initialize the database
	QString filename = AMUserSettings::userDataFolder + AMUserSettings::userDatabaseFilename;
	QFile dbFile(filename);
	if(!dbFile.exists())
		return databaseInitialization();
	else
		return true;

}

bool AMFirstTimeController::onEveryTime() {

	/// Upgrade database versions if required.
	bool success = databaseUpgrade();

	/// Load extended user settings from the database
	AMUser::user()->loadFromDb(AMDatabase::userdb(), 1);

	return success;
}

#include <dataman/AMDatabaseDefinition.h>
#include <dataman/AMXASScan.h>
#include <dataman/AMRun.h>
#include <dataman/AMSample.h>
#include <dataman/AMExperiment.h>
#include <dataman/AMControlSetInfo.h>
#include <dataman/AMSamplePlate.h>
#include <dataman/AMDetectorInfo.h>
#include "dataman/AMUser.h"

/// create structures and tables for a new user database, from scratch
bool AMFirstTimeController::databaseInitialization() {

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

	AMControlSetInfo csi;
	AMDatabaseDefinition::registerType(&csi, AMDatabase::userdb());

	AMSamplePlate sp;
	AMDatabaseDefinition::registerType(&sp, AMDatabase::userdb());
	AMDatabase::userdb()->createIndex(AMDatabaseDefinition::samplePlateTableName(), "createTime");

	AMDatabaseDefinition::registerType(AMUser::user(), AMDatabase::userdb());
	AMUser::user()->storeToDb(AMDatabase::userdb());

	AMFacility blank("", "[Other Facility]", ":/128x128/contents.png");
	AMDatabaseDefinition::registerType(&blank, AMDatabase::userdb());
	blank.storeToDb(AMDatabase::userdb());
	AMFacility als801("8.0.1", "Advanced Light Source Beamline 8.0.1", ":/alsIcon.png");
	als801.storeToDb(AMDatabase::userdb());
	AMFacility sgm("SGM", "Canadian Light Source SGM Beamline", ":/clsIcon.png");
	sgm.storeToDb(AMDatabase::userdb());

	/// Usability tweak: the first time users launch the program, we can assume they're going to do some acquisition, so let's create an initial run. (\todo This is SGM-specific... maybe it should be moved elsewhere)
	AMRun newRun("SGM", sgm.id());
	newRun.storeToDb(AMDatabase::userdb());

	/// \todo Better error checking. Complicated because some calls could fail even though the process completes successfully. (ie: creating db table columns that already exist will fail)
	return true;



}

/// Check whether the user database is the most recent version, and migrate if required.
bool AMFirstTimeController::databaseUpgrade() {
	/// \todo
	return true;
}
