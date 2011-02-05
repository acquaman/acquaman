/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
	path.append(dt.toString("/ddd_MM_dd_hh_mm_ss"));
	path.append(QString("_%1").arg(int(qrand()%10000), 4, 10, QChar('0')));
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
