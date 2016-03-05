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


#ifndef SGMDBUPGRADE1PT1_H
#define SGMDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

/// This class is used to upgrade databases from PGTDetectorInfo, MCPDetectorInfo, and OceanOptics65000DetectorInfo to CLSPGTDetectorInfo, SGMMCPDetectorInfo, and CLSOceanOptics65000DetectorInfo.

class SGMDbUpgrade1Pt1 : public AMDbUpgrade
{
Q_OBJECT
public:
	/// Constructor calls the base constructor for AMDbUpgrade
 	virtual ~SGMDbUpgrade1Pt1();
	SGMDbUpgrade1Pt1(QString databaseNameToUpgrade, QObject *parent = 0);

	/// Indicates the dependencies of this upgrade (in this case there are none).
	virtual QStringList upgradeFromTags() const;

	/// Returns true if the old PGT, MCP, or OceanOptics AMDbObjectTypes are present in the AMDbObjectTypes_table for any of the requested databases.
	virtual bool upgradeNecessary() const;

	/// Iterates over the requested databases and uses dbObjectClassBecomes to make sure that the old detectorInfos are turned into the new ones (PGT, MCP, OceanOptics).
	virtual bool upgradeImplementation();

	/// Creates new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade* createCopy() const;

	/// Upgrade tag for this upgrade is "SGMDbUpgrade1.1".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade
	virtual QString description() const;
};

#endif // SGMDBUPGRADE1PT1_H
