#include "AMDbUpgrade1Pt6.h"

#include "util/AMErrorMonitor.h"

#include <QStringBuilder>

AMDbUpgrade1Pt6::AMDbUpgrade1Pt6(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{

}

AMDbUpgrade1Pt6::~AMDbUpgrade1Pt6()
{

}

QStringList AMDbUpgrade1Pt6::upgradeFromTags() const
{
	return QStringList();
}

bool AMDbUpgrade1Pt6::upgradeNecessary() const
{
	return databaseToUpgrade_->tableExists("VESPERSXRFScanConfiguration_table")
			|| databaseToUpgrade_->tableExists("BioXASXRFScanConfiguration_table")
			|| databaseToUpgrade_->tableExists("SXRMBXRFScanConfiguration_table")
			|| databaseToUpgrade_->tableExists("IDEASXRFScanConfiguration_table");
}

bool AMDbUpgrade1Pt6::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	if(!databaseToUpgrade_->tableExists("AMXRFScanConfiguration_table")
			&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
							 "AMXRFScanConfiguration_table",
							 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "detectorConfigurations" << "axisControlInfos",
							 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "TEXT" << "TEXT",
							 true,
							 "XRF Scan Configuration",
							 "AMXRFScanConfiguration;AMScanConfiguration;AMDbObject;QObject")){

		AMErrorMon::alert(this, AMDBUPGRADE1PT6_COULD_NOT_CREATE_AMXRFSCANCONFIGURATION_TABLE, "Could not create the AMXRFScanConfiguration_table.");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->tableExists("BioXASXRFScanConfiguration_table")
			&& !updateSingleXRFTable("BioXASXRFScanConfiguration_table")){

		AMErrorMon::alert(this, AMDBUPGRADE1PT6_FAILED_TO_MIGRATE_BIOXAS_XRF_CONFIGURATION, "Failed to upgrade the database for the BioXASXRFScanConfiguration_table.");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->tableExists("VESPERSXRFScanConfiguration_table")
			&& !updateSingleXRFTable("VESPERSXRFScanConfiguration_table")){

		AMErrorMon::alert(this, AMDBUPGRADE1PT6_FAILED_TO_MIGRATE_VESPERS_XRF_CONFIGURATION, "Failed to upgrade the database for the VESPERSXRFScanConfiguration_table.");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->tableExists("IDEASXRFScanConfiguration_table")
			&& !updateSingleXRFTable("IDEASXRFScanConfiguration_table")){

		AMErrorMon::alert(this, AMDBUPGRADE1PT6_FAILED_TO_MIGRATE_IDEAS_XRF_CONFIGURATION, "Failed to upgrade the database for the IDEASXRFScanConfiguration_table.");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->tableExists("SXRMBXRFScanConfiguration_table")
			&& !updateSingleXRFTable("SXRMBXRFScanConfiguration_table")){

		AMErrorMon::alert(this, AMDBUPGRADE1PT6_FAILED_TO_MIGRATE_SXRMB_XRF_CONFIGURATION, "Failed to upgrade the database for the SXRMBXRFScanConfiguration_table.");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade *AMDbUpgrade1Pt6::createCopy() const
{
	AMDbUpgrade* retVal = new AMDbUpgrade1Pt6(this->databaseNameToUpgrade());

	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();

	return retVal;
}

QString AMDbUpgrade1Pt6::upgradeToTag() const
{
	return "AMDbUpgrade1.6";
}

QString AMDbUpgrade1Pt6::description() const
{
	return "Upgrades all the beamline specific XRF scans to the new AM version.";
}

bool AMDbUpgrade1Pt6::updateSingleXRFTable(const QString &tableName)
{
	QList<int> ids = databaseToUpgrade_->objectsContaining("AMScan_table", "scanConfiguration", tableName);

	foreach (int id, ids){

		QVariant newValue = databaseToUpgrade_->retrieve(id, "AMScan_table", "scanConfiguration").toString().replace(QRegExp("^\\w+XRF"), "AMXRF");

		if (!databaseToUpgrade_->update(id,
						"AMScan_table",
						QStringList() << "AMDbObjectType" << "scanConfiguration",
						QVariantList() << "AMScan" << newValue)){

			AMErrorMon::alert(this, 0, QString("Could not update AMScan_table - scanConfiguration with %1").arg(newValue.toString()));
			return false;
		}
	}

	QVariantList detectorConfigurations = databaseToUpgrade_->retrieve(tableName, "detectorConfigurations");
	QVariantList axisControlInfos = databaseToUpgrade_->retrieve(tableName, "axisControlInfos");

	for (int i = 0, size = detectorConfigurations.size(); i < size; i++){

		if (!databaseToUpgrade_->insertOrUpdate(0,
							"AMXRFScanConfiguration_table",
							QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "detectorConfigurations" << "axisControlInfos",
							QVariantList() << "AMXRFScanConfiguration" << 0 << QVariant() << "Unnamed Object" << detectorConfigurations.at(i) << axisControlInfos.at(i))){

			AMErrorMon::alert(this, 0, QString("Could not insert into AMXRFScanConfiguration_table."));
			return false;
		}
	}

	return true;
}

