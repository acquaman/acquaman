#ifndef BIOXASDBUPGRADE1PT1_H
#define BIOXASDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

#define BIOXASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN 234987
#define BIOXASDBUPGRADE1PT1_COULD_NOT_CREATE_TABLE 234988
#define BIOXASDBUPGRADE1PT1_COULD_NOT_INSERT_OR_UPDATE_TABLE 234989
#define BIOXASDBUPGRADE1PT1_UPGRADE_COMPLETED_SUCCESSFULLY 2348990
#define BIOXASDBUPGRADE1PT1_TABLE_DOES_NOT_EXIST 2348991
#define BIOXASDBUPGRADE1PT1_INSERT_OR_UPDATE_SUCCESSFUL 2348992

/// Upgrades database to account for new BioXASXASScanConfiguration, which functionally replaces existing BioXASSideXASScanConfiguration and BioXASMainXASScanConfiguration.
class BioXASDbUpgrade1Pt1 : public AMDbUpgrade
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASDbUpgrade1Pt1();

	/// Indicates the dependencies of this upgrade.
	virtual QStringList upgradeFromTags() const;
	/// Returns true if this upgrade needs to be done.
	virtual bool upgradeNecessary() const;
	/// Returns true if the upgrade is completed successfully, false otherwise. Contains the steps to complete the upgrade.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade.
	virtual AMDbUpgrade *createCopy() const;

	/// Returns the upgrade tag.
	virtual QString upgradeToTag() const;

	/// Returns the description.
	virtual QString description() const;

protected:
	/// Returns true if successful, false otherwise. Merges data from the named table into BioXASXASScanConfiguration_table.
	bool addConfigurationTableToXASTable(const QString &configurationTableName);

	/// Returns true if successful, false otherwise. Iterates through entries in AMScan_table and updates scanConfiguration entries from the named configuration table to BioXASXASScanConfiguration_table.
	bool updateScanTable(int configurationID, const QString &configurationTableName);

	/// Returns true if successful, false otherwise. Adds entries from named configuration's scan axes table to BioXASXASScanConfiguration_table_scanAxes.
	bool addConfigurationAxesToXASAxesTable(const QString &configurationTableName, const QString &scanAxesTableName);

};

#endif // BIOXASDBUPGRADE1PT1_H
