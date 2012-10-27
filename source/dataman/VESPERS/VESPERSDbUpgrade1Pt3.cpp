#include "VESPERSDbUpgrade1Pt3.h"

#include "util/AMErrorMonitor.h"

VESPERSDbUpgrade1Pt3::VESPERSDbUpgrade1Pt3(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt3::upgradeFromTags() const
{
	return QStringList();
}

bool VESPERSDbUpgrade1Pt3::upgradeNecessary() const
{
	// This uses select instread of any of the convience methods in AMDatabase because the if the lists are empty, but the table still exists then we still want to upgrade the database.  Therefore, I've used select and check to make sure everything went okay with the transaction.
	bool upgrade = true;

	// First: 2D scans.
	QSqlQuery query = databaseToUpgrade_->select("VESPERS2DScanConfiguration_table", "usingCCD");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	// Second: Line scans.
	query = databaseToUpgrade_->select("VESPERSSpatialLineScanConfiguration_table", "usingCCD");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	// Third:  Energy scans.
	query = databaseToUpgrade_->select("VESPERSEnergyScanConfiguration_table", "ccdDetector");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	return upgrade;
}

bool VESPERSDbUpgrade1Pt3::upgradeImplementation()
{
	bool success = true;

	databaseToUpgrade_->startTransaction();

	// Update the 2D and line scan tables.
	success &= databaseToUpgrade_->update("VESPERS2DScanConfiguration_table", "usingCCD='false'", "usingCCD", 0);
	success &= databaseToUpgrade_->update("VESPERS2DScanConfiguration_table", "usingCCD='true'", "usingCCD", 1);
	success &= databaseToUpgrade_->update("VESPERSSpatialLineScanConfiguration_table", "usingCCD='false'", "usingCCD", 0);
	success &= databaseToUpgrade_->update("VESPERSSpatialLineScanConfiguration_table", "usingCCD='true'", "usingCCD", 1);

	if (!success){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_FROM_BOOL_TO_ENUM, "Was unable to update the usingCCD column for one/multiple/all of the values for VESPERS2DScanConfiguration and VESPERSSpatialLineScanConfiguration.");
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	// Change the name of the usingCDD column to ccdDetector.
	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "usingCCD", "ccdDetector", "INTEGER")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_2DCONFIGURATION_COLUMN_NAME, "Could not change the VESPERS2DScanConfiguration table column name.");
		return false;
	}

	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "usingCCD", "ccdDetector", "INTEGER")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_LINECONFIGURATION_COLUMN_NAME, "Could not change the VESPERSSpatialLineScanConfiguration table column name.");
		return false;
	}

	databaseToUpgrade_->startTransaction();

	// Update the energy scan ccdDetector column.
	success &= databaseToUpgrade_->update("VESPERSEnergyScanConfiguration_table", "ccdDetector=1", "ccdDetector", 2);
	success &= databaseToUpgrade_->update("VESPERSEnergyScanConfiguration_table", "ccdDetector=0", "ccdDetector", 1);

	if (!success){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_ENERGYSCAN_COLUMN, "Could not update the enum values in the VESPERSEnergyScanConfiguration table.");
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt3::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt3(this->databaseNameToUpgrade());

	if (this->databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt3::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.3";
}

QString VESPERSDbUpgrade1Pt3::description() const
{
	return "Consolidates the CCD detector selection across all of the scan configurations.  Changes the 2D and line scans from a bool to an enum and renames the column and updates the energy scan table to use the new values for the enum.";
}
