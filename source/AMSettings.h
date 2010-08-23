#ifndef ACQMAN_SETTINGS_H
#define ACQMAN_SETTINGS_H


#include <QSettings>
#include <QString>

/*! This file encapsulates user settings/options and beamline settings that are persistent over many runs of the program.
   User-specific settings are stored under the AMUserAMSettings namespace.  They can be different for each user account.

   Program-wide settings are stored under the AMSettings namespace.
	For clean organization, we store process variable name definitions separately in their own namespace, inside beamline/ProcessVariables.h

	AMSettings are loaded from config. files by calling
   load(), and pushed out to the file with save().  Load() is normally called from main() before
   the program starts running, and save() when the AMSettings view gets hit with OK or Apply (todo... that AMSettings view).

   Accessing an option variable is simple and easy:
		AMUserAMSettings::userDataFolder or AMSettings::publicDatabaseFilename or PVNames::ringCurrent

   However, that convenience is balanced out by the awkwardness of setting up these settings...
   They need to be written down in four places:
	- Header (class) definition
	- actual variable declaration (cpp)
	- inside save()
	- inside load()

   TODO: Should we make these QObject properties, and then we don't have to add lines to save() and load() manually when adding new variables?

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

	/// 2. User Information:
	// ========================================

	/// User name:
	static QString userName;


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
