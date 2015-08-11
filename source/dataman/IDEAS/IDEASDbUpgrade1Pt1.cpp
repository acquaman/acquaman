#include "IDEASDbUpgrade1Pt1.h"

#include "util/AMErrorMonitor.h"

IDEASDbUpgrade1Pt1::IDEASDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

IDEASDbUpgrade1Pt1::~IDEASDbUpgrade1Pt1()
{

}

QStringList IDEASDbUpgrade1Pt1::upgradeFromTags() const
{
	return QStringList();
}

bool IDEASDbUpgrade1Pt1::upgradeNecessary() const
{
	return true;
}

bool IDEASDbUpgrade1Pt1::upgradeImplementation()
{
	// Getting started.
	databaseToUpgrade_->startTransaction();

	// Cleanup a bunch of old columns that have no purpose anymore.

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "startValue")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "startValue")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the startValue column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "endValue")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "endValue")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the endValue column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "minEnergy")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "minEnergy")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the minEnergy column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "maxEnergy")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "maxEnergy")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the maxEnergy column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "useFixedTime")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "useFixedTime")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the useFixedTime column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "regions")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "regions")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the regions column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "I0Channel")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "I0Channel")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the I0Channel column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "ItChannel")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "ItChannel")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the ItChannel column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "IrChannel")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "IrChannel")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the IrChannel column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "isXRFScan")
			&& !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "isXRFScan")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the isXRFScan column in IDEASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	// Make the new tables.  Only need two for this upgrade.

	QStringList names;
	names << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "energy" << "fluorescenceDetector";
	QStringList types;
	types << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "INTEGER";

	if (!databaseToUpgrade_->tableExists("IDEASScanConfigurationDbObject_table")
			&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_, "IDEASScanConfigurationDbObject_table", names, types, true, "IDEAS Scan Configuration Database Object", "IDEASScanConfigurationDbObject;AMDbObject;QObject")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_ADD_TABLE, "Could not add the IDEASScanConfigurationDbObject_table to the database.");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	names = QStringList() << "id1" << "table1" << "id2" << "table2";
	types = QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT";

	if (!databaseToUpgrade_->tableExists("IDEASScanConfigurationDbObject_table_regionsOfInterest")
			&& !databaseToUpgrade_->ensureTable("IDEASScanConfigurationDbObject_table_regionsOfInterest", names, types)){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_ADD_TABLE, "Could not add the IDEASScanConfigurationDbObject_table_regionsOfInterest to the database.");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	// Add the dbObject column to the XAS scan configuration table.

	if (!databaseToUpgrade_->columnExists("IDEASXASScanConfiguration_table", "configurationDbObject")
			&& !databaseToUpgrade_->ensureColumn("IDEASXASScanConfiguration_table", "configurationDbObject", "TEXT")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_CREATE_COLUMN, "Could no create the configurationDbObject column inside of IDEASXASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	// Fill the data.

	QVariantList ids = databaseToUpgrade_->retrieve("IDEASXASScanConfiguration_table", "id");
	QVariantList scanNames = databaseToUpgrade_->retrieve("IDEASXASScanConfiguration_table", "name");
	QVariantList energy = databaseToUpgrade_->retrieve("IDEASXASScanConfiguration_table", "edgeEnergy");
	QVariantList fluorescenceDetector = databaseToUpgrade_->retrieve("IDEASXASScanConfiguration_table", "fluorescenceDetector");

	for (int i = 0, size = ids.size(); i < size; i++){

		int id = databaseToUpgrade_->insertOrUpdate(0, "IDEASScanConfigurationDbObject_table",
													QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "energy" << "fluorescenceDetector",
													QVariantList()
													<< "IDEASScanConfigurationDbObject"
													<< 0
													<< QVariant()
													<< scanNames.at(i)
													<< energy.at(i)
													<< fluorescenceDetector.at(i));

		if (id == 0){

			AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_ADD_ROW, "Could not insert into IDEASScanConfigurationDbObject_table");
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		if (!databaseToUpgrade_->update(ids.at(i).toInt(), "IDEASXASScanConfiguration_table", "configurationDbObject", QString("IDEASScanConfigurationDbObject_table;%1").arg(id))){

			AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, "Could not update the configurationDbObject column in IDEASXASScanConfiguration_table");
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}
	}

	// Rename some columns.

	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "isTransScan", "usingTransmission")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_CHANGE_COLUMN_NAME, "Could not rename column isTransScan to usingTransmission in IDEASXASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "IDEASXASScanConfiguration_table", "useRef", "usingReference")){

		AMErrorMon::alert(this, IDEASDBUPGRADE1PT1_COULD_NOT_CHANGE_COLUMN_NAME, "Could not rename column useRef to usingReference in IDEASXASScanConfiguration_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	// Database cleanup.

	QList<int> idOfDbObject = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "tableName", "IDEASXASScanConfiguration_table");

	if (!idOfDbObject.isEmpty()){

		databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << idOfDbObject.first() << "configurationDbObject");
		databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << idOfDbObject.first() << "configurationDbObject");
		databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << idOfDbObject.first() << "configurationDbObject");
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade *IDEASDbUpgrade1Pt1::createCopy() const
{
	AMDbUpgrade *copy = new IDEASDbUpgrade1Pt1(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString IDEASDbUpgrade1Pt1::upgradeToTag() const
{
	return "IDEASDbUpgrade1.1";
}

QString IDEASDbUpgrade1Pt1::description() const
{
	return "Gives IDEAS an IDEASScanConfigurationDbObject and does all necessary data management to ensure that old scans can load with the new table scheme.";
}
