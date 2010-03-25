#ifndef SETTINGS_H
#define SETTINGS_H

#ifndef GLOBALOPTIONS_H
#define GLOBALOPTIONS_H

#include <QSettings>
#include <QString>

/*! This file encapsulates user settings/options and beamline settings that are persistent over many runs of the program.
   User-specific settings are stored under the UserSettings namespace.  They can be different for each user account.

   Program-wide settings are stored under the Settings namespace.
    For clean organization, we store process variable name definitions separately in their own namespace, inside beamline/ProcessVariables.h

    Settings are loaded from config. files by calling
   load(), and pushed out to the file with save().  Load() is normally called from main() before
   the program starts running, and save() when the Settings view gets hit with OK or Apply (todo... that Settings view).

   Accessing an option variable is simple and easy:
        UserSettings::userDataFolder or Settings::publicDatabaseFilename or PVNames::ringCurrent

   However, that convenience is balanced out by the awkwardness of setting up these settings...
   They need to be written down in four places:
    - Header (class) definition
    - actual variable declaration (cpp)
    - inside save()
    - inside load()

   TODO: Should we make these QObject properties, and then we don't have to add lines to save() and load() manually when adding new variables?

    Storage locations: on Mac OS X, storage is at:
        /Library/Preferences/Qt/mbdc/Acquaman.ini for system-wide stuff
        ~/.config/mbdc/Acquaman.ini for user-specific

   */

class UserSettings {
public:
    /// 1. Database and storage:
    // ========================================

    /// Data storage root folder:
    static QString userDataFolder;
    /// name of user database
    static QString userDatabaseFilename;

    /// 2. User Information:
    // ========================================

    /// User name:
    static QString userName;


    /// Load settings from disk:
    static void load() ;

    /// Save settings to disk:
    static void save();

};

class Settings {
public:
    /// 1. Process Variable name definitions should go into beamline/PVNames.h instead...
    // ========================================


    /// 2. public database and storage:
    // ========================================

    /// This is where public (archived/reviewed) data is stored, system-wide
    static QString publicDataFolder;
    /// This is the public database filename:
    static QString publicDatabaseFilename;

	/// This is the name of the root database table that will store all objects
	static QString dbObjectTableName;



    /// Load settings from disk:
    static void load();

    /// Save settings to disk:
    static void save();

};



#endif // GLOBALOPTIONS_H


#endif // SETTINGS_H
