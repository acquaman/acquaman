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


#include "AMSettings.h"

#include <QCoreApplication>
#include <QDir>
#include <QDateTime>
#include <QStringBuilder>
#include <QDesktopServices>

#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>

#include "util/AMErrorMonitor.h"

/// User Settings:
// ========================================


/// 1. Database and storage:
// ========================================

/// User-account based data storage
bool AMUserSettings::userBasedDataStorage;
/// Data storage backup folder:
QString AMUserSettings::remoteDataFolder;
/// Data storage root folder:
QString AMUserSettings::userDataFolder;
/// name of user database
QString AMUserSettings::userDatabaseFilename;
/// report startup errors for reading the settings file
bool AMUserSettings::userSettingsStartupError;

QString AMUserSettings::defaultRelativePathForScan(const QDateTime& dt) {
	QDir dir;
	QString path = dt.toString("yyyy/MM");
	dir.mkpath(userDataFolder + "/" + path);
	path.append(dt.toString("/ddd_MM_dd_hh_mm_ss"));
	path.append(QString("_%1").arg(int(qrand()%10000), 4, 10, QChar('0')));
	return path;
}

QString AMUserSettings::defaultAbsolutePathForScan(const QDateTime& dt) {
	QDir dir;
	QString path = userDataFolder + dt.toString("/yyyy/MM");
	dir.mkpath(path);
	path.append(dt.toString("/ddd_MM_dd_hh_mm_ss"));
	path.append(QString("_%1").arg(int(qrand()%10000), 4, 10, QChar('0')));
	return path;
}

QString AMUserSettings::relativePathFromUserDataFolder(const QString &absolutePath, bool *wasInUserDataFolder)
{
	bool wasOK;
	QString rv;

	QString normalizedPath = QDir::fromNativeSeparators(absolutePath);
	QString normalizedUserDataFolder = QDir::fromNativeSeparators(userDataFolder);
	if(normalizedPath.startsWith(normalizedUserDataFolder)) {
		wasOK = true;
		rv = normalizedPath.mid(normalizedUserDataFolder.length());
	}
	else {
		wasOK = false;
		rv = absolutePath;
	}

	if(wasInUserDataFolder) {
		*wasInUserDataFolder = wasOK;
	}

	return rv;
}

QString AMUserSettings::defaultAbsoluteExportFolder()
{
	QDir userDataDirectory(userDataFolder);
	userDataDirectory.cdUp();

	//If the export folder exists (as it always should) then return the path to exportData.
	//Otherwise, have exports go to Desktop so the problem is obvious without compromising data based actions.
	if(userDataDirectory.cd("exportData"))
		return userDataDirectory.absolutePath();
	else
		return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
}


/// Load settings from disk:
void AMUserSettings::load() {
	userSettingsStartupError = false;
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");

	if(settings.status() != QSettings::NoError)
		userSettingsStartupError = true;

	// All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
	// Don't forget to add here if you add new user options.
	// variable = settings.value(key, defaultValue).toType()

	userBasedDataStorage = settings.value("userBasedDataStorage", true).toBool();
	remoteDataFolder = settings.value("remoteDataFolder", "").toString();
	userDataFolder = settings.value("userDataFolder", QDir::homePath() + "/beamline/programming/acquaman/devUserData/").toString();
	userDatabaseFilename = settings.value("userDatabaseFilename", "userdata.db").toString();

}

/// Save settings to disk:
void AMUserSettings::save(bool forceRemoteDataFolderUpdate)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");

	// All settings variables are saved here to the user-specific file.
	// Don't forget to add here if you add new user options.
	settings.setValue("userBasedDataStorage", userBasedDataStorage);

	if (forceRemoteDataFolderUpdate)
		settings.setValue("remoteDataFolder", remoteDataFolder);

	else if(!remoteDataFolder.isEmpty())
		settings.setValue("remoteDataFolder", remoteDataFolder);

	settings.setValue("userDataFolder", userDataFolder);
	settings.setValue("userDatabaseFilename", userDatabaseFilename);
}

void AMUserSettings::removeRemoteDataFolderEntry()
{
	remoteDataFolder = "";

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");
	settings.remove("remoteDataFolder");
}

/// Load settings from disk:
void AMSettings::load() {

	QWriteLocker wl(&mutex_);

	// changing to NativeFormat, IniFormat is broken on Mac OS X Lion for SystemScope.  Filed Qt Bug: https://bugreports.qt.nokia.com/browse/QTBUG-21744
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat, QSettings::SystemScope, "Acquaman", "Acquaman");
#else
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "Acquaman");
#endif

	// All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
	// Don't forget to add here if you add new user options.

	// variable = settings.value(key, defaultValue).toType()

	publicDataFolder_ = settings.value("publicDataFolder", "/home/acquaman/data/").toString();
	publicDatabaseFilename_ = settings.value("publicDatabaseFilename", "publicdata.db").toString();

	QDir applicationDir(QCoreApplication::applicationDirPath());
	if(!applicationDir.exists()) {
		AMErrorMon::alert(0, AM_SETTINGS_NONE_EXIST_APPLICATION_PATH, QString("The application path %1 doesn't exist!").arg(applicationDir.absolutePath()));
	} else {
		applicationDir.cdUp();

		QDir fileLoderPluginDir(applicationDir.absolutePath() % "/plugins/FileLoaders");
		if (!fileLoderPluginDir.exists()) {
			AMErrorMon::alert(0, AM_SETTINGS_NONE_EXIST_PLUGIN_PATH, QString("The plugin path %1 doesn't exist!").arg(fileLoderPluginDir.absolutePath()));
		}

		QDir analysisBlockPluginDir(applicationDir.absolutePath() % "/plugins/AnalysisBlocks");
		if (!analysisBlockPluginDir.exists()) {
			AMErrorMon::alert(0, AM_SETTINGS_NONE_EXIST_PLUGIN_PATH, QString("The plugin path %1 doesn't exist!").arg(analysisBlockPluginDir.absolutePath()));
		}

		fileLoaderPluginsFolder_ = settings.value("fileLoaderPluginsFolder", fileLoderPluginDir.absolutePath()).toString();
		analysisBlockPluginsFolder_ = settings.value("analysisBlockPluginsFolder", analysisBlockPluginDir.absolutePath()).toString();
	}
}

/// Save settings to disk:
void AMSettings::save() {

	QReadLocker rl(&mutex_);

	// changing to NativeFormat, IniFormat is broken on Mac OS X Lion for SystemScope.  Filed Qt Bug: https://bugreports.qt.nokia.com/browse/QTBUG-21744
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat, QSettings::SystemScope, "Acquaman", "Acquaman");
#else
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "Acquaman");
#endif

	// All settings variables are saved here to the system-wide file.
	// Don't forget to add here if you add new system-wide options.

	settings.setValue("publicDataFolder", publicDataFolder_);
	settings.setValue("publicDatabaseFilename", publicDatabaseFilename_);
	settings.setValue("fileLoaderPluginsFolder", fileLoaderPluginsFolder_);
	settings.setValue("analysisBlockPluginsFolder", analysisBlockPluginsFolder_);
}


// Singleton variables:
QMutex AMSettings::instanceMutex_(QMutex::Recursive);
AMSettings* AMSettings::instance_ = 0;

AMSettings * AMSettings::s()
{
	QMutexLocker ml(&instanceMutex_);

	if(!instance_) {
		instance_ = new AMSettings();
	}
	return instance_;
}

QString AMSettings::publicDataFolder() const
{
	QReadLocker rl(&mutex_);
	return publicDataFolder_;
}

void AMSettings::setPublicDataFolder(QString publicDataFolder)
{
	QWriteLocker wl(&mutex_);
	publicDataFolder_ = publicDataFolder;
}

QString AMSettings::publicDatabaseFilename() const
{
	QReadLocker rl(&mutex_);
	return publicDatabaseFilename_;
}

void AMSettings::setPublicDatabaseFilename(QString publicDatabaseFilename)
{
	QWriteLocker wl(&mutex_);
	publicDatabaseFilename_ = publicDatabaseFilename;
}

QString AMSettings::fileLoaderPluginsFolder() const
{
	QReadLocker rl(&mutex_);
	return fileLoaderPluginsFolder_;
}

void AMSettings::setFileLoaderPluginsFolder(QString fileLoaderPluginsFolder)
{
	QWriteLocker wl(&mutex_);
	fileLoaderPluginsFolder_ = fileLoaderPluginsFolder;
}

QString AMSettings::analysisBlockPluginsFolder() const
{
	QReadLocker rl(&mutex_);
	return analysisBlockPluginsFolder_;
}

void AMSettings::setAnalysisBlockPluginsFolder(QString analysisBlockPluginsFolder)
{
	QWriteLocker wl(&mutex_);
	analysisBlockPluginsFolder_ = analysisBlockPluginsFolder;
}





 AMSettings::~AMSettings(){}
