#ifndef CLSDBUPGRADE1PT3_H
#define CLSDBUPGRADE1PT3_H

#include "dataman/database/AMDbUpgrade.h"

#define CLSDBUPGRADE1PT3_COULD_NOT_CREATE_TABLE 201010301
#define CLSDBUPGRADE1PT3_COULD_NOT_CREATE_INDEX 201010302
#define CLSDBUPGRADE1PT3_COULD_NOT_INSERT_OR_UPDATE_TABLE 201010303


class CLSDbUpgrade1Pt3 : public AMDbUpgrade
{
    Q_OBJECT
public:
	explicit CLSDbUpgrade1Pt3(const QString &databaseNameToUpgrade, QObject *parent = 0);

	/// Destructor.
	virtual ~CLSDbUpgrade1Pt3();

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

};

#endif // CLSDBUPGRADE1PT3_H
