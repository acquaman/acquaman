#ifndef AMDBUPGRADE1PT7_H
#define AMDBUPGRADE1PT7_H

#include "dataman/database/AMDbUpgrade.h"

#define AMDBUPGRADE1PT7_COULD_NOT_UPDATE_COLUMN 979700


/// This class brings the AMRun table of database up to date by:
///     change the beamline id to correct one
///
class AMDbUpgrade1Pt7 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
	AMDbUpgrade1Pt7(int facilityId, const QString &facilityName, const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	~AMDbUpgrade1Pt7();

	/// Indicates the dependecies of this upgrade.  This is the first upgrade and therefore there are no dependencies.
	virtual QStringList upgradeFromTags() const;

	/// Returns true if there exist any 2D or EXAFS scans.
	virtual bool upgradeNecessary() const;

	/// Iterates over all scans and migrates all the data from specific configuration tables to the shared configuration.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "AMDbUpgrade1.7".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;

protected:
	int facilityId_;
	QString facilityName_;
};

#endif // AMDBUPGRADE1PT7_H
