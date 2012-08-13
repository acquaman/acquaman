#include "VESPERSDbUpgrade1Pt1.h"

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

	for (int i = 0, size = indicesToUpgrade.size(); i < size; i++){

		detectorChoice = databaseToUpgrade_->retrieve(indicesToUpgrade.at(i), "AMScan_table", "scanConfiguration");
		configName = detectorChoice.toString().split(";").first();
		configId = detectorChoice.toString().split(";").at(1).toInt();

		if (configName != "VESPERS2DScanConfiguration_table")
			return false;

		numberOfElements = databaseToUpgrade_->retrieve(configId, configName, "fluorescenceDetectorChoice");

		if (numberOfElements.toInt() == 1){

			measurements = databaseToUpgrade_->objectsMatching("AMScan_table_rawDataSources", "id1", indicesToUpgrade.at(i));
			int rawDatasourceId = databaseToUpgrade_->insertOrUpdate(0, "AMRawDataSource_table",
																	 QStringList() << "AMDbObjectType" << "thumbnailCount" << "name" << "mearurementId" << "description" << "scanRank" << "measurementRank" << "rank" << "visibleInPlots" << "hiddenFromUsers",
																	 QVariantList() << "AMRawDataSource" << 0 << "spectra" << measurements.size() << "1-el Vortex" << 1 << 1 << 1 << "false" << "true"
																	 );

			if (!rawDatasourceId || !databaseToUpgrade_->insertOrUpdate(0, "AMScan_table_rawDataSources",
																		   QStringList() << "id1" << "table1" << "id2" << "table2",
																		   QVariantList() << indicesToUpgrade.at(i) << "AMScan_table" << rawDatasourceId << "AMRawDataSource_table"))
					return false;
		}

		else if (numberOfElements.toInt() == 4){

			measurements = databaseToUpgrade_->objectsMatching("AMScan_table_rawDataSources", "id1", indicesToUpgrade.at(i));
			int rawDatasourceId = databaseToUpgrade_->insertOrUpdate(0, "AMRawDataSource_table",
																	 QStringList() << "AMDbObjectType" << "thumbnailCount" << "name" << "mearurementId" << "description" << "scanRank" << "measurementRank" << "rank" << "visibleInPlots" << "hiddenFromUsers",
																	 QVariantList() << "AMRawDataSource" << 0 << "corrSum" << measurements.size() << "4-el Vortex" << 1 << 1 << 2 << "false" << "true"
																	 );

			if (!rawDatasourceId || !databaseToUpgrade_->insertOrUpdate(0, "AMScan_table_rawDataSources",
																		   QStringList() << "id1" << "table1" << "id2" << "table2",
																		   QVariantList() << indicesToUpgrade.at(i) << "AMScan_table" << rawDatasourceId << "AMRawDataSource_table"))
					return false;

			for (int index = 1; index < 5; index++){

				rawDatasourceId = databaseToUpgrade_->insertOrUpdate(0, "AMRawDataSource_table",
																	 QStringList() << "AMDbObjectType" << "thumbnailCount" << "name" << "mearurementId" << "description" << "scanRank" << "measurementRank" << "rank" << "visibleInPlots" << "hiddenFromUsers",
																	 QVariantList() << "AMRawDataSource" << 0 << QString("raw%1").arg(index) << measurements.size()+index << "4-el Vortex" << 1 << 1 << 2 << "false" << "true"
																	 );

				if (!rawDatasourceId || !databaseToUpgrade_->insertOrUpdate(0, "AMScan_table_rawDataSources",
																			   QStringList() << "id1" << "table1" << "id2" << "table2",
																			   QVariantList() << indicesToUpgrade.at(i) << "AMScan_table" << rawDatasourceId << "AMRawDataSource_table"))
						return false;
			}
		}

		else
			return false;
	}

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
