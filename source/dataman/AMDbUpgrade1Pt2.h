#ifndef AMDBUPGRADE1PT2_H
#define AMDBUPGRADE1PT2_H

#include "dataman/database/AMDbUpgrade.h"

/// This class is used to upgrade databases from AMDetectorInfoSet to AMOldDetectorInfoSet. This is necessary to upgrade to the new AMDetector suite.


class AMDbUpgrade1Pt2 : public AMDbUpgrade
{
Q_OBJECT
public:
	/// Constructor calls the base constructor for AMDbUpgrade
	AMDbUpgrade1Pt2(QString databaseNameToUpgrade, QObject *parent = 0);

	/// This upgrade depends on AMDbUpgrade1.1
	virtual QStringList upgradeFromTags() const;

	/// Returns true if the old AMDetectorInfo AMDbObjectType is present in the AMDbObjectTypes_table AND the table contains the old columns for any of the requested databases.
	virtual bool upgradeNecessary() const;

	/// Iterates over the requested databases and uses dbObjectClassBecomes to make sure that the old detectorInfo is turned into the new one.
	virtual bool upgradeImplementation();

	/// Creates new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade* createCopy() const;

	/// Upgrade tag for this upgrade is "AMDbUpgrade1.2".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade
	virtual QString description() const;
};

#endif // AMDBUPGRADE1PT2_H
