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


#ifndef AM_SETTINGS_H
#define AM_SETTINGS_H


#include <QSettings>
#include <QString>

/// This class encapsulates user settings/options that are persistent over many runs of the program.  Now that we have a database system for persistent data, only the essential information required to get to the database is stored in here. (See AMUser for the rest of the user's settings.)
/*!
   User-specific settings are stored under the AMUserSettings class.  They can be different for each user account.


	AMSettings are loaded from config. files by calling
   load(), and pushed out to the file with save().  Load() is normally called on program startup, and save() when the AMSettings view gets hit with OK or Apply (\todo... Make an AMSettings view).

   Accessing an option variable is simple and easy:
		AMUserAMSettings::userDataFolder or AMSettings::s()->publicDatabaseFilename(), etc.


   For user settings, only two basic pieces of information are stored in the config file: the folder to store the user's data and database, and the name of their database file.  Once we have this information, we can access the user's personal meta-data object, which is stored in the database itself. (see AMUser::user() for more information.)

	Storage locations: on Mac OS X, storage is at:
		- /Library/Preferences/Qt/Acquaman/Acquaman.ini for system-wide stuff
		- ~/.config/Acquaman/Acquaman.ini for user-specific

	On linux, storage is at:
		- /etc/xdg/Acquaman/Acquaman.ini (system-wide)
		- /home/user/.config/Acquaman/Acquaman.ini (user-specific)
   */

class AMUserSettings {
public:
	/// 1. Database and storage:
	// ========================================

	/// Flag to identify user-account based storage
	static bool userBasedDataStorage;
	/// Backup location to copy the data to
	static QString remoteDataFolder;
	/// Data storage root folder:
	static QString userDataFolder;
	/// name of user database
	static QString userDatabaseFilename;
	/// report startup errors for reading the settings file
	static bool userSettingsStartupError;

	/// Generates a default file path and file name (without an extension) within the user data storage folder. You can  trust this to be unique. It will also ensure that the complete path (folders and subfolders) exists all the way down to the destination.  \note This version provides an absolute path name, starting at the root of the filesystem.
	static QString defaultAbsolutePathForScan(const QDateTime&);

	/// Generates a default file path and file name (without an extension) within the user data storage folder. You can  trust this to be unique. It will also ensure that the complete path (folders and subfolders) exists all the way down to the destination.  \note This version provides a relative path name, relative to the user's \c userDataFolder.
	static QString defaultRelativePathForScan(const QDateTime&);

	/// Takes an absolute file path, and if it can be expressed relative to the userDataFolder, returns it as that relative path. (Example: /Users/mboots/acquamanUserData/2010/03/foo.txt becomes 2010/03/foo.txt, if my userDataFolder is /User/mboots/acquamanUserData).  If provided, \c wasInUserDataFolder is set to true if \c absolutePath could be expressed within the userDataFolder, and false if it was outside of that.
	static QString relativePathFromUserDataFolder(const QString& absolutePath, bool* wasInUserDataFolder = 0);

        /// Returns the absolute export folder path based on the current data folder location.
        static QString defaultAbsoluteExportFolder();

	/// Removes the remote data folder entry.
	static void removeRemoteDataFolderEntry();

	/// Load settings from disk:
	static void load() ;

	/// Save settings to disk:  This will not update the remote data folder if it is empty.  But there are times when you need to override that behaviour.
	static void save(bool forceRemoteDataFolderUpdate = false);

};


#include <QReadWriteLock>
#include <QMutex>
#include <QMultiMap>

/// This singleton class encapsulates application-wide settings and options that are persistent over many runs of the program. Access it using AMSettings::s() to get the instance. This class is thread-safe.
class AMSettings {
public:

	/// AMSettings is a singleton class. Access the only instance of it using AMSettings::s().
	static AMSettings* s();


	/// 1. public database and storage:
	// ========================================

	/// This is where public (archived/reviewed) data is stored, system-wide
	QString publicDataFolder() const;
	void setPublicDataFolder(QString publicDataFolder);
	/// This is the public database filename:
	QString publicDatabaseFilename() const;
	void setPublicDatabaseFilename(QString publicDatabaseFilename);

	/// This is the location of the folder that contains the file loader plugins
	QString fileLoaderPluginsFolder() const;
	void setFileLoaderPluginsFolder(QString fileLoaderPluginsFolder);

	/// This is the location of the folder that contains the analysis block plugins
	QString analysisBlockPluginsFolder() const;
	void setAnalysisBlockPluginsFolder(QString analysisBlockPluginsFolder);

	/// Load settings from disk persistent storage:
	void load();
	/// Save settings to disk persistent storage:
	void save();


protected:
	/// This is a singleton class, so the constructor is protected.
	virtual ~AMSettings();
	AMSettings() : mutex_(QReadWriteLock::Recursive) {}

	QString publicDataFolder_;
	QString publicDatabaseFilename_;

	QString fileLoaderPluginsFolder_;
	QString analysisBlockPluginsFolder_;

	// thread safety
	mutable QReadWriteLock mutex_;

	// singleton instance
	static AMSettings* instance_;
	static QMutex instanceMutex_;

};





#endif // SETTINGS_H
