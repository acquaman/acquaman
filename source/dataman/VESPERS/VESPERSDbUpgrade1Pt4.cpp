#include "VESPERSDbUpgrade1Pt4.h"

#include "util/AMErrorMonitor.h"

VESPERSDbUpgrade1Pt4::VESPERSDbUpgrade1Pt4(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt4::upgradeFromTags() const
{
	return QStringList();
}

bool VESPERSDbUpgrade1Pt4::upgradeNecessary() const
{
	// This uses select instread of any of the convience methods in AMDatabase because the if the lists are empty, but the table still exists then we still want to upgrade the database.  Therefore, I've used select and check to make sure everything went okay with the transaction.
	bool upgrade = true;

	// First: 2D scans.
	QSqlQuery query = databaseToUpgrade_->select("VESPERS2DScanConfiguration_table", "motorsChoice");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	// Second: Line scans.
	query = databaseToUpgrade_->select("VESPERSSpatialLineScanConfiguration_table", "motorChoice");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	return upgrade;
}

bool VESPERSDbUpgrade1Pt4::upgradeImplementation()
{
	bool success = true;

	databaseToUpgrade_->startTransaction();

	// Update the 2D and line scan tables.
	success &= databaseToUpgrade_->update("VESPERS2DScanConfiguration_table", "motorsChoice=0", "motorsChoice", 3);
	success &= databaseToUpgrade_->update("VESPERS2DScanConfiguration_table", "motorsChoice=1", "motorsChoice", 24);

	if (!success){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT4_COULD_NOT_UPDATE_2D_SCAN_COLUMNS, "Was unable to update the motorsChoice column for VESPERS2DScanConfiguration.");
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	// Change the name of the motorsChoice column to motor.
	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "motorsChoice", "motor")){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT4_COULD_NOT_CHANGE_2D_SCAN_COLUMN_NAME, "Could not change the VESPERS2DScanConfiguration table column name.");
		return false;
	}

	databaseToUpgrade_->startTransaction();

	success &= databaseToUpgrade_->update("VESPERSSpatialLineScanConfiguration_table", "motorChoice=2", "motorChoice", 2);	// Technically not required, but I wanted to be pedantic.
	success &= databaseToUpgrade_->update("VESPERSSpatialLineScanConfiguration_table", "motorChoice=1", "motorChoice", 8);
	success &= databaseToUpgrade_->update("VESPERSSpatialLineScanConfiguration_table", "motorChoice=0", "motorChoice", 1);
	success &= databaseToUpgrade_->update("VESPERSSpatialLineScanConfiguration_table", "motorChoice=3", "motorChoice", 16);

	if (!success){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT4_COULD_NOT_CHANGE_LINE_SCAN_COLUMNS, "Was unable to update the motorsChoice column for VESPERSSpatialLineScanConfiguration.");
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	// Change the name of the motorChoice column to motor.
	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "motorChoice", "motor")){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT4_COULD_NOT_CHANGE_LINE_SCAN_COLUMN_NAME, "Could not change the VESPERSSpatialLineScanConfiguration table column name.");
		return false;
	}

	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt4::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt4(this->databaseNameToUpgrade());

	if (this->databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt4::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.4";
}

QString VESPERSDbUpgrade1Pt4::description() const
{
	return "Consolidates the motor's enum that is used for what motors a scan should use when doing a scan.  Updates the values in both VESPERS2DScanConfiguration_table and VESPERSSpatialLineScanConfiguration_table to the new values.";
}
