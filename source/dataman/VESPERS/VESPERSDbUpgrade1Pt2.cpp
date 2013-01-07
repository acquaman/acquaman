#include "VESPERSDbUpgrade1Pt2.h"

#include "util/AMErrorMonitor.h"

VESPERSDbUpgrade1Pt2::VESPERSDbUpgrade1Pt2(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt2::upgradeFromTags() const
{
	return QStringList() << "VESPERSDbUpgrade1.1";
}

bool VESPERSDbUpgrade1Pt2::upgradeNecessary() const
{
	// This uses select instread of any of the convience methods in AMDatabase because the if the lists are empty, but the table still exists then we still want to upgrade the database.  Therefore, I've used select and check to make sure everything went okay with the transaction.
	bool upgrade = true;

	// First: 2D scans.
	QSqlQuery query = databaseToUpgrade_->select("VESPERS2DScanConfiguration_table", "fluorescenceDetectorChoice");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	// Second: Line scans.
	query = databaseToUpgrade_->select("VESPERSSpatialLineScanConfiguration_table", "fluorescenceDetectorChoice");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	// Third:  EXAFS scans.
	query = databaseToUpgrade_->select("VESPERSEXAFSScanConfiguration_table", "fluorescenceDetectorChoice");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	return upgrade;
}

bool VESPERSDbUpgrade1Pt2::upgradeImplementation()
{
	// Since all three of the tables MUST exist, we'll just go ahead and do the upgrade without any further existence checks.
	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "fluorescenceDetectorChoice", "fluorescenceDetector")){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_2DCONFIGURATION, "Could not upgrade the 2D scan configuration table.");
		return false;
	}

	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "fluorescenceDetectorChoice", "fluorescenceDetector")){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_LINECONFIGURATION, "Could not upgrade the spatial line scan configuration table.");
		return false;
	}

	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERSEXAFSScanConfiguration_table", "fluorescenceDetectorChoice", "fluorescenceDetector")){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_EXAFSCONFIGURATION, "Could not upgrade the EXAFS scan configuration table.");
		return false;
	}

	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt2::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt2(this->databaseNameToUpgrade());

	if (this->databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt2::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.2";
}

QString VESPERSDbUpgrade1Pt2::description() const
{
	return "Changes the name of fluorescenceDetectorChoice to fluorescenceDetector.";
}
