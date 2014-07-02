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


#ifndef VESPERSDBUPGRADE1PT3_H
#define VESPERSDBUPGRADE1PT3_H

#include "dataman/database/AMDbUpgrade.h"

#define VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_FROM_BOOL_TO_ENUM 389200
#define VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_2DCONFIGURATION_COLUMN_NAME 389201
#define VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_LINECONFIGURATION_COLUMN_NAME 389202
#define VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_ENERGYSCAN_COLUMN 389203

/// This class is a more elaborate database upgrade.
/*!
	This database upgrade has two primary tasks:  to consolidate the the CCD detector
	enums and make sure all of the scan configuration table schemas look the same.  This
	will enable CCD detector selection to be generalized to not be scan dependent, as
	it is now.

	The stages for this upgrade are as follows:

		1) Change the "usingCCD" boolean column to the new enum (false = 0 [NoCCD], true = 1 [Roper]) for 2D and line scans.
		2) Change the name of the column "usingCCD" to "ccdDetector" and change the type from TEXT to INTEGER.
		3) Update the energy scan to match the new CCD enum (0 [Roper] = 1 [new Roper], 1 [Mar] = 2 [New Mar]).
	*/
class VESPERSDbUpgrade1Pt3 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
 	virtual ~VESPERSDbUpgrade1Pt3();
	VESPERSDbUpgrade1Pt3(const QString &databaseNameToUpgrade, QObject *parent = 0);

	/// Indicates the dependencies of this upgrade.  There are no dependencies on this upgrade because it is entirely based on a single column which has been static since creation.
	virtual QStringList upgradeFromTags() const;

	/// Returns true.  ALL modifications to the structure of the database need to be performed.
	virtual bool upgradeNecessary() const;

	/// Makes all the changes outlined in the class description.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "VESPERSDbUpgrade1.3".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // VESPERSDBUPGRADE1PT3_H
