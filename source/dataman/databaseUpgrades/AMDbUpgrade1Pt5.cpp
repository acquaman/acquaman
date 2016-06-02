#include "AMDbUpgrade1Pt5.h"

#include "util/AMErrorMonitor.h"

#include <QStringBuilder>
#include <QMap>

AMDbUpgrade1Pt5::AMDbUpgrade1Pt5(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList AMDbUpgrade1Pt5::upgradeFromTags() const
{
	return QStringList() << "AMDbUpgrade1.4";
}

bool AMDbUpgrade1Pt5::upgradeNecessary() const
{
	return true;
}

bool AMDbUpgrade1Pt5::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	// Doing AMRegionScanConfiguration first.

	// Get all the scan configurations that have AMRegionScanConfiguration as part of their inheritance (region, XAS, EXAFS).
	QSqlQuery query = databaseToUpgrade_->query();
	query.prepare("SELECT id, inheritance FROM AMDbObjectTypes_table WHERE inheritance LIKE '%AMRegionScanConfiguration%';");

	if (!databaseToUpgrade_->execQuery(query)){

		query.finish();
		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, AMDBUPGRADE1PT5_REGION_CONFIGURATION_QUERY_FAILED, "Query to find AMRegionScanConfigurations failed.");
		return false;
	}

	QStringList scanConfigurations = QStringList();
	QList<int> scanConfigurationIds = QList<int>();

	if (query.first()){

		do {

			scanConfigurationIds << query.value(0).toInt();
			scanConfigurations << query.value(1).toString();
		} while (query.next());
	}

	query.finish();

	if (!scanConfigurations.isEmpty()){

		// Create all the tables that are associated with AMStepScanConfiguration.
		if (!databaseToUpgrade_->tableExists("AMStepScanConfiguration_table")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMStepScanConfiguration_table",
												 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "detectorConfigurations" << "axisControlInfos",
												 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "TEXT" << "TEXT",
												 true,
												 "Base configuration for step axis scans.",
												 "AMStepScanConfiguration;AMScanConfiguration;AMDbObject;QObject")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMStepScanConfiguration_table table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMStepScanConfiguration_table_scanAxes")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMStepScanConfiguration_table_scanAxes",
												 QStringList() << "id1" << "table1" << "id2" << "table2",
												 QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMStepScanConfiguration_table_scanAxes table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxis_table")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMScanAxis_table",
												 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "axisType",
												 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "INTEGER",
												 true,
												 "Describes a single axis of an n-dimensional scan.",
												 "AMScanAxis;AMDbObject;QObject")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMScanAxis_table table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxis_table_axisRegions")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMScanAxis_table_axisRegions",
												 QStringList() << "id1" << "table1" << "id2" << "table2",
												 QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMScanAxis_table_axisRegions table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxisRegion_table")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMScanAxisRegion_table",
												 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime",
												 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "REAL" << "REAL" << "REAL",
												 true,
												 "Defines a single region that can make up an axis.",
												 "AMScanAxisRegion;AMDbObject;QObject")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMScanAxisRegion_table table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxisEXAFSRegion_table")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMScanAxisEXAFSRegion_table",
												 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime" << "inKSpace" << "edgeEnergy" << "maximumTime" << "a2" << "equation",
												 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "REAL" << "REAL" << "REAL" << "TEXT" << "REAL" << "REAL" << "REAL" << "INTEGER",
												 true,
												 "Specific EXAFS region for EXAFS scan axes.",
												 "AMScanAxisEXAFSRegion;AMDbObject;QObject")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMScanAxisEXAFSRegion_table table.");
			return false;
		}

		QStringList tablesToModify;

		foreach (QString configuration, scanConfigurations)
			tablesToModify << configuration.split(";").first();

		foreach (QString table, tablesToModify){

			// Get all ids and region strings.
			QVariantList ids = databaseToUpgrade_->retrieve(table % "_table", "id");
			QVariantList regions = databaseToUpgrade_->retrieve(table % "_table", "regions");

			if (!ids.isEmpty()){

				// Find out if there are they are EXAFS or not.
				bool areEXAFSRegions = regions.first().toString().contains("exafsVersion1.0");

				// Create the associated table for the specific scan configuration.
				if (!databaseToUpgrade_->tableExists(table % "_table_scanAxes")
						&& !databaseToUpgrade_->ensureTable(table % "_table_scanAxes",
															QStringList() << "id1" << "table1" << "id2" << "table2",
															QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT")){

					databaseToUpgrade_->rollbackTransaction();
					AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, QString("Failed to create the %1_table table.").arg(table));
					return false;
				}

				// Add each region to either the scan axis regions table or the scan axis EXAFS regions table.
				for (int i = 0, regionsSize = regions.size(); i < regionsSize; i++){

					QString scanAxisName = axisName(QString("%1_table").arg(table), ids.at(i).toInt());
					int scanAxisId = databaseToUpgrade_->insertOrUpdate(0,
																		"AMScanAxis_table",
																		QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "axisType",
																		QVariantList() << "AMScanAxis" << 0 << QVariant() << scanAxisName << 0);

					if (scanAxisId == 0){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS, QString("Could not add the %1 axis.").arg(scanAxisName));
						return false;
					}

					if (!databaseToUpgrade_->insertOrUpdate(0,
															table % "_table_scanAxes",
															QStringList() << "id1" << "table1" << "id2" << "table2",
															QVariantList() << ids.at(i).toInt() << QString("%1_table").arg(table) << scanAxisId << "AMScanAxis_table")){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_TO_SCAN_CONFIGURATION_ASSOCIATED_TABLE, QString("Could not add to %1's scan axis associated table.").arg(table));
						return false;
					}

					QMap<QString, int> regionIds = QMap<QString, int>();
					QStringList configurationRegions = regions.at(i).toString().split("\n");

					if (!areEXAFSRegions){

						foreach (QString region, configurationRegions){

							QStringList regionComponents = region.split(",");

							if (regionComponents.size() == 3){

								int newRegionId = databaseToUpgrade_->insertOrUpdate(0,
																					 "AMScanAxisRegion_table",
																					 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime",
																					 QVariantList() << "AMScanAxisRegion" << 0 << QVariant() << QString("%1 region %2").arg(scanAxisName).arg(regionIds.size()+1) << regionComponents.at(0).toDouble() << regionComponents.at(1).toDouble() << regionComponents.at(2).toDouble() << 1.0);

								if (newRegionId == 0){

									databaseToUpgrade_->rollbackTransaction();
									AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS_REGION, QString("Could not add %1 region %2 to AMScanAxisRegion_table.").arg(scanAxisName).arg(regionIds.size()+1));
									return false;
								}

								regionIds.insert("AMScanAxisRegion_table", newRegionId);
							}

							else if (regionComponents.size() == 7 || regionComponents.size() == 9){

								int newRegionId = databaseToUpgrade_->insertOrUpdate(0,
																					 "AMScanAxisRegion_table",
																					 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime",
																					 QVariantList() << "AMScanAxisRegion" << 0 << QVariant() << QString("%1 region %2").arg(scanAxisName).arg(regionIds.size()+1) << regionComponents.at(1).toDouble() << regionComponents.at(2).toDouble() << regionComponents.at(3).toDouble() << regionComponents.at(6).toDouble());

								if (newRegionId == 0){

									databaseToUpgrade_->rollbackTransaction();
									AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS_REGION, QString("Could not add %1 region %2 to AMScanAxisRegion_table.").arg(scanAxisName).arg(regionIds.size()+1));
									return false;
								}

								regionIds.insert("AMScanAxisRegion_table", newRegionId);
							}
						}
					}

					else{

						for (int i = 0, size = configurationRegions.size(); i < size; i++){

							QString region = configurationRegions.at(i);
							QStringList regionComponents = region.split(",");

							// Some specifics for EXAFS regions.
							double time = -1.0;
							QString isKSpace = "";
							double maximumTime = -1.0;
							double edgeEnergy = -1.0;

							// If an EXAFS region, then we need data from other regions.
							if (regionComponents.at(i).toInt() == 1){

								time = (i == 0 ? 1.0 : configurationRegions.at(i-1).split(",").at(9).toDouble());
								isKSpace = "true";
								maximumTime = regionComponents.at(9).toDouble();
								edgeEnergy = regionComponents.at(2).toDouble();
							}

							else {

								time = regionComponents.at(9).toDouble();
								isKSpace = "false";
								maximumTime = -1.0;
								edgeEnergy = regionComponents.at(2).toDouble();
							}

							int newRegionId = databaseToUpgrade_->insertOrUpdate(0,
																				 "AMScanAxisEXAFSRegion_table",
																				 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime" << "inKSpace" << "edgeEnergy" << "maximumTime" << "a2" << "equation",
																				 QVariantList() << "AMScanAxisEXAFSRegion" << 0 << QVariant() << QString("%1 region %2").arg(scanAxisName).arg(regionIds.size()+1) << regionComponents.at(4).toDouble() << regionComponents.at(5).toDouble() << regionComponents.at(6).toDouble() << regionComponents.at(9).toDouble() << isKSpace << edgeEnergy << maximumTime << 2.0 << 3);

							if (newRegionId == 0){

								databaseToUpgrade_->rollbackTransaction();
								AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS_EXAFS_REGION, QString("Could not add %1 region %2 to AMScanAxisEXAFSRegion_table.").arg(scanAxisName).arg(regionIds.size()+1));
								return false;
							}

							regionIds.insert("AMScanAxisEXAFSRegion_table", newRegionId);
						}
					}

					for (int i = 0, size = regionIds.size(); i < size; i++){

						if (!databaseToUpgrade_->insertOrUpdate(0,
																"AMScanAxis_table_axisRegions",
																QStringList() << "id1" << "table1" << "id2" << "table2",
																QVariantList() << scanAxisId << "AMScanAxis_table" << regionIds.values().at(i) << regionIds.keys().at(i))){

							databaseToUpgrade_->rollbackTransaction();
							AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_TO_SCAN_AXIS_ASSOCIATED_TABLE, QString("Could not add to AMScanAxis_table_axisRegions's associated table."));
							return false;
						}
					}
				}

				if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "regions")
						|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "startValue")
						|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "endValue")){

					databaseToUpgrade_->rollbackTransaction();
					AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS, QString("Could not remove one or more of 'regions', 'startValue', or 'endValue' columns from %1.").arg(table));
					return false;
				}

				if (!databaseToUpgrade_->update(scanConfigurationIds.at(tablesToModify.indexOf(table)),
												"AMDbObjectTypes_table",
												"inheritance",
												QString("%1;AMStepScanConfiguration;AMScanConfiguration;AMDbObject;QObject").arg(table))){

					databaseToUpgrade_->rollbackTransaction();
					AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_UPDATE_INHERITANCE, QString("Could not update %1's inheritance.").arg(table));
					return false;
				}
			}
		}
	}

	// Doing AM2DScanConfiguration.

	scanConfigurations = QStringList();
	scanConfigurationIds = QList<int>();

	query.prepare("SELECT id, inheritance FROM AMDbObjectTypes_table WHERE inheritance LIKE '%AM2DScanConfiguration%';");

	if (!databaseToUpgrade_->execQuery(query)){

		query.finish();
		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, AMDBUPGRADE1PT5_2D_CONFIGURATION_QUERY_FAILED, "Query to find AM2DScanConfigurations failed.");
		return false;
	}

	if (query.first()){

		do {

			scanConfigurationIds << query.value(0).toInt();
			scanConfigurations << query.value(1).toString();
		} while (query.next());
	}

	query.finish();

	if (!scanConfigurations.isEmpty()){

		// Create all the tables that are associated with AMStepScanConfiguration.
		if (!databaseToUpgrade_->tableExists("AMStepScanConfiguration_table")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMStepScanConfiguration_table",
												 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "detectorConfigurations" << "axisControlInfos",
												 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "TEXT" << "TEXT",
												 true,
												 "Base configuration for step axis scans.",
												 "AMStepScanConfiguration;AMScanConfiguration;AMDbObject;QObject")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMStepScanConfiguration_table table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMStepScanConfiguration_table_scanAxes")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMStepScanConfiguration_table_scanAxes",
												 QStringList() << "id1" << "table1" << "id2" << "table2",
												 QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMStepScanConfiguration_table_scanAxes table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxis_table")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMScanAxis_table",
												 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "axisType",
												 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "INTEGER",
												 true,
												 "Describes a single axis of an n-dimensional scan.",
												 "AMScanAxis;AMDbObject;QObject")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMScanAxis_table table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxis_table_axisRegions")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMScanAxis_table_axisRegions",
												 QStringList() << "id1" << "table1" << "id2" << "table2",
												 QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMScanAxis_table_axisRegions table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxisRegion_table")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMScanAxisRegion_table",
												 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime",
												 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "REAL" << "REAL" << "REAL",
												 true,
												 "Defines a single region that can make up an axis.",
												 "AMScanAxisRegion;AMDbObject;QObject")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMScanAxisRegion_table table.");
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxisEXAFSRegion_table")
				&& !AMDbUpgradeSupport::addTable(databaseToUpgrade_,
												 "AMScanAxisEXAFSRegion_table",
												 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime" << "inKSpace" << "edgeEnergy" << "maximumTime" << "a2" << "equation",
												 QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "REAL" << "REAL" << "REAL" << "TEXT" << "REAL" << "REAL" << "REAL" << "INTEGER",
												 true,
												 "Specific EXAFS region for EXAFS scan axes.",
												 "AMScanAxisEXAFSRegion;AMDbObject;QObject")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE, "Failed to create the AMScanAxisEXAFSRegion_table table.");
			return false;
		}

		QStringList tablesToModify;

		foreach (QString configuration, scanConfigurations)
			tablesToModify << configuration.split(";").first();

		foreach (QString table, tablesToModify){

			// Get all ids and region strings.
			QVariantList ids = databaseToUpgrade_->retrieve(table % "_table", "id");
			QVariantList xStarts = databaseToUpgrade_->retrieve(table % "_table", "xStart");
			QVariantList xSteps = databaseToUpgrade_->retrieve(table % "_table", "xStep");
			QVariantList xEnds = databaseToUpgrade_->retrieve(table % "_table", "xEnd");
			QVariantList yStarts = databaseToUpgrade_->retrieve(table % "_table", "yStart");
			QVariantList ySteps = databaseToUpgrade_->retrieve(table % "_table", "yStep");
			QVariantList yEnds = databaseToUpgrade_->retrieve(table % "_table", "yEnd");
			QVariantList timeSteps = databaseToUpgrade_->retrieve(table % "_table", "timeStep");

			if (!ids.isEmpty()){

				// Create the associated table for the specific scan configuration.
				if (!databaseToUpgrade_->tableExists(table % "_table_scanAxes")
						&& !databaseToUpgrade_->ensureTable(table % "_table_scanAxes",
															QStringList() << "id1" << "table1" << "id2" << "table2",
															QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT")){

					databaseToUpgrade_->rollbackTransaction();
					return false;
				}

				// Add each region to either the scan axis regions table or the scan axis EXAFS regions table.
				for (int i = 0, configurationSize = ids.size(); i < configurationSize; i++){

					// First axis.
					QString scanAxisName = "H";
					int scanAxisId = databaseToUpgrade_->insertOrUpdate(0,
																		"AMScanAxis_table",
																		QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "axisType",
																		QVariantList() << "AMScanAxis" << 0 << QVariant() << scanAxisName << 0);

					if (scanAxisId == 0){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS, QString("Could not add the %1 axis.").arg(scanAxisName));
						return false;
					}

					if (!databaseToUpgrade_->insertOrUpdate(0,
															table % "_table_scanAxes",
															QStringList() << "id1" << "table1" << "id2" << "table2",
															QVariantList() << ids.at(i).toInt() << QString("%1_table").arg(table) << scanAxisId << "AMScanAxis_table")){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_TO_SCAN_CONFIGURATION_ASSOCIATED_TABLE, QString("Could not add to %1's scan axis associated table.").arg(table));
						return false;
					}

					int newRegionId = databaseToUpgrade_->insertOrUpdate(0,
																		 "AMScanAxisRegion_table",
																		 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime",
																		 QVariantList() << "AMScanAxisRegion" << 0 << QVariant() << QString("%1 region 1").arg(scanAxisName) << xStarts.at(i).toDouble() << xSteps.at(i).toDouble() << xEnds.at(i).toDouble() << timeSteps.at(i).toDouble());

					if (newRegionId == 0){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS_REGION, QString("Could not add %1 region 1 to AMScanAxisRegion_table.").arg(scanAxisName));
						return false;
					}

					if (!databaseToUpgrade_->insertOrUpdate(0,
															"AMScanAxis_table_axisRegions",
															QStringList() << "id1" << "table1" << "id2" << "table2",
															QVariantList() << scanAxisId << "AMScanAxis_table" << newRegionId << "AMScanAxisRegion_table")){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_TO_SCAN_AXIS_ASSOCIATED_TABLE, QString("Could not add to AMScanAxis_table_axisRegions's associated table."));
						return false;
					}

					// Second axis.
					scanAxisName = "V";
					scanAxisId = databaseToUpgrade_->insertOrUpdate(0,
																	"AMScanAxis_table",
																	QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "axisType",
																	QVariantList() << "AMScanAxis" << 0 << QVariant() << scanAxisName << 0);

					if (scanAxisId == 0){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS, QString("Could not add the %1 axis.").arg(scanAxisName));
						return false;
					}

					if (!databaseToUpgrade_->insertOrUpdate(0,
															table % "_table_scanAxes",
															QStringList() << "id1" << "table1" << "id2" << "table2",
															QVariantList() << ids.at(i).toInt() << QString("%1_table").arg(table) << scanAxisId << "AMScanAxis_table")){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_TO_SCAN_CONFIGURATION_ASSOCIATED_TABLE, QString("Could not add to %1's scan axis associated table.").arg(table));
						return false;
					}

					newRegionId = databaseToUpgrade_->insertOrUpdate(0,
																	 "AMScanAxisRegion_table",
																	 QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime",
																	 QVariantList() << "AMScanAxisRegion" << 0 << QVariant() << QString("%1 region 1").arg(scanAxisName) << yStarts.at(i).toDouble() << ySteps.at(i).toDouble() << yEnds.at(i).toDouble() << timeSteps.at(i).toDouble());

					if (newRegionId == 0){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS_REGION, QString("Could not add %1 region 1 to AMScanAxisRegion_table.").arg(scanAxisName));
						return false;
					}

					if (!databaseToUpgrade_->insertOrUpdate(0,
															"AMScanAxis_table_axisRegions",
															QStringList() << "id1" << "table1" << "id2" << "table2",
															QVariantList() << scanAxisId << "AMScanAxis_table" << newRegionId << "AMScanAxisRegion_table")){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_ADD_TO_SCAN_AXIS_ASSOCIATED_TABLE, QString("Could not add to AMScanAxis_table_axisRegions's associated table."));
						return false;
					}

					if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "xStart")
							|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "xStep")
							|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "xEnd")
							|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "yStart")
							|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "yStep")
							|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "yEnd")
							|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "timeStep")
							|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "fastAxis")
							|| !AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, QString("%1_table").arg(table), "slowAxis")){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS, QString("Could not remove one or more of 'xStart', 'xStep', 'xEnd', 'yStart', 'yStep, 'yEnd', 'timeStep, 'fastAxis', or 'slowAxis' columns from %1.").arg(table));
						return false;
					}

					if (!databaseToUpgrade_->update(scanConfigurationIds.at(tablesToModify.indexOf(table)),
													"AMDbObjectTypes_table",
													"inheritance",
													QString("%1;AMStepScanConfiguration;AMScanConfiguration;AMDbObject;QObject").arg(table))){

						databaseToUpgrade_->rollbackTransaction();
						AMErrorMon::alert(this, AMDBUPGRADE1PT5_COULD_NOT_UPDATE_INHERITANCE, QString("Could not update %1's inheritance.").arg(table));
						return false;
					}
				}
			}
		}
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade* AMDbUpgrade1Pt5::createCopy() const
{
	AMDbUpgrade* retVal = new AMDbUpgrade1Pt5(this->databaseNameToUpgrade());

	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();

	return retVal;
}

QString AMDbUpgrade1Pt5::upgradeToTag() const
{
	return QString("AMDbUpgrade1.5");
}

QString AMDbUpgrade1Pt5::description() const
{
	return QString("Upgrades all scan configurations that depend on AMRegionScanConfiguration and AM2DScanConfiguration.");
}

QString AMDbUpgrade1Pt5::axisName(const QString &tableName, int id) const
{
	if (tableName.contains(QRegExp("VESPERSSpatialLine"))){

		QString vespersDbObject = databaseToUpgrade_->retrieve(id, "VESPERSSpatialLineScanConfiguration_table", "configurationDbObject").toString();
		int motorValue = databaseToUpgrade_->retrieve(vespersDbObject.split(";").last().toInt(), "VESPERSScanConfigurationDbObject_table", "motor").toInt();

		if (motorValue == 1 || motorValue == 8)
			return "H";

		else if (motorValue == 2 || motorValue == 16)
			return "V";

		else
			return "H";
	}

	return "Energy";
}
