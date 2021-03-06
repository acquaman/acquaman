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


#ifndef VESPERSDBUPGRADE1PT2_H
#define VESPERSDBUPGRADE1PT2_H

#include "dataman/database/AMDbUpgrade.h"

#define VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_2DCONFIGURATION 389100
#define VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_LINECONFIGURATION 389101
#define VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_EXAFSCONFIGURATION 389102

/// This class is a very simple database upgrade where the column name for fluorescenceDetectorChoice is renamed to fluorescenceDetector.
/*!
	Although the actual change is quite minor, the process is somewhat involved because SQLite doesn't have the entire SQL language
	options.  The other reason for this upgrade is for practice changing column names since the next database upgrade
	changes the contents of the columns as well as the name of the column.
  */
class VESPERSDbUpgrade1Pt2 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
 	virtual ~VESPERSDbUpgrade1Pt2();
	VESPERSDbUpgrade1Pt2(const QString &databaseNameToUpgrade, QObject *parent = 0);

	/// Indicates the dependencies of this upgrade.
	virtual QStringList upgradeFromTags() const;

	/// Returns true.  ALL modifications to the structure of the database need to be performed.
	virtual bool upgradeNecessary() const;

	/// Renames the current table, creates a new one with the old name with the modified column name.  Then all of the data is transferred to the new table.  Lastly, the old table is deleted.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "VESPERSDbUpgrade1.2".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // VESPERSDBUPGRADE1PT2_H
