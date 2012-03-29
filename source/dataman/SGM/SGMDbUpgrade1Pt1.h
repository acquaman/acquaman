#ifndef SGMDBUPGRADE1PT1_H
#define SGMDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

/// This class is used to upgrade databases from PGTDetectorInfo, MCPDetectorInfo, and OceanOptics65000DetectorInfo to CLSPGTDetector, SGMMCPDetectorInfo, and CLSOceanOptics65000DetectorInfo.

class SGMDbUpgrade1Pt1 : public AMDbUpgrade
{
Q_OBJECT
public:
	/// Constructor calls the base constructor for AMDbUpgrade
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
