#ifndef AMPLUGINSMANAGER_H
#define AMPLUGINSMANAGER_H

#include <QString>
#include <QMultiMap>
#include <QList>
#include <QReadWriteLock>
#include <QMutex>

class AMFileLoaderFactory;
class AMAnalysisBlockInterface;

/// This class manages the loading of dynamic plugins (file loaders and analysis blocks) for Acquaman applications. You can cause plugins to be reloaded by calling AMPluginsManager::s()->loadApplicationPlugins(), and access the registry of current plugins with availableFileLoaders() and availableAnalysisBlocks().  This class is thread-safe.
class AMPluginsManager {
public:

	/// AMPluginsManager is a singleton class. Access the only instance of it using AMPluginsManager::s().
	static AMPluginsManager* s();

	/// This returns a registry of the available AMFileLoaderFactory, indexed by the file formats they can handle.
	QMultiMap<QString, AMFileLoaderFactory*> availableFileLoaderPlugins() const;

	/// This returns a list of the available analysis block plugins (Warning: these are currently unused)
	QList<AMAnalysisBlockInterface*> availableAnalysisBlocks() const;

	/// Calling this will clear the existing plugins and reload all that we find on the disk, in AMSettings::fileLoaderPluginsFolder() and AMSettings::analysisBlockPluginsFolder().
	void loadApplicationPlugins();

protected:
	/// This is a singleton class, so the constructor is protected.
	AMPluginsManager() : mutex_(QReadWriteLock::Recursive) {}

	/// registry of available file formats, indexed by file format
	QMultiMap<QString, AMFileLoaderFactory*> fileFormats2fileLoaderFactories_;
	/// list of available analysis blocks
	QList<AMAnalysisBlockInterface*> availableAnalysisBlocks_;

	// thread safety:
	mutable QReadWriteLock mutex_;

	// singleton instance
	static AMPluginsManager* instance_;
	static QMutex instanceMutex_;

};

#endif // AMPLUGINSMANAGER_H
