#include "VESPERSDbUpgrade1Pt4.h"

#include "util/AMErrorMonitor.h"

 VESPERSDbUpgrade1Pt4::~VESPERSDbUpgrade1Pt4(){}
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
	return true;
}

bool VESPERSDbUpgrade1Pt4::upgradeImplementation()
{
	bool success = true;

	if (databaseToUpgrade_->tableExists("VESPERS2DScanConfiguration_table") && databaseToUpgrade_->columnExists("VESPERS2DScanConfiguration_table", "motorsChoice")){

		databaseToUpgrade_->startTransaction();

		// Update the 2D and line scan tables.
		success &= databaseToUpgrade_->update("VESPERS2DScanConfiguration_table", "motorsChoice=0", "motorsChoice", 3);
		success &= databaseToUpgrade_->update("VESPERS2DScanConfiguration_table", "motorsChoice=1", "motorsChoice", 24);

		if (!success){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT4_COULD_NOT_UPDATE_2D_SCAN_COLUMNS, "Was unable to update the motorsChoice column for VESPERS2DScanConfiguration.");
			return false;
		}

		// Change the name of the motorsChoice column to motor.
		if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "motorsChoice", "motor")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT4_COULD_NOT_CHANGE_2D_SCAN_COLUMN_NAME, "Could not change the VESPERS2DScanConfiguration table column name.");
			return false;
		}
	}

	if (databaseToUpgrade_->tableExists("VESPERSSpatialLineScanConfiguration_table") && databaseToUpgrade_->columnExists("VESPERSSpatialLineScanConfiguration_table", "motorsChoice")){

		if (!databaseToUpgrade_->transactionInProgress())
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

		// Change the name of the motorChoice column to motor.
		if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "motorChoice", "motor")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT4_COULD_NOT_CHANGE_LINE_SCAN_COLUMN_NAME, "Could not change the VESPERSSpatialLineScanConfiguration table column name.");
			return false;
		}
	}

	if (databaseToUpgrade_->transactionInProgress())
		databaseToUpgrade_->commitTransaction();

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
