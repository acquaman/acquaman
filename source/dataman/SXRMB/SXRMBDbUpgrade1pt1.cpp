#include "SXRMBDbUpgrade1pt1.h"

#include "util/AMErrorMonitor.h"

SXRMBDbUpgrade1pt1::SXRMBDbUpgrade1pt1(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{

}

SXRMBDbUpgrade1pt1::~SXRMBDbUpgrade1pt1()
{

}

QStringList SXRMBDbUpgrade1pt1::upgradeFromTags() const
{
	return QStringList();
}

bool SXRMBDbUpgrade1pt1::upgradeNecessary() const
{
	return !databaseToUpgrade_->objectsMatching("AMScan_table", "AMDbObjectType", "AM2DScan").isEmpty()
			|| databaseToUpgrade_->objectsMatching("AMScan_table", "AMDbObjectType", "AMXASScan").isEmpty();
}

bool SXRMBDbUpgrade1pt1::upgradeImplementation()
{
	// Prologue.
	////////////////////////////////////////////////

	databaseToUpgrade_->startTransaction();

	// Stage 1:  Create/Modify all columns
	///////////////////////////////////////////////////

	if (!databaseToUpgrade_->ensureColumn("SXRMBScanConfigurationDbObject_table", "fluorescenceDetectors", "INTEGER")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_CREATE_COLUMN, "Could not create the fluorescenceDetectors column in SXRMBScanConfigurationDbObject_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!databaseToUpgrade_->ensureColumn("SXRMBScanConfigurationDbObject_table", "x", "REAL")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_CREATE_COLUMN, "Could not create the x column in SXRMBScanConfigurationDbObject_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!databaseToUpgrade_->ensureColumn("SXRMBScanConfigurationDbObject_table", "z", "REAL")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_CREATE_COLUMN, "Could not create the z column in SXRMBScanConfigurationDbObject_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!databaseToUpgrade_->ensureColumn("SXRMBScanConfigurationDbObject_table", "energy", "REAL")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_CREATE_COLUMN, "Could not create the energy column in SXRMBScanConfigurationDbObject_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "SXRMBScanConfigurationDbObject_table", "normalPosition", "y")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_CHANGE_COLUMN_NAME, "Could not change the normalPosition column name to y in SXRMBScanConfigurationDbObject_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	// Stage 2:  Filling all the data as required.
	/////////////////////////////////////////////////////

	// 2D
	QVariantList ids = QVariantList();
	QVariantList energies = QVariantList();
	QList<int> updateIds = QList<int>();

	ids = databaseToUpgrade_->retrieve("SXRMB2DMapScanConfiguration_table", "configurationDbObject");
	energies = databaseToUpgrade_->retrieve("SXRMB2DMapScanConfiguration_table", "excitationEnergy");

	foreach (QVariant id, ids)
		updateIds << id.toString().split(";").last().toInt();

	for (int i = 0, size = updateIds.size(); i < size; i++){

		if (!databaseToUpgrade_->update(updateIds.at(i), "SXRMBScanConfigurationDbObject_table", "energy", energies.at(i))){

			AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, QString("Could not update the energy column at %1 to %2 in SXRMBScanConfigurationDbObject_table").arg(updateIds.at(i)).arg(energies.at(i).toDouble()));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}
	}

	// EXAFS

	ids = QVariantList();
	energies = QVariantList();
	updateIds = QList<int>();
	QVariantList xPositions = QVariantList();
	QVariantList zPositions = QVariantList();

	ids = databaseToUpgrade_->retrieve("SXRMBEXAFSScanConfiguration_table", "configurationDbObject");
	energies = databaseToUpgrade_->retrieve("SXRMBEXAFSScanConfiguration_table", "edgeEnergy");
	xPositions = databaseToUpgrade_->retrieve("SXRMBEXAFSScanConfiguration_table", "microprobeSampleStageX");
	zPositions = databaseToUpgrade_->retrieve("SXRMBEXAFSScanConfiguration_table", "microprobeSampleStageZ");

	foreach (QVariant id, ids)
		updateIds << id.toString().split(";").last().toInt();

	for (int i = 0, size = updateIds.size(); i < size; i++){

		if (!databaseToUpgrade_->update(updateIds.at(i), "SXRMBScanConfigurationDbObject_table", "energy", energies.at(i))){

			AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, QString("Could not update the energy column at %1 to %2 in SXRMBScanConfigurationDbObject_table").arg(updateIds.at(i)).arg(energies.at(i).toDouble()));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		if (!databaseToUpgrade_->update(updateIds.at(i), "SXRMBScanConfigurationDbObject_table", "x", xPositions.at(i))){

			AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, QString("Could not update the x column at %1 to %2 in SXRMBScanConfigurationDbObject_table").arg(updateIds.at(i)).arg(xPositions.at(i).toDouble()));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		if (!databaseToUpgrade_->update(updateIds.at(i), "SXRMBScanConfigurationDbObject_table", "z", zPositions.at(i))){

			AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, QString("Could not update the z column at %1 to %2 in SXRMBScanConfigurationDbObject_table").arg(updateIds.at(i)).arg(zPositions.at(i).toDouble()));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}
	}

	// Db Object

	ids = QVariantList();
	updateIds = QList<int>();
	QVariantList brukerEnabled = QVariantList();

	ids = databaseToUpgrade_->retrieve("SXRMBScanConfigurationDbObject_table", "id");
	brukerEnabled = databaseToUpgrade_->retrieve("SXRMBScanConfigurationDbObject_table", "enableBrukerDetector");

	foreach (QVariant id, ids)
		updateIds << id.toString().split(";").last().toInt();

	for (int i = 0, size = updateIds.size(); i < size; i++){

		if (!databaseToUpgrade_->update(updateIds.at(i), "SXRMBScanConfigurationDbObject_table", "fluorescenceDetectors", brukerEnabled.at(i).toBool() ? QVariant(1) : QVariant(0))){

			AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, QString("Could not update the fluorescence column at %1 to %2 in SXRMBScanConfigurationDbObject_table").arg(updateIds.at(i)).arg(brukerEnabled.at(i).toInt()));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}
	}

	// Stage 3:  Removing old columns.
	/////////////////////////////////////////////////////

	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "SXRMBScanConfigurationDbObject_table", "enableBrukerDetector")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, QString("Could not remove the enableBrukerDetector column in SXRMBScanConfigurationDbObject_table"));
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "SXRMBEXAFSScanConfiguration_table", "edgeEnergy")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, QString("Could not remove the edgeEnergy column in SXRMBEXAFSScanConfiguration_table"));
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "SXRMBEXAFSScanConfiguration_table", "microprobeSampleStageX")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, QString("Could not remove the microprobeSampleStageX column in SXRMBEXAFSScanConfiguration_table"));
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "SXRMBEXAFSScanConfiguration_table", "microprobeSampleStageZ")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, QString("Could not remove the microprobeSampleStageZ column in SXRMBEXAFSScanConfiguration_table"));
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "SXRMB2DMapScanConfiguration_table", "excitationEnergy")){

		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, QString("Could not remove the excitationEnergy column in SXRMB2DScanConfiguration_table"));
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	// Epilogue.
	///////////////////////////////////////////////////////

	int configurationDbObjectId = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "SXRMBScanConfigurationDbObject").first();

	// First: allColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "fluorescenceDetectors") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "x") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "z") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "energy") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS, "Could not add the entries for SXRMBScanConfigurationDbObject to the allColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	// Second: visibleColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "fluorescenceDetectors") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "x") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "z") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "energy") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_VISIBLECOLUMNS, "Could not add the entries for SXRMBScanConfigurationDbObject to the visibleColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	// Third: loadColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "fluorescenceDetectors") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "x") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "z") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << configurationDbObjectId << "energy") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, SXRMBDBUPGRADE1PT1_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_LOADCOLUMNS, "Could not add the entries for SXRMBScanConfigurationDbObject to the loadColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade * SXRMBDbUpgrade1pt1::createCopy() const
{
	AMDbUpgrade *copy = new SXRMBDbUpgrade1pt1(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString SXRMBDbUpgrade1pt1::upgradeToTag() const
{
	return "SXRMBDbUpgrade1.1";
}

QString SXRMBDbUpgrade1pt1::description() const
{
	return "Upgrades the scan configuration tables by consolidating the meta information in the common scan configuration.";
}
