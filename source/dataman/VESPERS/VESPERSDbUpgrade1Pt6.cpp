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

	// 2) Fill the new region table.
	QVariantList ids = databaseToUpgrade_->retrieve("AMROIInfo_table", "id");
	QVariantList regionNames = databaseToUpgrade_->retrieve("AMROIInfo_table", "name");
	QVariantList energies = databaseToUpgrade_->retrieve("AMROIInfo_table", "energy");
	QVariantList lowerBounds = databaseToUpgrade_->retrieve("AMROIInfo_table", "low");
	QVariantList upperBounds = databaseToUpgrade_->retrieve("AMROIInfo_table", "high");

	for (int i = 0, size = ids.size(); i < size; i++){

		int newId = databaseToUpgrade_->insertOrUpdate(0, "AMRegionOfInterest_table", names,
													   QVariantList()
													   << "AMRegionOfInterest"
													   << 0
													   << QVariant()
													   << regionNames.at(i)
													   << energies.at(i)
													   << lowerBounds.at(i)
													   << upperBounds.at(i));

		if (newId == 0){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, 0, "Was unable to copy the data from AMROIInfo columns to the new AMRegionOfInterest table.");
			return false;
		}
	}

	// 3) Create the associated table for regions of interest, VESPERSScanConfigurationDbObject_table.
	names = QStringList() << "id1" << "table1" << "id2" << "table2";
	types = QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT";

	if (!databaseToUpgrade_->ensureTable("VESPERSScanConfigurationDbObject_table_regionsOfInterest", names, types)){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, 0, "Was unable to create the VESPERSScanConfigurationDbObjet_table_regionsOfInterest table.");
		return false;
	}


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
