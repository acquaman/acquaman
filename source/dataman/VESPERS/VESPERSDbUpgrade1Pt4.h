#ifndef VESPERSDBUPGRADE1PT4_H
#define VESPERSDBUPGRADE1PT4_H

#include "dataman/database/AMDbUpgrade.h"

#define VESPERSDBUPGRADE1PT4_COULD_NOT_UPDATE_2D_SCAN_COLUMNS 389300
#define VESPERSDBUPGRADE1PT4_COULD_NOT_CHANGE_2D_SCAN_COLUMN_NAME 389301
#define VESPERSDBUPGRADE1PT4_COULD_NOT_CHANGE_LINE_SCAN_COLUMNS 389302
#define VESPERSDBUPGRADE1PT4_COULD_NOT_CHANGE_LINE_SCAN_COLUMN_NAME 389303

/// This class upgrades the database to use the new VESPERS::Motor enum.
/*!
	This database upgrade takes the two different motor choice enums in VESPERS2DScanConfiguration
	and VESPERSSpatialLineScanConfiguration and consolidates them to use the new
	enum that contains just individual motors that can be OR'ed together because
	they are setup in powers of two.  The steps are the same for both tables:

		1) Change the values in the table from the old enum to the new enum values.
		2) Change the name of the column to make it identical across scan configurations.
  */
class VESPERSDbUpgrade1Pt4 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.  Need to provide the name of the database to upgrade.
 	virtual ~VESPERSDbUpgrade1Pt4();
	VESPERSDbUpgrade1Pt4(const QString &databaseNameToUpgrade, QObject *parent = 0);

	/// Indicates the dependicies of this upgrade.  There are no dependencies on this database upgrade because it is modifying only a single columns contents that have been static since creation.
	virtual QStringList upgradeFromTags() const;

	/// Returns true.  ALL modifications to the structure of the database need to be performed.
	virtual bool upgradeNecessary() const;

	/// Makes all the changes outlined in the class description.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "VESPERSDbUpgrade1.4".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // VESPERSDBUPGRADE1PT4_H
