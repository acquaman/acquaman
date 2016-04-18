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


#ifndef AMDBUPGRADE1PT3_H
#define AMDBUPGRADE1PT3_H

#include "dataman/database/AMDbUpgrade.h"

#define AMDBUPGRADE1PT3_COULD_NOT_UPDATE_DB 978000
#define AMDBUPGRADE1PT3_BEGINNING_MESSAGE 978001
#define AMDBUPGRADE1PT3_END_MESSAGE 978002

/// This class is used to upgrade the database from using the old file format used in alpha and beta testing of the new scan action controller.  It also goes through every data file and replaces the Version text in the header.
class AMDbUpgrade1Pt3 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Calls the base constructor for AMDbUpgrade.
	AMDbUpgrade1Pt3(const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	virtual ~AMDbUpgrade1Pt3(){}

	/// This upgrade doesn't depend on anything.
	virtual QStringList upgradeFromTags() const;
	/// Returns true if the old AMDetectorInfo AmDbObjectType is present in the AMDbObjectTypes_table AND the table contains the old columns for any of the requested databases.
	virtual bool upgradeNecessary() const;

	/// Finds all instances of the fileFormat "sgm2013XAS" and replaces it with "amRegionAscii2013".  It also replaces the version inside of the data files.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;
	/// Upgrade tag for this upgrade is "AMDbUpgrade1.3".
	virtual QString upgradeToTag() const;
	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // AMDBUPGRADE1PT3_H
