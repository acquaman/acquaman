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

#ifndef AMFILELOADERINTERFACE_H
#define AMFILELOADERINTERFACE_H

#include <QtPlugin>
#include <QStringList>

class AMScan;
class AMErrorMon;

/// File loaders in Acquaman use a plugin system. This is the base class of all object which actually implement file loading.
/*! To create a file loader plugin, you need to do 2 things:

  - 1) Create a regular class that inherits AMFileLoaderInterface. This is the actual file loader, which will receive an AMScan and load data into it. One instance of it will be created and used whenever a load is necessary.

  Your subclass must implement the acceptedFileFormats() and accepts() functions to indicate what types of scans it can handle.  It also needs to have a load() function which actually implements loading the data into the scan provided, based on the scan's fileFormat() and filePath() (and whatever else you want to consider).

  \note Threading: This class should be re-entrant, but doesn't need to be thread-safe.  (This means it can have member variables, but shouldn't access static variables or other shared resources without thread protection.)

  - 2) Create a "factory" class that inherits public QObject and public AMFileLoaderFactory. This is the plugin class, which will be loaded once and used to create file loaders as required. Inside the class declaration, declare it as a plugin interface with "Q_INTERFACES(AMFileLoaderFactory)".  Make sure to include the Q_OBJECT macro as well.

 This subclass must also implement the acceptedFileFormats() and accepts() functions to indicate what types of scans it can handle.  It also needs to have a createFileLoader() function that creates and returns an instance of the first object, which will actually be used for loading.

\note Threading: This class should be thread-safe.  This is easiest to do if you simply avoid having member variables, and contain all your logic in local function variables.

  - 3) At the bottom of your .cpp implementation, include the plugin export macro for the factory class:
  Q_EXPORT_PLUGIN2(YourSubclassFactory, YourSubclassFactory)

  */
class AMFileLoaderInterface {
public:
	virtual ~AMFileLoaderInterface() {}

	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle (ex: "sgm2004"). You should have at least one here, otherwise your plugin will never be used.  It's fine to accept multiple file formats.
	virtual QStringList acceptedFileFormats() = 0;
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan) = 0;

	/// This function loads data into \c scan, by looking at the scan's AMScan::fileFormat(), AMScan::filePath(), and anything else you want to consider. The AMScan::filePath() is considered relative to \c userDataFolder.
	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor) = 0;
};

/// File loaders in Acquaman use a plugin system. This is the base class of all dynamically-loaded plugins which can create file loaders.
/*! For details, see the documentation in AMFileLoaderInterface.*/
class AMFileLoaderFactory
{
public:
	virtual ~AMFileLoaderFactory() {}

	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle (ex: "sgm2004"). You should have at least one here, otherwise your plugin will never be used.  It's fine to accept multiple file formats.
	virtual QStringList acceptedFileFormats() = 0;
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan) = 0;

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() = 0;
};



QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(AMFileLoaderFactory,
			"AMFileLoaderFactory/1.0");
QT_END_NAMESPACE

#endif // AMFILELOADERINTERFACE_H
