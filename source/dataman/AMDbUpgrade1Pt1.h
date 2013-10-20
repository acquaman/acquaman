#ifndef AMDBUPGRADE1PT1_H
#define AMDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

/// This class is used to upgrade databases from AMDetectorInfo to AMOldDetectorInfo. This is necessary to upgrade to the new AMDetector suite.


class AMDbUpgrade1Pt1 : public AMDbUpgrade
{
Q_OBJECT
public:
	/// Constructor calls the base constructor for AMDbUpgrade
	AMDbUpgrade1Pt1(QString databaseNameToUpgrade, QObject *parent = 0);

	/// Indicates the dependencies of this upgrade (in this case there are no general dependencies, but beamline specific upgrades may need to be run first).
	virtual QStringList upgradeFromTags() const;

	/// Returns true if the old AMDetectorInfo AMDbObjectType is present in the AMDbObjectTypes_table AND the table contains the old columns for any of the requested databases.
	virtual bool upgradeNecessary() const;

	/// Iterates over the requested databases and uses dbObjectClassBecomes to make sure that the old detectorInfo is turned into the new one.
	virtual bool upgradeImplementation();

	/// Creates new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade* createCopy() const;

	/// Upgrade tag for this upgrade is "AMDbUpgrade1.1".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade
	virtual QString description() const;
};

#endif // AMDBUPGRADE1PT1_H
