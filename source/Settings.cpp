#include "Settings.h"

#include <QDir>
// Needed for getenv():
#include <cstdlib>

#include <QDebug>
/// User Settings:
// ========================================


/// 1. Database and storage:
// ========================================

/// Data storage root folder:
QString UserSettings::userDataFolder;
/// name of user database
QString UserSettings::userDatabaseFilename;

/// 2. User Information:
// ========================================

/// User name:
QString UserSettings::userName;



/// Load settings from disk:
void UserSettings::load() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");

    // All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
    // Don't forget to add here if you add new user options.

    // variable = settings.value(key, defaultValue).toType();

	userDataFolder = settings.value("userDataFolder", QDir::homePath() + "/acquamanData/").toString();
	// TODO: warn here if doesn't exist?
    userDatabaseFilename = settings.value("userDatabaseFilename", "userdata.db").toString();

	userName = settings.value("userName", getenv("USER") ).toString();
}

/// Save settings to disk:
void UserSettings::save() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");

    // All settings variables are saved here to the user-specific file.
    // Don't forget to add here if you add new user options.

    settings.setValue("userDataFolder", userDataFolder);
    settings.setValue("userDatabaseFilename", userDatabaseFilename);

    settings.setValue("userName", userName);
}




/// Program Settings
// =========================================

/// 2. public database and storage:
// ========================================

/// This is where public (archived/reviewed) data is stored, system-wide
QString Settings::publicDataFolder;
/// This is the public database filename:
QString Settings::publicDatabaseFilename;
/// Root database table:
QString Settings::dbObjectTableName;

/// Load settings from disk:
void Settings::load() {
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "Acquaman");

    // All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
    // Don't forget to add here if you add new user options.

    // variable = settings.value(key, defaultValue).toType();

    publicDataFolder = settings.value("publicDataFolder", "/home/acquaman/data/").toString();
    publicDatabaseFilename = settings.value("publicDatabaseFilename", "publicdata.db").toString();
	dbObjectTableName = settings.value("dbObjectTableName", "dbObjectTable").toString();
}

/// Save settings to disk:
void Settings::save() {
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "Acquaman");

    // All settings variables are saved here to the user-specific file.
    // Don't forget to add here if you add new user options.

    settings.setValue("publicDataFolder", publicDataFolder);
    settings.setValue("publicDatabaseFilename", publicDatabaseFilename);
	settings.setValue("dbObjectTableName", dbObjectTableName);
}
