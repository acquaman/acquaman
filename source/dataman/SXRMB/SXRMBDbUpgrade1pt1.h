#ifndef SXRMBDBUPGRADE1PT1_H
#define SXRMBDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

#define SXRMBDBUPGRADE1PT1_COULD_NOT_CREATE_COLUMN 778000
#define SXRMBDBUPGRADE1PT1_COULD_NOT_CHANGE_COLUMN_NAME 778001
#define SXRMBDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN 778002
#define SXRMBDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN 778003
#define SXRMBDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS 778004
#define SXRMBDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_VISIBLECOLUMNS 778005
#define SXRMBDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_LOADCOLUMNS 778006


/// This class brings the SXRMB database up to date by consolidating a lot of the meta information into the configuration db object rather than individual scan configurations.
class SXRMBDbUpgrade1pt1 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
	SXRMBDbUpgrade1pt1(const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	~SXRMBDbUpgrade1pt1();

	/// Indicates the dependecies of this upgrade.  This is the first upgrade and therefore there are no dependencies.
	virtual QStringList upgradeFromTags() const;

	/// Returns true if there exist any 2D or EXAFS scans.
	virtual bool upgradeNecessary() const;

	/// Iterates over all scans and migrates all the data from specific configuration tables to the shared configuration.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "SXRMBDbUpgrade1.1".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // SXRMBDBUPGRADE1PT1_H
