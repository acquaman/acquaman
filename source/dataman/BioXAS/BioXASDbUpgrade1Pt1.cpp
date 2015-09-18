#include "BioXASDbUpgrade1Pt1.h"
#include "util/AMErrorMonitor.h"

BioXASDbUpgrade1Pt1::BioXASDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNameToUpgrade, parent)
{

}

BioXASDbUpgrade1Pt1::~BioXASDbUpgrade1Pt1()
{

}

QStringList BioXASDbUpgrade1Pt1::upgradeFromTags() const
{
	return QStringList();
}

bool BioXASDbUpgrade1Pt1::upgradeNecessary() const
{
	bool usingXRFDetector = databaseToUpgrade_->tableExists("BioXASScanConfigurationDbObject_table") && databaseToUpgrade_->columnExists("BioXASScanConfigurationDbObject_table", "usingXRFDetector");
	bool sideConfiguration = !databaseToUpgrade_->tableExists("BioXASXASScanConfiguration_table") && databaseToUpgrade_->tableExists("BioXASSideXASScanConfiguration_table");
	bool mainConfiguration = !databaseToUpgrade_->tableExists("BioXASXASScanConfiguration_table") && databaseToUpgrade_->tableExists("BioXASMainXASScanConfiguration_table");

	bool upgradeNeeded = (usingXRFDetector || sideConfiguration || mainConfiguration);
	return upgradeNeeded;
}

bool BioXASDbUpgrade1Pt1::upgradeImplementation()
{
	// Begin upgrade.

	databaseToUpgrade_->startTransaction();

	// Remove usingXRFDetector column from BioXASScanConfigurationDbObject_table, if it exists. This column is no longer used.

	if (databaseToUpgrade_->tableExists("BioXASScanConfigurationDbObject_table")) {
		if (databaseToUpgrade_->columnExists("BioXASXASScanConfigurationDbObject_table", "usingXRFDetector")) {
			if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "BioXASScanConfigurationDbObject_table", "usingXRFDetector")) {
				AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the usingXRFDetector column from BioXASScanConfigurationDbObject_table.");
				databaseToUpgrade_->rollbackTransaction();
				return false;
			}
		}
	}

	// Create and populate BioXASXASScanConfiguration_table and BioXASXASScanConfiguration_table_scanAxes, if they doesn't already exist.

	if (!databaseToUpgrade_->tableExists("BioXASXASScanConfiguration_table")) {
		QStringList dbObjectNames = QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "detectorConfigurations" << "axisControlInfos" << "configurationDbObject" << "header";
		QStringList dbObjectTypes = QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "TEXT" << "TEXT" << "TEXT" << "TEXT";

		if (!AMDbUpgradeSupport::addTable(databaseToUpgrade_, "BioXASXASScanConfiguration_table", dbObjectNames, dbObjectTypes, true, "BioXAS XAS Scan Configuration", "BioXASXASScanConfiguration;AMGenericStepScanConfiguration;AMStepScanConfiguration;AMScanConfiguration;AMDbObject;QObject")) {
			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_CREATE_TABLE, "Could not create BioXASXASScanConfiguration_table.");
			return false;
		}

		QStringList scanAxesNames = QStringList() << "id1" << "table1" << "id2" << "table2";
		QStringList scanAxesTypes = QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT";

		if (!databaseToUpgrade_->ensureTable("BioXASXASScanConfiguration_table_scanAxes", scanAxesNames, scanAxesTypes)) {
			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_CREATE_TABLE, "Could not create BioXASXASScanConfiguration_table_scanAxes.");
			return false;
		}

		// Populate BioXASXASScanConfiguration_table with items from BioXASSideXASScanConfiguration_table, if it exists.

		if (databaseToUpgrade_->tableExists("BioXASSideXASScanConfiguration_table")) {

			if (!addConfigurationTableToXASTable("BioXASSideXASScanConfiguration_table"))
				return false;

			if (!addConfigurationAxesToXASAxesTable("BioXASSideXASScanConfiguration_table", "BioXASSideXASScanConfiguration_table_scanAxes"))
				return false;
		}

		// Populate BioXASXASScanConfiguration_table with items from BioXASMainXASScanConfiguration_table, if it exists.

		if (databaseToUpgrade_->tableExists("BioXASMainXASScanConfiguration_table")) {

			if (!addConfigurationTableToXASTable("BioXASMainXASScanConfiguration_table"))
				return false;

			if (!addConfigurationAxesToXASAxesTable("BioXASMainXASScanConfiguration_table", "BioXASMainXASScanConfiguration_table_scanAxes"))
				return false;
		}
	}

	// Upgrade complete.

	AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_UPGRADE_COMPLETED_SUCCESSFULLY, "Database upgrade completed successfully!");

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade* BioXASDbUpgrade1Pt1::createCopy() const
{
	AMDbUpgrade *copy = new BioXASDbUpgrade1Pt1(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString BioXASDbUpgrade1Pt1::upgradeToTag() const
{
	return "BioXASDbUpgrade1.1";
}

QString BioXASDbUpgrade1Pt1::description() const
{
	return "Upgrades database to account for new BioXASXASScanConfiguration, which functionally replaces existing BioXASSideXASScanConfiguration and BioXASMainXASScanConfiguration.";
}

bool BioXASDbUpgrade1Pt1::addConfigurationTableToXASTable(const QString &configurationTableName)
{
	if (!databaseToUpgrade_->tableExists("BioXASXASScanConfiguration_table")) {
		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_TABLE_DOES_NOT_EXIST, QString("Could not populate BioXASXASScanConfiguration_table with entries from %1--BioXASXASScanConfiguration_table does not exist.").arg(configurationTableName));
		return false;
	}

	if (!databaseToUpgrade_->tableExists(configurationTableName)) {
		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_TABLE_DOES_NOT_EXIST, QString("Could not populate BioXASXASScanConfiguration_table with entries from %1--%1 does not exist.").arg(configurationTableName));
		return false;
	}

	// Identify data from the named configuration table that will be added to BioXASXASScanConfiguration_table.

	QVariantList ids = databaseToUpgrade_->retrieve(configurationTableName, "id");
	QVariantList thumbnailCounts = databaseToUpgrade_->retrieve(configurationTableName, "thumbnailCount");
	QVariantList thumbnailFirstIds = databaseToUpgrade_->retrieve(configurationTableName, "thumbnailFirstId");
	QVariantList names = databaseToUpgrade_->retrieve(configurationTableName, "name");
	QVariantList detectors = databaseToUpgrade_->retrieve(configurationTableName, "detectorConfigurations");
	QVariantList axisControls = databaseToUpgrade_->retrieve(configurationTableName, "axisControlInfos");
	QVariantList dbObjects = databaseToUpgrade_->retrieve(configurationTableName, "configurationDbObject");
	QVariantList headers = databaseToUpgrade_->retrieve(configurationTableName, "header");

	QList<int> updateIds;

	foreach (QVariant id, ids)
		updateIds << id.toString().split(";").last().toInt();

	// Add each configuration to BioXASXASScanConfiguration_table, and update corresponding entry in AMScan_table.

	for (int idIndex = 0, idIndexCount = updateIds.count(); idIndex < idIndexCount; idIndex++) {

		int newId = databaseToUpgrade_->insertOrUpdate(0, "BioXASXASScanConfiguration_table",
													   QStringList()
													   << "AMDbObjectType"
													   << "thumbnailCount"
													   << "thumbnailFirstId"
													   << "name"
													   << "detectorConfigurations"
													   << "axisControlInfos"
													   << "configurationDbObject"
													   << "header",
													   QVariantList()
													   << "BioXASXASScanConfiguration"
													   << thumbnailCounts.at(idIndex)
													   << thumbnailFirstIds.at(idIndex)
													   << names.at(idIndex)
													   << detectors.at(idIndex)
													   << axisControls.at(idIndex)
													   << dbObjects.at(idIndex)
													   << headers.at(idIndex)
													   );

		if (newId == 0) {
			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_INSERT_OR_UPDATE_TABLE, QString("Could not populate BioXASXASScanConfiguration_table with old configurations from %1.").arg(configurationTableName));
			return false;
		}

		if (!updateScanTable(newId, configurationTableName))
			return false;
	}

	// Insertion of new entries to BioXASXASScanConfiguration_table complete.

	AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_INSERT_OR_UPDATE_SUCCESSFUL, QString("Successfully added entries from %1 to BioXASXASScanConfiguration_table!").arg(configurationTableName));

	return true;
}

bool BioXASDbUpgrade1Pt1::updateScanTable(int configurationID, const QString &configurationTableName)
{
	if (!databaseToUpgrade_->tableExists("AMScan_table")) {
		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_TABLE_DOES_NOT_EXIST, QString("Could not update AMScan_table entries--AMScan_table does not exist.").arg(configurationTableName));
		return false;
	}

	// Collect information from AMScan_table to be updated, or used for update.

	QVariantList ids = databaseToUpgrade_->retrieve("AMScan_table", "id");
	QVariantList configurations = databaseToUpgrade_->retrieve("AMScan_table", "scanConfiguration");

	QList<int> updateIds;

	foreach (QVariant id, ids)
		updateIds << id.toString().split(";").last().toInt();


	// Iterate through the entries in AMScan_table until out of entries or an entry is found
	// that has the scanConfiguration matching the given table name and id.
	// Each scan will correspond to a unique scan configuration--no need to keep looking once
	// configuration is found.

	bool configurationFound = false;

	for (int idIndex = 0, idIndexCount = updateIds.count(); idIndex < idIndexCount && !configurationFound; idIndex++) {

		QString entry = configurations.at(idIndex).toString();
		QString entryTableName = entry.split(";").first();

		if (entryTableName == configurationTableName) {
			QString newEntry = QString("BioXASXASScanConfiguration_table;%1").arg(configurationID);

			if (!databaseToUpgrade_->update(updateIds.at(idIndex), "AMScan_table", "scanConfiguration", newEntry)) {
				databaseToUpgrade_->rollbackTransaction();
				AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_INSERT_OR_UPDATE_TABLE, QString("Could not update %1 entry in AMScan_table.").arg(configurationTableName));
				return false;
			}

			configurationFound = true;
		}
	}

	if (!configurationFound) {
		AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_INSERT_OR_UPDATE_TABLE, QString("Could not update %1 entry in AMScan_table--entry could not be found.").arg(configurationTableName));
	}

	return true;
}

bool BioXASDbUpgrade1Pt1::addConfigurationAxesToXASAxesTable(const QString &configurationTableName, const QString &scanAxesTableName)
{
	if (!databaseToUpgrade_->tableExists("BioXASXASScanConfiguration_table_scanAxes")) {
		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_TABLE_DOES_NOT_EXIST, QString("Could not populate BioXASXASScanConfiguration_table_scanAxes with entries from %1--BioXASXASScanConfiguration_table_scanAxes does not exist.").arg(configurationTableName));
		return false;
	}

	if (!databaseToUpgrade_->tableExists(scanAxesTableName)) {
		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_TABLE_DOES_NOT_EXIST, QString("Could not populate BioXASXASScanConfiguration_table_scanAxes with entries from %1--%1 does not exist.").arg(scanAxesTableName));
		return false;
	}

	// Identify data from named scan axes table that will be transferred to BioXASXASScanConfiguration_table_scanAxes.

	QVariantList ids = databaseToUpgrade_->retrieve(scanAxesTableName, "id");
	QVariantList configurationIds = databaseToUpgrade_->retrieve(scanAxesTableName, "id1");
	QVariantList configurations = databaseToUpgrade_->retrieve(scanAxesTableName, "table1");
	QVariantList scanAxesIds = databaseToUpgrade_->retrieve(scanAxesTableName, "id2");
	QVariantList scanAxes = databaseToUpgrade_->retrieve(scanAxesTableName, "table2");

	QList<int> updateIds;

	foreach (QVariant id, ids)
		updateIds << id.toString().split(";").last().toInt();

	// Iterate through all entries in the named scan axes table, copying over matches to the named configuration table.

	for (int idIndex = 0, idIndexCount = ids.count(); idIndex < idIndexCount; idIndex++) {
		QString entry = configurations.at(idIndex).toString();

		if (entry == configurationTableName) {

			int newID = databaseToUpgrade_->insertOrUpdate(0, "BioXASXASScanConfiguration_table_scanAxes",
											   QStringList()
											   << "id1"
											   << "table1"
											   << "id2"
											   << "table2",
											   QVariantList()
											   << configurationIds.at(idIndex)
											   << "BioXASXASScanConfiguration_table"
											   << scanAxesIds.at(idIndex)
											   << scanAxes.at(idIndex)
											   );

			if (newID == 0) {
				databaseToUpgrade_->rollbackTransaction();
				AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_INSERT_OR_UPDATE_TABLE, QString("Could not copy entries from %1 to BioXASXASScanConfiguration_table_scanAxes.").arg(scanAxesTableName));
				return false;
			}
		}
	}

	AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_INSERT_OR_UPDATE_SUCCESSFUL, QString("Successfully copied entries from %1 to BioXASXASScanConfiguration_table_scanAxes!").arg(scanAxesTableName));

	return true;
}
