#include "VESPERSDbUpgrade1Pt3.h"

#include "util/AMErrorMonitor.h"

 VESPERSDbUpgrade1Pt3::~VESPERSDbUpgrade1Pt3(){}
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
	return true;
}

bool VESPERSDbUpgrade1Pt3::upgradeImplementation()
{
	bool success = true;

	databaseToUpgrade_->startTransaction();

	// Update the 2D and line scan tables.
	if (databaseToUpgrade_->tableExists("VESPERS2DScanConfiguration_table") && databaseToUpgrade_->columnExists("VESPERS2DScanConfiguration_table", "usingCCD")){

		success &= databaseToUpgrade_->update("VESPERS2DScanConfiguration_table", "usingCCD='false'", "usingCCD", 0);
		success &= databaseToUpgrade_->update("VESPERS2DScanConfiguration_table", "usingCCD='true'", "usingCCD", 1);
	}

	if (databaseToUpgrade_->tableExists("VESPERSSpatialLineScanConfiguration_table") && databaseToUpgrade_->columnExists("VESPERSSpatialLineScanConfiguration_table", "usingCCD")){

		success &= databaseToUpgrade_->update("VESPERSSpatialLineScanConfiguration_table", "usingCCD='false'", "usingCCD", 0);
		success &= databaseToUpgrade_->update("VESPERSSpatialLineScanConfiguration_table", "usingCCD='true'", "usingCCD", 1);
	}

	if (!success){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_FROM_BOOL_TO_ENUM, "Was unable to update the usingCCD column for one/multiple/all of the values for VESPERS2DScanConfiguration and VESPERSSpatialLineScanConfiguration.");
		return false;
	}

	// Change the name of the usingCDD column to ccdDetector.
	if (databaseToUpgrade_->tableExists("VESPERS2DScanConfiguration_table")
			&& databaseToUpgrade_->columnExists("VESPERS2DScanConfiguration_table", "usingCCD")
			&& !AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "usingCCD", "ccdDetector", "INTEGER")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_2DCONFIGURATION_COLUMN_NAME, "Could not change the VESPERS2DScanConfiguration table column name.");
		return false;
	}

	if (databaseToUpgrade_->tableExists("VESPERSSpatialLineScanConfiguration_table")
			&& databaseToUpgrade_->columnExists("VESPERSSpatialLineScanConfiguration_table", "usingCCD")
			&& !AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "usingCCD", "ccdDetector", "INTEGER")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT3_COULD_NOT_UPDATE_LINECONFIGURATION_COLUMN_NAME, "Could not change the VESPERSSpatialLineScanConfiguration table column name.");
		return false;
	}

	// Update the energy scan ccdDetector column.
	if (databaseToUpgrade_->tableExists("VESPERSEnergyScanConfiguration_table")){

		success &= databaseToUpgrade_->update("VESPERSEnergyScanConfiguration_table", "ccdDetector=1", "ccdDetector", 2);
		success &= databaseToUpgrade_->update("VESPERSEnergyScanConfiguration_table", "ccdDetector=0", "ccdDetector", 1);
	}

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
