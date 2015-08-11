#ifndef IDEASDBUPGRADE1PT1_H
#define IDEASDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

#define IDEASDBUPGRADE1PT1_COULD_NOT_CREATE_COLUMN 938000
#define IDEASDBUPGRADE1PT1_COULD_NOT_CHANGE_COLUMN_NAME 938001
#define IDEASDBUPGRADE1PT1_COULD_NOT_ADD_ROW 938002
#define IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN 938003
#define IDEASDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS 938004
#define IDEASDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_VISIBLECOLUMNS 938005
#define IDEASDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_LOADCOLUMNS 938006
#define IDEASDBUPGRADE1PT1_COULD_NOT_ADD_TABLE 938007
#define IDEASDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN 938008
#define IDEASDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRY_VESPERSSCANCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE 938009

/// This class is used to give IDEAS a scan configuration database object similar to other beamlines.  It helps reduce the amount of code new scan configurations require.
class IDEASDbUpgrade1Pt1 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
	IDEASDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	virtual ~IDEASDbUpgrade1Pt1();

	/// Indicates the dependecies of this upgrade.  Because this is the first upgrade, there are no dependencies.
	virtual QStringList upgradeFromTags() const;

	/// Returns true all the time because all the old scans would have not had a dbObject aspect to its configuration.
	virtual bool upgradeNecessary() const;

	/// Creates an IDEASScanConfigurationDbObject table and necessary additions and fills it with XAS scan information.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "IDEASDbUpgrade1.1".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // IDEASDBUPGRADE1PT1_H
