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


#include "AMPluginsManager.h"

#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>
#include <QDir>
#include <QPluginLoader>

#include "dataman/AMFileLoaderInterface.h"
#include "dataman/AMAnalysisBlockInterface.h"
#include "util/AMSettings.h"

// Singleton variables:
QMutex AMPluginsManager::instanceMutex_(QMutex::Recursive);
AMPluginsManager* AMPluginsManager::instance_ = 0;

AMPluginsManager * AMPluginsManager::s()
{
	QMutexLocker ml(&instanceMutex_);

	if(!instance_) {
		instance_ = new AMPluginsManager();
	}
	return instance_;
}

QMultiMap<QString, AMFileLoaderFactory *> AMPluginsManager::availableFileLoaderPlugins() const
{
	QReadLocker rl(&mutex_);
	return fileFormats2fileLoaderFactories_;
}

QList<AMAnalysisBlockInterface *> AMPluginsManager::availableAnalysisBlocks() const
{
	QReadLocker rl(&mutex_);
	return availableAnalysisBlocks_;
}

#include "util/AMErrorMonitor.h"

void AMPluginsManager::loadApplicationPlugins(){
	loadApplicationPlugins(AMSettings::s()->fileLoaderPluginsFolder(), AMSettings::s()->analysisBlockPluginsFolder());
}

void AMPluginsManager::loadApplicationPlugins(const QString &fileLoaderFolder, const QString &analysisBlocksFolder) {
	Q_UNUSED(analysisBlocksFolder) //Darren removed these a while ago

	QWriteLocker wl(&mutex_);


	// Load file loader plugins
	fileFormats2fileLoaderFactories_.clear();


	QDir fileLoaderPluginsDirectory(fileLoaderFolder);
	foreach (QString fileName, fileLoaderPluginsDirectory.entryList(QDir::Files)) {
		QPluginLoader pluginLoader(fileLoaderPluginsDirectory.absoluteFilePath(fileName));
		QObject *plugin = pluginLoader.instance();
		if(plugin) {
			AMFileLoaderFactory *factory = qobject_cast<AMFileLoaderFactory *>(plugin);
			if(factory) {
				foreach(QString fileFormat, factory->acceptedFileFormats()) {
					fileFormats2fileLoaderFactories_.insert(fileFormat, factory);
				}
			}
		}
		else {
			AMErrorMon::report(AMErrorReport(0, 
							AMErrorReport::Debug, 
							-417,
							QString("AMPluginsManager: There was a problem trying to load the plugin '%1'. The plugin system report the error: %2").arg(fileName).arg(pluginLoader.errorString())));
		}
	}

	// Load analysis block plugins
	availableAnalysisBlocks_.clear();

	/* Not Building these at the current time
	QDir analysisBlockPluginsDirectory(analysisBlocksFolder);
	foreach (QString fileName, analysisBlockPluginsDirectory.entryList(QDir::Files)) {
		QPluginLoader pluginLoader(analysisBlockPluginsDirectory.absoluteFilePath(fileName));
		QObject *plugin = pluginLoader.instance();
		if (plugin) {
			AMAnalysisBlockInterface *analysisBlock = qobject_cast<AMAnalysisBlockInterface *>(plugin);
			if (analysisBlock){
				availableAnalysisBlocks_.append(analysisBlock);
			}
		}
	}
	*/
}
 AMPluginsManager::~AMPluginsManager(){}
