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


#ifndef AM_PVNAMES_H
#define AM_PVNAMES_H

#include "util/AMErrorMonitor.h"

#include <QSettings>
#include <QString>

#include "util/AMBiHash.h"

/// This class is not currently used by anyone. If desired, it can be used to provide a map between meaningful names and PV names, which are stored (and editable) in a config file on disk.
/*! This class provides definitions for all Process Variable names that we use/reference.  They are read from a configuration file on program startup,
    but defaults are provided here in case the file is missing/not created yet.  AMPVNames can also be stored from memory to disk with save(), but this
	should rarely be used. (Maybe by the developers for creating the file, but that's it.)
    */

class AMPVNames {
public:
    /// 1. Process Variable name definitions.
    // ========================================

	/// lookup a PV name based on its meaningful name. Returns a null and empty QString if not found.
	static QString toPV(const QString& meaningfulName) {
		QString retval = d_.valueF(meaningfulName);
		if(retval.isEmpty())
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "AMPVNames: I couldn't find the PV name you were looking for."));
		return retval;
	}

	/// reverse-lookup; a meaningful name from a PV. Returns a null and empty QString if not found.
	static QString fromPV(const QString& pvName) {
		QString retval = d_.valueR(pvName);
		if(retval.isEmpty())
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "AMPVNames: I couldn't find a name for that PV."));
		return retval;
	}



    /// Load settings from disk:
    static void load();

    /// Save settings to disk:
    static void save();

protected:
	/// Provides lookup from a meaningful name to a PV name:
	static AMBiHash<QString, QString> d_;

};

#endif // AM_PVNAMES_H
