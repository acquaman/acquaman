/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "VESPERSDbUpgrade1Pt1.h"

#include "util/AMErrorMonitor.h"

 VESPERSDbUpgrade1Pt1::~VESPERSDbUpgrade1Pt1(){}
VESPERSDbUpgrade1Pt1::VESPERSDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt1::upgradeFromTags() const
{
	return QStringList();
}

bool VESPERSDbUpgrade1Pt1::upgradeNecessary() const
{
	return !databaseToUpgrade_->objectsMatching("AMScan_table", "AMDbObjectType", "AM2DScan").isEmpty();
}

bool VESPERSDbUpgrade1Pt1::upgradeImplementation()
{
	QList<int> indicesToUpgrade = databaseToUpgrade_->objectsMatching("AMScan_table", "AMDbObjectType", "AM2DScan");
	QList<int> measurements;
	QVariant detectorChoice;
	QString configName;
	int configId;
	QVariant numberOfElements;

	databaseToUpgrade_->startTransaction();

	for (int i = 0, size = indicesToUpgrade.size(); i < size; i++){

		detectorChoice = databaseToUpgrade_->retrieve(indicesToUpgrade.at(i), "AMScan_table", "scanConfiguration");
		configName = detectorChoice.toString().split(";").first();
		configId = detectorChoice.toString().split(";").at(1).toInt();

		if (configName != "VESPERS2DScanConfiguration_table"){

			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT1_INVALID_SCAN_CONFIGURATION, QString("Scan configuration was not valid. %1 is not VESPERS2DScanConfiguration_table.").arg(configName));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		numberOfElements = databaseToUpgrade_->retrieve(configId, configName, "fluorescenceDetectorChoice");

		if (numberOfElements.toInt() == 1){

			measurements = databaseToUpgrade_->objectsMatching("AMScan_table_rawDataSources", "id1", indicesToUpgrade.at(i));
			int rawDatasourceId = databaseToUpgrade_->insertOrUpdate(0, "AMRawDataSource_table",
																	 QStringList() << "AMDbObjectType" << "thumbnailCount" << "name" << "measurementId" << "description" << "scanRank" << "measurementRank" << "rank" << "visibleInPlots" << "hiddenFromUsers",
																	 QVariantList() << "AMRawDataSource" << 0 << "spectra" << measurements.size() << "1-el Vortex" << 2 << 1 << 3 << "false" << "true"
																	 );

			if (!rawDatasourceId || !databaseToUpgrade_->insertOrUpdate(0, "AMScan_table_rawDataSources",
																		   QStringList() << "id1" << "table1" << "id2" << "table2",
																		   QVariantList() << indicesToUpgrade.at(i) << "AMScan_table" << rawDatasourceId << "AMRawDataSource_table")
					){

				AMErrorMon::alert(this, VESPERSDBUPGRADE1PT1_COULD_NOT_INSERT, QString("Could not insert into the following table: %1.").arg(rawDatasourceId == 0 ? "AMRawDataSource_table" : "AMScan_table_rawDataSources"));
				databaseToUpgrade_->rollbackTransaction();
				return false;
			}
		}

		else if (numberOfElements.toInt() == 2){

			measurements = databaseToUpgrade_->objectsMatching("AMScan_table_rawDataSources", "id1", indicesToUpgrade.at(i));
			int rawDatasourceId = databaseToUpgrade_->insertOrUpdate(0, "AMRawDataSource_table",
																	 QStringList() << "AMDbObjectType" << "thumbnailCount" << "name" << "measurementId" << "description" << "scanRank" << "measurementRank" << "rank" << "visibleInPlots" << "hiddenFromUsers",
																	 QVariantList() << "AMRawDataSource" << 0 << "corrSum" << measurements.size() << "4-el Vortex" << 2 << 1 << 3 << "false" << "true"
																	 );

			if (!rawDatasourceId || !databaseToUpgrade_->insertOrUpdate(0, "AMScan_table_rawDataSources",
																		   QStringList() << "id1" << "table1" << "id2" << "table2",
																		   QVariantList() << indicesToUpgrade.at(i) << "AMScan_table" << rawDatasourceId << "AMRawDataSource_table")
					){

				AMErrorMon::alert(this, VESPERSDBUPGRADE1PT1_COULD_NOT_INSERT, QString("Could not insert into the following table: %1.").arg(rawDatasourceId == 0 ? "AMRawDataSource_table" : "AMScan_table_rawDataSources"));
				databaseToUpgrade_->rollbackTransaction();
				return false;
			}

			for (int index = 1; index < 5; index++){

				rawDatasourceId = databaseToUpgrade_->insertOrUpdate(0, "AMRawDataSource_table",
																	 QStringList() << "AMDbObjectType" << "thumbnailCount" << "name" << "measurementId" << "description" << "scanRank" << "measurementRank" << "rank" << "visibleInPlots" << "hiddenFromUsers",
																	 QVariantList() << "AMRawDataSource" << 0 << QString("raw%1").arg(index) << measurements.size()+index << "4-el Vortex" << 2 << 1 << 3 << "false" << "true"
																	 );

				if (!rawDatasourceId || !databaseToUpgrade_->insertOrUpdate(0, "AMScan_table_rawDataSources",
																			   QStringList() << "id1" << "table1" << "id2" << "table2",
																			   QVariantList() << indicesToUpgrade.at(i) << "AMScan_table" << rawDatasourceId << "AMRawDataSource_table")
						){

					AMErrorMon::alert(this, VESPERSDBUPGRADE1PT1_COULD_NOT_INSERT, QString("Could not insert into the following table: %1.").arg(rawDatasourceId == 0 ? "AMRawDataSource_table" : "AMScan_table_rawDataSources"));
					databaseToUpgrade_->rollbackTransaction();
					return false;
				}
			}
		}

		else{

			AMErrorMon::alert(this, VESPERSDBUPGRADE1PT1_INCORRECT_ELEMENT_COUNT, QString("The number of elements was not 1 or 4."));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt1::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt1(this->databaseNameToUpgrade());

	if (this->databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt1::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.1";
}

QString VESPERSDbUpgrade1Pt1::description() const
{
	return "Upgrade all 2D scans to have spectra raw data sources.  Scans using the single element get one extra raw data source and the four element get five extra.";
}
