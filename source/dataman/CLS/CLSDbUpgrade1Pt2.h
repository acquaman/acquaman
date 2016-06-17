#ifndef CLSDBUPGRADE1PT2_H
#define CLSDBUPGRADE1PT2_H

#include "dataman/database/AMDbUpgrade.h"

#include "application/CLS/CLSAppController.h"

#define CLSDbUpgrade1Pt2_COULD_NOT_DELETE_FACILITY 779000
#define CLSDbUpgrade1Pt2_COULD_NOT_INSERT_NEW_FACILITY 779001
#define CLSDbUpgrade1Pt2_COULD_NOT_DELETE_AMRUN_COLUMN 779002


/// This class brings the following tables up to date:
///    AMFacility:
///       - remove the facilityId column
///       - remove all the records
///       - add the distinguished record for the current beamline
///
///    AMRun:
///       - remove the facilityId column
///

class CLSDbUpgrade1Pt2 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
	CLSDbUpgrade1Pt2(const QString &facilityName, const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	~CLSDbUpgrade1Pt2();

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
	QString targetFacilityName_;
};

#endif // CLSDBUPGRADE1PT2_H
