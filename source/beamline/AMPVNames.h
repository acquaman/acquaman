#ifndef ACQMAN_PVNAMES_H
#define ACQMAN_PVNAMES_H

#include <QSettings>
#include <QString>

/*! This namespace provides definitions for all Process Variable names that we use/reference.  They are read from a configuration file on program startup,
    but defaults are provided here in case the file is missing/not created yet.  AMPVNames can also be stored from memory to disk with save(), but this
    should rarely be used. (Maybe by the developers for creating the file, but that's it ; )

    When adding PVs to this list, don't forget to add the actual declaration to AMPVNames.cpp, and add lines in save() and load() as well.

    */

class AMPVNames {
public:
    /// 1. Process Variable name definitions.
    // ========================================
    /// Ring current
    static QString ringCurrent;


    /// Load settings from disk:
    static void load();

    /// Save settings to disk:
    static void save();
};

#endif // ACQMAN_PVNAMES_H
