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


#ifndef ACQMAN_SETTINGS_H
#define ACQMAN_SETTINGS_H


#include <QSettings>
#include <QString>

/*! This file encapsulates user settings/options and beamline settings that are persistent over many runs of the program.
   User-specific settings are stored under the AMUserSettings class.  They can be different for each user account.

   Program-wide settings are stored under the AMSettings class.
	(We store process variable name definitions separately in their own namespace, inside beamline/AMProcessVariables.h)

	AMSettings are loaded from config. files by calling
   load(), and pushed out to the file with save().  Load() is normally called on program startup, and save() when the AMSettings view gets hit with OK or Apply (todo... that AMSettings view).

   Accessing an option variable is simple and easy:
		AMUserAMSettings::userDataFolder or AMSettings::publicDatabaseFilename or PVNames::ringCurrent

   However, that convenience is balanced out by the awkwardness of setting up these settings...
   They need to be written down in four places:
	- Header (class) definition
	- actual variable declaration (cpp)
	- inside save()
	- inside load()

   For user settings, only two basic pieces of information are stored in the config file: the folder to store the user's data and database, and the name of their database file.  Once we have this information, we can access the user's personal meta-data object, which is stored in the database itself. (see AMUser::user() for more information.)

	Storage locations: on Mac OS X, storage is at:
		- /Library/Preferences/Qt/Acquaman/Acquaman.ini for system-wide stuff
		- ~/.config/Acquaman/Acquaman.ini for user-specific

	On linux, storage is at:
		- /etc/xdg/Acquaman/Acquaman.ini (system-wide)
		- /home/user/.config/Acquaman/Acquaman.ini (user-specific)

	\todo Problem: on linux (Ubuntu 9), don't have permission to access this folder. Results not getting saved.



   */

class AMUserSettings {
public:
	/// 1. Database and storage:
	// ========================================

	/// Data storage root folder:
	static QString userDataFolder;
	/// name of user database
	static QString userDatabaseFilename;

	/// Generates a default file path and file name (without an extension) within the user data storage folder. You can probably trust this to be unique. It will also ensure that the complete path (folders and subfolders) exists all the way down to the destination.
	static QString defaultFilePath(const QDateTime&);

	/// 2. User Information: (MOVED to AMUser)
	// ========================================


	/// Load settings from disk:
	static void load() ;

	/// Save settings to disk:
	static void save();

};

class AMSettings {
public:
	/// 1. Process Variable name definitions should go into beamline/PVNames.h instead...
	// ========================================


	/// 2. public database and storage:
	// ========================================

	/// This is where public (archived/reviewed) data is stored, system-wide
	static QString publicDataFolder;
	/// This is the public database filename:
	static QString publicDatabaseFilename;




	/// Load settings from disk:
	static void load();

	/// Save settings to disk:
	static void save();

};





#endif // SETTINGS_H
