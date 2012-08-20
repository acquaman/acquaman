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