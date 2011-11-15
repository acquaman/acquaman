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

void AMPluginsManager::loadApplicationPlugins() {

	QWriteLocker wl(&mutex_);

	// Load file loader plugins
	fileFormats2fileLoaderFactories_.clear();

	QDir fileLoaderPluginsDirectory(AMSettings::s()->fileLoaderPluginsFolder());
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
	}

	// Load analysis block plugins
	availableAnalysisBlocks_.clear();

	QDir analysisBlockPluginsDirectory(AMSettings::s()->analysisBlockPluginsFolder());
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
}
