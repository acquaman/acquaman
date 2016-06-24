#include "CLSDbUpgrade1Pt3.h"

#include <QStringBuilder>

#include "util/AMErrorMonitor.h"

CLSDbUpgrade1Pt3::CLSDbUpgrade1Pt3(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

CLSDbUpgrade1Pt3::~CLSDbUpgrade1Pt3()
{

}

QStringList CLSDbUpgrade1Pt3::upgradeFromTags() const
{
	return QStringList();
}

bool CLSDbUpgrade1Pt3::upgradeNecessary() const
{
	// run update when:
	//
	//   - there is no CLSUserConfiguration_table and no CLSUserConfiguration_table_regionsOfInterest

	bool tableExists = databaseToUpgrade_->tableExists("CLSUserConfiguration_table");

	return !tableExists;
}

bool CLSDbUpgrade1Pt3::upgradeImplementation()
{
	// Prologue.
	////////////////////////////////////////////////

	databaseToUpgrade_->startTransaction();

	// Stage 1:  create table: CLSUserConfiguration_table and CLSUserConfiguration_table_regionsOfInterest
	///////////////////////////////////////////////////

	QString clsConfigurationTableName = "CLSUserConfiguration_table";
	QString clsUserConfigurationClassInheritence = "CLSUserConfiguration;AMDbObject;QObject";

	if ( !databaseToUpgrade_->tableExists(clsConfigurationTableName) ) {
		QStringList columnNames = QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name";
		QStringList columnTypes = QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT";

		if (!AMDbUpgradeSupport::addTable(databaseToUpgrade_, clsConfigurationTableName, columnNames, columnTypes, true, "CLS User Configuration", clsUserConfigurationClassInheritence)) {
			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, CLSDBUPGRADE1PT3_COULD_NOT_CREATE_TABLE, QString("Could not create %1.").arg(clsConfigurationTableName));
			return false;
		}

		QString configurationROITableName = QString("%1_regionsOfInterest").arg(clsConfigurationTableName);
		QStringList ROITableColumnNames = QStringList() << "id1" << "table1" << "id2" << "table2";
		QStringList ROITableColumnTypes = QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT";

		if (!databaseToUpgrade_->ensureTable(configurationROITableName, ROITableColumnNames, ROITableColumnTypes)) {
			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, CLSDBUPGRADE1PT3_COULD_NOT_CREATE_TABLE, QString("Could not create %1.").arg(configurationROITableName));
			return false;
		}

		QStringList indexColumns = QStringList() << "id1" << "id2" ;
		foreach (QString column, indexColumns) {
			if (!databaseToUpgrade_->createIndex(configurationROITableName, column)) {
				databaseToUpgrade_->rollbackTransaction();
				AMErrorMon::alert(this, CLSDBUPGRADE1PT3_COULD_NOT_CREATE_INDEX, QString("Could not create index for %1 on %2.").arg(configurationROITableName).arg(column));
				return false;
			}
		}
	}

	// Stage 2:  update the inheritance of the beamline UserConfiguration
	///////////////////////////////////////////////////

	QString tableClassNameTemplate = "%1UserConfiguration";
	QString tableDescriptionTemplate = "%1 User Configuration Database Object";

	QStringList dboColumnNames = QStringList() << "AMDbObjectType" << "tableName" << "description" << "version" << "inheritance";
	QVariantList dboValues ;
	QString tableClassName;

	// ------ find the beamline information and update the inheritence information ------
	QStringList beamlinesToCheck = QStringList() << "BioXAS" << "IDEAS" << "PGM" << "SGM" << "SXRMB" << "VESPERS";

	foreach (QString beamlineName, beamlinesToCheck) {
		tableClassName = tableClassNameTemplate.arg(beamlineName);

		QVariantList dboClassNames = databaseToUpgrade_->retrieve("AMDbObjectTypes_table", "AMDbObjectType");
		if (dboClassNames.indexOf(tableClassName) >= 0) {

			dboValues = QVariantList() << tableClassName << QString("%1_table").arg(tableClassName) << tableDescriptionTemplate.arg(beamlineName)
									   << 1 << QString("%1;%2").arg(tableClassName).arg(clsUserConfigurationClassInheritence);

			int dbResult = databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_table", dboColumnNames, dboValues);
			if (dbResult == 0) {
				databaseToUpgrade_->rollbackTransaction();
				AMErrorMon::alert(this, CLSDBUPGRADE1PT3_COULD_NOT_INSERT_OR_UPDATE_TABLE, QString("Could not update AMDbObjectTypes_table with new configurations for %1.").arg(beamlineName));
				return false;
			}
		}
	}


	// Epilogue.
	///////////////////////////////////////////////////////
	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade * CLSDbUpgrade1Pt3::createCopy() const
{
	AMDbUpgrade *copy = new CLSDbUpgrade1Pt3(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString CLSDbUpgrade1Pt3::upgradeToTag() const
{
	return "CLSDbUpgrade1.3";
}

QString CLSDbUpgrade1Pt3::description() const
{
	return QString("Create the CLSUserConfiguration_table and CLSUserConfiguration_table_regionsOfInterest and update the inheritance information of the beamline user configurations.");
}
