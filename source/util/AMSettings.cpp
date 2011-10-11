/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "dataman/AMFileLoaderInterface.h"
#include "dataman/AMAnalysisBlockInterface.h"
#include <QDir>
#include <QPluginLoader>

#include <QStringBuilder>

/// User Settings:
// ========================================


/// 1. Database and storage:
// ========================================

/// Data storage root folder:
QString AMUserSettings::userDataFolder;
/// name of user database
QString AMUserSettings::userDatabaseFilename;

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
/// This is the location of the folder that contains the file loader plugins
QString AMSettings::fileLoaderPluginsFolder;
/// This is where the file loader plugins are located
QList<AMFileLoaderInterface*> AMSettings::availableFileLoaders;
/// This is the location of the folder that contains the analysis block plugins
QString AMSettings::analysisBlockPluginsFolder;
/// This is where the analysis block plugins are located
QList<AMAnalysisBlockInterface*> AMSettings::availableAnalysisBlocks;


/// Load settings from disk:
void AMSettings::load() {
	// changing to NativeFormat; IniFormat is broken on Mac OS X Lion for SystemScope.  Filed Qt Bug: https://bugreports.qt.nokia.com/browse/QTBUG-21744
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat, QSettings::SystemScope, "Acquaman", "Acquaman");
#else
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "Acquaman");
#endif

	// All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
	// Don't forget to add here if you add new user options.

	// variable = settings.value(key, defaultValue).toType();

	publicDataFolder = settings.value("publicDataFolder", "/home/acquaman/data/").toString();
	publicDatabaseFilename = settings.value("publicDatabaseFilename", "publicdata.db").toString();

#ifdef Q_WS_MAC
	QString defaultBasePath(QDir::homePath() % "/dev");
#else
	QString defaultBasePath(QDir::homePath() % "/beamline/programming");
#endif
	fileLoaderPluginsFolder = settings.value("fileLoaderPluginsFolder", QString(defaultBasePath % "/acquaman/plugins/FileLoaders")).toString();
	analysisBlockPluginsFolder = settings.value("analysisBlockPluginsFolder", QString(defaultBasePath % "/acquaman/plugins/AnalysisBlocks")).toString();

	// move this out of here? Might not want to do automatically every time settings are re-loaded.
	loadApplicationPlugins();
}

/// Save settings to disk:
void AMSettings::save() {
	// changing to NativeFormat; IniFormat is broken on Mac OS X Lion for SystemScope.  Filed Qt Bug: https://bugreports.qt.nokia.com/browse/QTBUG-21744
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat, QSettings::SystemScope, "Acquaman", "Acquaman");
#else
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "Acquaman");
#endif

	// All settings variables are saved here to the user-specific file.
	// Don't forget to add here if you add new user options.

	settings.setValue("publicDataFolder", publicDataFolder);
	settings.setValue("publicDatabaseFilename", publicDatabaseFilename);
	settings.setValue("fileLoaderPluginsFolder", fileLoaderPluginsFolder);
	settings.setValue("analysisBlockPluginsFolder", analysisBlockPluginsFolder);
}


void AMSettings::loadApplicationPlugins() {

	// Load file loader plugins
	AMSettings::availableFileLoaders.clear();
	QDir fileLoaderPluginsDirectory(AMSettings::fileLoaderPluginsFolder);
	foreach (QString fileName, fileLoaderPluginsDirectory.entryList(QDir::Files)) {
		QPluginLoader pluginLoader(fileLoaderPluginsDirectory.absoluteFilePath(fileName));
		QObject *plugin = pluginLoader.instance();
		if (plugin) {
			AMFileLoaderInterface *tmpfl = qobject_cast<AMFileLoaderInterface *>(plugin);
			if (tmpfl){
				AMSettings::availableFileLoaders.append(tmpfl);
				qDebug() << "Found a file loader";
			}
		}
	}

	// Load analysis block plugins
	AMSettings::availableAnalysisBlocks.clear();
	QDir analysisBlockPluginsDirectory(AMSettings::analysisBlockPluginsFolder);
	foreach (QString fileName, analysisBlockPluginsDirectory.entryList(QDir::Files)) {
		QPluginLoader pluginLoader(analysisBlockPluginsDirectory.absoluteFilePath(fileName));
		QObject *plugin = pluginLoader.instance();
		if (plugin) {
			AMAnalysisBlockInterface *tmpab = qobject_cast<AMAnalysisBlockInterface *>(plugin);
			if (tmpab){
				availableAnalysisBlocks.append(tmpab);
				qDebug() << "Found an analysis block";
			}
		}
	}
}
