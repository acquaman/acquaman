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

	/// Calling this will clear the existing plugins and reload all that we find on the disk, in fileLoaderFolder and analysisBlockFolder.
	void loadApplicationPlugins(const QString &fileLoaderFolder, const QString &analysisBlocksFolder);

protected:
	/// This is a singleton class, so the constructor is protected.
 	virtual ~AMPluginsManager();
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
