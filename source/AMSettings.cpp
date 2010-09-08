#include "AMSettings.h"

#include <QDir>
#include <QDateTime>

#include <QDebug>
#include <QDateTime>

/// User Settings:
// ========================================


/// 1. Database and storage:
// ========================================

/// Data storage root folder:
QString AMUserSettings::userDataFolder;
/// name of user database
QString AMUserSettings::userDatabaseFilename;


QString AMUserSettings::defaultFilePath(const QDateTime& dt) {
	QDir dir;
	QString path = userDataFolder + dt.toString("/yyyy/MM");
	dir.mkpath(path);
	path.append(dt.toString("/ddd_MM_dd_hh_mm_ss_zzzz"));
	return path;
}

/// 2. User Information: (MOVED to AMUser)
// ========================================


/// Load settings from disk:
void AMUserSettings::load() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");

	// All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
	// Don't forget to add here if you add new user options.

	// variable = settings.value(key, defaultValue).toType();

	userDataFolder = settings.value("userDataFolder", QDir::homePath() + "/beamline/programming/acquaman/devUserData/").toString();
	userDatabaseFilename = settings.value("userDatabaseFilename", "userdata.db").toString();

}

/// Save settings to disk:
void AMUserSettings::save() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");

	// All settings variables are saved here to the user-specific file.
	// Don't forget to add here if you add new user options.

	settings.setValue("userDataFolder", userDataFolder);
	settings.setValue("userDatabaseFilename", userDatabaseFilename);

	// settings.setValue("userName", userName);
}




/// Program Settings
// =========================================

/// 2. public database and storage:
// ========================================

/// This is where public (archived/reviewed) data is stored, system-wide
QString AMSettings::publicDataFolder;
/// This is the public database filename:
QString AMSettings::publicDatabaseFilename;


/// Load settings from disk:
void AMSettings::load() {
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "Acquaman");

	// All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
	// Don't forget to add here if you add new user options.

	// variable = settings.value(key, defaultValue).toType();

	publicDataFolder = settings.value("publicDataFolder", "/home/acquaman/data/").toString();
	publicDatabaseFilename = settings.value("publicDatabaseFilename", "publicdata.db").toString();

}

/// Save settings to disk:
void AMSettings::save() {
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "Acquaman");

	// All settings variables are saved here to the user-specific file.
	// Don't forget to add here if you add new user options.

	settings.setValue("publicDataFolder", publicDataFolder);
	settings.setValue("publicDatabaseFilename", publicDatabaseFilename);
}
