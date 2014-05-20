#include "VESPERSDbUpgrade1Pt6.h"

#include "util/AMErrorMonitor.h"

VESPERSDbUpgrade1Pt6::VESPERSDbUpgrade1Pt6(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt6::upgradeFromTags() const
{
	return QStringList() << "VESPERSDbUpgrade1.5";
}

bool VESPERSDbUpgrade1Pt6::upgradeNecessary() const
{
	return true;
}

bool VESPERSDbUpgrade1Pt6::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	// 1) Create the AMRegionOfInterest_table.
	QStringList names;
	names << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "energy" << "lowerBound" << "upperBound";
	QStringList types;
	types << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "REAL" << "REAL";

	if (!databaseToUpgrade_->ensureTable("AMRegionOfInterest_table", names, types)){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, 0, "Was unable to create the AMRegionOfInterest table.");
		return false;
	}

	// 2) Create the associated table for regions of interest, VESPERSScanConfigurationDbObject_table.
	names = QStringList() << "id1" << "table1" << "id2" << "table2";
	types = QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT";

//	if (!databaseToUpgrade_->ensureTable("VESPERSScanConfigurationDbObject_table_regionsOfInterest")){

//		databaseToUpgrade_->rollbackTransaction();
//		AMErrorMon::alert(this, 0, "Was unable to create the VESPERSScanConfigurationDbObjet_table_regionsOfInterest table.");
//		return false;
//	}



	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt6::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt6(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt6::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.6";
}

QString VESPERSDbUpgrade1Pt6::description() const
{
	return "Upgrades database by migrating away from old AMROIInfo and AMROIInfoLists to the new AMRegionOfInterest.";
}
