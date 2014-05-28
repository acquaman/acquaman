#ifndef VESPERSDBUPGRADE1PT6_H
#define VESPERSDBUPGRADE1PT6_H

#include "dataman/database/AMDbUpgrade.h"

/// This class upgrades the database to change how regions of interest are stored in the database.
/*!
	This database upgrade changes VESPERS databases to use the new paradigm of AMRegionOfInterest
	rather than AMROIInfo/AMROIInfoList's.  This needs to change it so that old classes reference
	the correct tables and then make sure all the old AMROIInfo entries are properly transferred
	to the new AMRegionOfInterest table.
  */
class VESPERSDbUpgrade1Pt6 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
	VESPERSDbUpgrade1Pt6(const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSDbUpgrade1Pt6(){}

	/// Indicates the dependencies of this upgrade.  Requires the last big upgrade of "VESPERSDbUpgrade1.5".
	virtual QStringList upgradeFromTags() const;

	/// Returns true.  ALL modifications to the structure of the database needs to be done.
	virtual bool upgradeNecessary() const;

	/// Makes all the necessary changes outlined in the class description.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tage for this upgrade is "VESPERSDbUpgrade1.6".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // VESPERSDBUPGRADE1PT6_H
