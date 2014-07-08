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


#ifndef VESPERSDBUPGRADE1PT1_H
#define VESPERSDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

#define VESPERSDBUPGRADE1PT1_INVALID_SCAN_CONFIGURATION 389000
#define VESPERSDBUPGRADE1PT1_COULD_NOT_INSERT 389001
#define VESPERSDBUPGRADE1PT1_INCORRECT_ELEMENT_COUNT 389002

/// This class is used to upgrade databases with 2D scans by adding in the additional spectra data sources.
class VESPERSDbUpgrade1Pt1 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
 	virtual ~VESPERSDbUpgrade1Pt1();
	VESPERSDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent = 0);

	/// Indicates the dependecies of this upgrade.  Because this is the first upgrade, there are no dependencies.
	virtual QStringList upgradeFromTags() const;

	/// Returns true if there exist 2D scans.
	virtual bool upgradeNecessary() const;

	/// Iterates over the 2D scans and adds the appropriate lines to the table.  Because we are adding raw data sources, there are additions to the rawDataSources_table and the reference table.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "VESPERSDbUpgrade1.1".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // VESPERSDBUPGRADE1PT1_H
