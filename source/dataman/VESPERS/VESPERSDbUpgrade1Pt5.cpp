#include "VESPERSDbUpgrade1Pt5.h"

#include "util/AMErrorMonitor.h"

VESPERSDbUpgrade1Pt5::VESPERSDbUpgrade1Pt5(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt5::upgradeFromTags() const
{
	return QStringList()
			<< "VESPERSDbUpgrade1.1"
			<< "VESPERSDbUpgrade1.2"
			<< "VESPERSDbUpgrade1.3"
			<< "VESPERSDbUpgrade1.4";
}

bool VESPERSDbUpgrade1Pt5::upgradeNecessary() const
{
	bool upgrade = true;

	// First: 2D scans.
	QSqlQuery query = databaseToUpgrade_->select("sqlite_master", "*", "tbl_name='VESPERS2DScanConfiguration_table'");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	// Second: Line scans.
	query = databaseToUpgrade_->select("sqlite_master", "*", "tbl_name='VESPERSSpatialLineScanConfiguration_table'");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	// Third: EXAFS scans.
	query = databaseToUpgrade_->select("sqlite_master", "*", "tbl_name='VESPERSEXAFSScanConfiguration_table'");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	// Fourth: Energy scans.
	query = databaseToUpgrade_->select("sqlite_master", "*", "tbl_name='VESPERSEnergyScanConfiguration_table'");
	databaseToUpgrade_->execQuery(query);
	upgrade &= (query.lastError().type() == QSqlError::NoError);

	return upgrade;
}

bool VESPERSDbUpgrade1Pt5::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	// Step 1)
	/////////////////////////////////////////
	QStringList names;
	names << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "incomingChoice" << "transmissionChoice" << "motor" << "fluorescenceDetector" << "ccdDetector" << "ccdFileName" << "roiInfoList";
	QStringList types;
	types << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "INTEGER" << "INTEGER" << "INTEGER" << "INTEGER" << "INTEGER" << "TEXT" << "TEXT";

	if (!databaseToUpgrade_->ensureTable("VESPERSScanConfigurationDbObject_table", names, types)){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_VESPERSSCANCONFIGURATIONDBOBJECT_TABLE, "Was unable to create the VESPERSScanConfigurationDbObject table.");
		return false;
	}

	// Step 2)
	////////////////////////////////////////////

	// Holds all of the values for all the columns.  For faster storage.
	QVariantList ids;
	QVariantList scanNames;
	QVariantList incomingValues;
	QVariantList transmissionValues;
	QVariantList motorValues;
	QVariantList xrfValues;
	QVariantList ccdValues;
	QVariantList fileNameValues;
	QVariantList roiValues;
	int newId;

	// First 2D.
	if (!databaseToUpgrade_->ensureColumn("VESPERS2DScanConfiguration_table", "configurationDbObject")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_COLUMN_VESPERS2DSCANCONFIGURATION_TABLE, "Was unable to create the configurationDbObject column in VESPERS2DScanConfiguration table.");
		return false;
	}

	QList<int> dbObjectIntList = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "tableName", "VESPERS2DScanConfiguration_table");

	if (!dbObjectIntList.isEmpty()){

		newId = dbObjectIntList.first();

		if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "configurationDbObject") == 0
				|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "configurationDbObject") == 0
				){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_2DCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS_OR_LOADCOLUMNS, "Could not add the configurationDbObject to the associated tables allColumns and/or loadColumns of AMDbObjectTypes for VESPERS2DScanConfiguration.");
			return false;
		}
	}

	ids = databaseToUpgrade_->retrieve("VESPERS2DScanConfiguration_table", "id");
	scanNames = databaseToUpgrade_->retrieve("VESPERS2DScanConfiguration_table", "name");
	incomingValues = databaseToUpgrade_->retrieve("VESPERS2DScanConfiguration_table", "incomingChoice");
	motorValues = databaseToUpgrade_->retrieve("VESPERS2DScanConfiguration_table", "motor");
	xrfValues = databaseToUpgrade_->retrieve("VESPERS2DScanConfiguration_table", "fluorescenceDetector");
	ccdValues = databaseToUpgrade_->retrieve("VESPERS2DScanConfiguration_table", "ccdDetector");
	fileNameValues = databaseToUpgrade_->retrieve("VESPERS2DScanConfiguration_table", "ccdFileName");
	roiValues = databaseToUpgrade_->retrieve("VESPERS2DScanConfiguration_table", "roiInfoList");

	// All columns will have the same size.  Only need to do work if there are values to copy over.
	if (!ids.isEmpty()){

		for (int i = 0, size = ids.size(); i < size; i++){

			newId = databaseToUpgrade_->insertOrUpdate(0, "VESPERSScanConfigurationDbObject_table", names,
											   QVariantList()
													   << "VESPERSScanConfigurationDbObject"
													   << 0
													   << QVariant()
													   << scanNames.at(i)
													   << incomingValues.at(i)
													   << 3
													   << motorValues.at(i)
													   << xrfValues.at(i)
													   << ccdValues.at(i)
													   << fileNameValues.at(i)
													   << roiValues.at(i));

			if (newId == 0 || !databaseToUpgrade_->update(ids.at(i).toInt(), "VESPERS2DScanConfiguration_table", "configurationDbObject", QString("VESPERSScanConfigurationDbObject_table;%1").arg(newId))){

				databaseToUpgrade_->rollbackTransaction();
				AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_COPY_DATA_TO_NEW_TABLE_FROM_VESPERS2DSCANCONFIGURATION_TABLE, "Was unable to copy the data from VESPERS2DScanConfiguration columns to the new VESPERSScanConfigurationDbObject table.");
				return false;
			}
		}
	}

	// Second Spatial Line scan.
	if (!databaseToUpgrade_->ensureColumn("VESPERSSpatialLineScanConfiguration_table", "configurationDbObject")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_COLUMN_VESPERSSPATIALLINESCANCONFIGURATION_TABLE, "Was unable to create the configurationDbObject column in VESPERSSpatialLineScanConfiguration table.");
		return false;
	}

	dbObjectIntList = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "tableName", "VESPERSSpatialLineScanConfiguration_table");

	if (!dbObjectIntList.isEmpty()){

		newId = dbObjectIntList.first();

		if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "configurationDbObject") == 0
				|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "configurationDbObject") == 0
				){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_SPATIALLINECONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS_OR_LOADCOLUMNS, "Could not add the configurationDbObject to the associated tables allColumns and/or loadColumns of AMDbObjectTypes for VESPERSSpatialLineScanConfiguration.");
			return false;
		}
	}

	ids = databaseToUpgrade_->retrieve("VESPERSSpatialLineScanConfiguration_table", "id");
	scanNames = databaseToUpgrade_->retrieve("VESPERSSpatialLineScanConfiguration_table", "name");
	incomingValues = databaseToUpgrade_->retrieve("VESPERSSpatialLineScanConfiguration_table", "incomingChoice");
	motorValues = databaseToUpgrade_->retrieve("VESPERSSpatialLineScanConfiguration_table", "motor");
	xrfValues = databaseToUpgrade_->retrieve("VESPERSSpatialLineScanConfiguration_table", "fluorescenceDetector");
	ccdValues = databaseToUpgrade_->retrieve("VESPERSSpatialLineScanConfiguration_table", "ccdDetector");
	fileNameValues = databaseToUpgrade_->retrieve("VESPERSSpatialLineScanConfiguration_table", "ccdFileName");
	roiValues = databaseToUpgrade_->retrieve("VESPERSSpatialLineScanConfiguration_table", "roiInfoList");

	// All columns will have the same size.  Only need to do work if there are values to copy over.
	if (!ids.isEmpty()){

		for (int i = 0, size = ids.size(); i < size; i++){

			newId = databaseToUpgrade_->insertOrUpdate(0, "VESPERSScanConfigurationDbObject_table", names,
											   QVariantList()
													   << "VESPERSScanConfigurationDbObject"
													   << 0
													   << QVariant()
													   << scanNames.at(i)
													   << incomingValues.at(i)
													   << 3
													   << motorValues.at(i)
													   << xrfValues.at(i)
													   << ccdValues.at(i)
													   << fileNameValues.at(i)
													   << roiValues.at(i));

			if (newId == 0 || !databaseToUpgrade_->update(ids.at(i).toInt(), "VESPERSSpatialLineScanConfiguration_table", "configurationDbObject", QString("VESPERSScanConfigurationDbObject_table;%1").arg(newId))){

				databaseToUpgrade_->rollbackTransaction();
				AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_COPY_DATA_TO_NEW_TABLE_FROM_VESPERSSPATIALLINESCANCONFIGURATION_TABLE, "Was unable to copy the data from VESPERSSpatialLineScanConfiguration columns to the new VESPERSScanConfigurationDbObject table.");
				return false;
			}
		}
	}

	// Third EXAFS.
	if (!databaseToUpgrade_->ensureColumn("VESPERSEXAFSScanConfiguration_table", "configurationDbObject")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_COLUMN_VESPERSEXAFSSCANCONFIGURATION_TABLE, "Was unable to create the configurationDbObject column in VESPERSEXAFSScanConfiguration table.");
		return false;
	}

	dbObjectIntList = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "tableName", "VESPERSEXAFSScanConfiguration_table");

	if (!dbObjectIntList.isEmpty()){

		newId = dbObjectIntList.first();

		if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "configurationDbObject") == 0
				|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "configurationDbObject") == 0
				){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_EXAFSCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS_OR_LOADCOLUMNS, "Could not add the configurationDbObject to the associated tables allColumns and/or loadColumns of AMDbObjectTypes for VESPERSEXAFSScanConfiguration.");
			return false;
		}
	}

	ids = databaseToUpgrade_->retrieve("VESPERSEXAFSScanConfiguration_table", "id");
	scanNames = databaseToUpgrade_->retrieve("VESPERSEXAFSScanConfiguration_table", "name");
	incomingValues = databaseToUpgrade_->retrieve("VESPERSEXAFSScanConfiguration_table", "incomingChoice");
	transmissionValues = databaseToUpgrade_->retrieve("VESPERSEXAFSScanConfiguration_table", "transmissionChoice");
	xrfValues = databaseToUpgrade_->retrieve("VESPERSEXAFSScanConfiguration_table", "fluorescenceDetector");
	roiValues = databaseToUpgrade_->retrieve("VESPERSEXAFSScanConfiguration_table", "roiInfoList");

	// All columns will have the same size.  Only need to do work if there are values to copy over.
	if (!ids.isEmpty()){

		for (int i = 0, size = ids.size(); i < size; i++){

			newId = databaseToUpgrade_->insertOrUpdate(0, "VESPERSScanConfigurationDbObject_table", names,
											   QVariantList()
													   << "VESPERSScanConfigurationDbObject"
													   << 0
													   << QVariant()
													   << scanNames.at(i)
													   << incomingValues.at(i)
													   << transmissionValues.at(i)
													   << 0
													   << xrfValues.at(i)
													   << 0
													   << ""
													   << roiValues.at(i));

			if (newId == 0 || !databaseToUpgrade_->update(ids.at(i).toInt(), "VESPERSEXAFSScanConfiguration_table", "configurationDbObject", QString("VESPERSScanConfigurationDbObject_table;%1").arg(newId))){

				databaseToUpgrade_->rollbackTransaction();
				AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_COPY_DATA_TO_NEW_TABLE_FROM_VESPERSEXAFSSCANCONFIGURATION_TABLE, "Was unable to copy the data from VESPERSEXAFSScanConfiguration columns to the new VESPERSScanConfigurationDbObject table.");
				return false;
			}
		}
	}

	// Fourth Energy.
	if (!databaseToUpgrade_->ensureColumn("VESPERSEnergyScanConfiguration_table", "configurationDbObject")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_COLUMN_VESPERSENERGYSCANCONFIGURATION_TABLE, "Was unable to create the configurationDbObject column in VESPERSEnergyScanConfiguration table.");
		return false;
	}

	dbObjectIntList = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "tableName", "VESPERSEnergyScanConfiguration_table");

	if (!dbObjectIntList.isEmpty()){

		newId = dbObjectIntList.first();

		if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "configurationDbObject") == 0
				|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "configurationDbObject") == 0
				){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_ENERGYCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS_OR_LOADCOLUMNS, "Could not add the configurationDbObject to the associated tables allColumns and/or loadColumns of AMDbObjectTypes for VESPERSEnergyScanConfiguration.");
			return false;
		}
	}

	ids = databaseToUpgrade_->retrieve("VESPERSEnergyScanConfiguration_table", "id");
	scanNames = databaseToUpgrade_->retrieve("VESPERSEnergyScanConfiguration_table", "name");
	ccdValues = databaseToUpgrade_->retrieve("VESPERSEnergyScanConfiguration_table", "ccdDetector");
	fileNameValues = databaseToUpgrade_->retrieve("VESPERSEnergyScanConfiguration_table", "ccdFileName");

	// All columns will have the same size.  Only need to do work if there are values to copy over.
	if (!ids.isEmpty()){

		QStringList columns = QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "description";
		QVariantList values = QVariantList() << "AMROIInfoList" << 0 << "" << "Unnamed Object" << "";

		for (int i = 0, size = ids.size(); i < size; i++){

			newId = databaseToUpgrade_->insertOrUpdate(0, "AMROIInfoList_table", columns, values);
			newId = databaseToUpgrade_->insertOrUpdate(0, "VESPERSScanConfigurationDbObject_table", names,
												QVariantList()
													   << "VESPERSScanConfigurationDbObject"
													   << 0
													   << QVariant()
													   << scanNames.at(i)
													   << 2
													   << 3
													   << 0
													   << 0
													   << ccdValues.at(i)
													   << fileNameValues.at(i)
													   << QString("AMROIInfoList_table;%1").arg(newId));

			if (newId == 0 || !databaseToUpgrade_->update(ids.at(i).toInt(), "VESPERSEnergyScanConfiguration_table", "configurationDbObject", QString("VESPERSScanConfigurationDbObject_table;%1").arg(newId))){

				databaseToUpgrade_->rollbackTransaction();
				AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_COPY_DATA_TO_NEW_TABLE_FROM_VESPERSENERGYSCANCONFIGURATION_TABLE, "Was unable to copy the data from VESPERSEnergyScanConfiguration columns to the new VESPERSScanConfigurationDbObject table.");
				return false;
			}
		}
	}

	databaseToUpgrade_->commitTransaction();

	// Step 3)
	/////////////////////////////////////

	// First: 2D
	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "incomingChoice")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "fluorescenceDetector")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "ccdDetector")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "ccdFileName")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "motor")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "roiInfoList")
			){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS_VESPERS2DSCANCONFIGURATION_TABLE, "Was unable to remove old columns from VESPERS2DScanConfiguration table.");
		return false;
	}

	// Second: Line
	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "incomingChoice")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "fluorescenceDetector")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "ccdDetector")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "ccdFileName")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "motor")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "roiInfoList")
			){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS_VESPERSSPATIALLINESCANCONFIGURATION_TABLE, "Was unable to remove old columns from VESPERSSpatialLineScanConfiguration table.");
		return false;
	}

	// Third: EXAFS
	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSEXAFSScanConfiguration_table", "incomingChoice")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSEXAFSScanConfiguration_table", "transmissionChoice")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSEXAFSScanConfiguration_table", "fluorescenceDetector")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSEXAFSScanConfiguration_table", "roiInfoList")
			){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS_VESPERSEXAFSSCANCONFIGURATION_TABLE, "Was unable to remove old columns from VESPERSEXAFSScanConfiguration table.");
		return false;
	}

	// Fourth: Energy
	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSEnergyScanConfiguration_table", "ccdDetector")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSEnergyScanConfiguration_table", "ccdFileName")
			){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS_VESPERSENERGYSCANCONFIGURATION_TABLE, "Was unable to remove old columns from VESPERSEnergyScanConfiguration table.");
		return false;
	}

	// 4)
	///////////////////////////////////////////

	if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "rois")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "rois")
			|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSEXAFSScanConfiguration_table", "rois")
			){

		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_ALL_INSTANCES_ROIS_COLUMN, "Was unable to remove all instances of the rois column from VESPERS2DScanConfiguration table, VESPERSSpatialLineScanConfiguration table, and VEPSERSEXAFSScanConfiguration table.");
		return false;
	}

	// 5)
	//////////////////////////////////////////

	databaseToUpgrade_->startTransaction();
	QStringList dboColumnNames = QStringList() << "AMDbObjectType" << "tableName" << "description" << "version" << "inheritance";
	QVariantList dboValues = QVariantList() << "VESPERSScanConfigurationDbObject" << "VESPERSScanConfigurationDbObject_table" << "VESPERS Scan Configuration Database Object" << 1 << "VESPERSScanConfigurationDbObject;AMDbObject;QObject";
	newId = databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_table", dboColumnNames, dboValues);

	if (newId == 0){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRY_VESPERSSCANCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE, "Could not add the new VESPERSScanConfigurationDbObject entry to the AMDbObjectTypes table.");
		return false;
	}

	// 6)
	//////////////////////////////////////////

	// First: allColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "name") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "incomingChoice") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "transmissionChoice") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "motor") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "fluorescenceDetector") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "ccdDetector") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "ccdFileName") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "roiInfoList") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS, "Could not add the entries for VESPERSScanConfigurationDbObject to the allColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	// Second: visibleColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "name") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "incomingChoice") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "transmissionChoice") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "motor") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "fluorescenceDetector") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "ccdDetector") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "ccdFileName") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_VISIBLECOLUMNS, "Could not add the entries for VESPERSScanConfigurationDbObject to the visibleColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	// Third: loadColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "name") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "incomingChoice") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "transmissionChoice") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "motor") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "fluorescenceDetector") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "ccdDetector") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "ccdFileName") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "roiInfoList") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_LOADCOLUMNS, "Could not add the entries for VESPERSScanConfigurationDbObject to the loadColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt5::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt5(this->databaseNameToUpgrade());

	if (this->databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt5::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.5";
}

QString VESPERSDbUpgrade1Pt5::description() const
{
	return "Upgrades database to use new VESPERScanConfiguration where many aspects are consolidated in a new database object.";
}
