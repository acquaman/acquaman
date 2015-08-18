#ifndef BIOXASDBUPGRADE1PT1_H
#define BIOXASDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

#define BIOXASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN 234987
#define BIOXASDBUPGRADE1PT1_COULD_NOT_CREATE_TABLE 234988
#define BIOXASUPGRADE1PT1_COULD_NOT_INSERT_OR_UPDATE_TABLE 234989

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

	/// Returns true. All modifications to the structure of the database needs to be done.
	virtual bool upgradeNecessary() const;

	/// Makes all the necessary changes outlined in the class description.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Returns the upgrade tag for this upgrade.
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;

};

#endif // BIOXASDBUPGRADE1PT1_H
