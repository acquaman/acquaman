#include "AMDbUpgrade1Pt5.h"

#include "util/AMErrorMonitor.h"
#include <QStringBuilder>

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
#include <QDebug>
bool AMDbUpgrade1Pt5::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	// Doing AMRegionScanConfiguration.
	QSqlQuery query = databaseToUpgrade_->query();
	query.prepare("SELECT inheritance FROM AMDbObjectTypes_table WHERE inheritance LIKE '%AMRegionScanConfiguration%';");

	if (!databaseToUpgrade_->execQuery(query)){

		query.finish();
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	QStringList scanConfigurations;

	if (query.first()){

		do {

			scanConfigurations << query.value(0).toString();
		} while (query.next());
	}

	query.finish();

	qDebug() << scanConfigurations;

	if (!scanConfigurations.isEmpty()){

		if (!databaseToUpgrade_->tableExists("AMStepScanConfiguration_table")
				&& databaseToUpgrade_->ensureTable("AMStepScanConfiguration_table",
												   QStringList() << "AMDbObjectTypes" << "thumbnailCount" << "thumbnailFirstId" << "name" << "detectorConfigurations" << "axisControlInfos",
												   QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "TEXT" << "TEXT")){

			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMStepScanConfiguration_table_scanAxes")
				&& databaseToUpgrade_->ensureTable("AMStepScanConfiguration_table_scanAxes",
												   QStringList() << "id1" << "table1" << "id2" << "table2",
												   QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT")){

			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxis_table")
				&& databaseToUpgrade_->ensureTable("AMScanAxis_table",
												   QStringList() << "AMDbObjectTypes" << "thumbnailCount" << "thumbnailFirstId" << "name" << "axisType",
												   QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "INTEGER")){

			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxis_table_axisRegions")
				&& databaseToUpgrade_->ensureTable("AMScanAxis_table_axisRegions",
												   QStringList() << "id1" << "table1" << "id2" << "table2",
												   QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT")){

			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxisRegion_table")
				&& databaseToUpgrade_->ensureTable("AMScanAxisRegion_table",
												   QStringList() << "AMDbObjectTypes" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime",
												   QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "REAL" << "REAL" << "REAL")){

			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		if (!databaseToUpgrade_->tableExists("AMScanAxisEXAFSRegion_table")
				&& databaseToUpgrade_->ensureTable("AMScanAxisEXAFSRegion_table",
												   QStringList() << "AMDbObjectTypes" << "thumbnailCount" << "thumbnailFirstId" << "name" << "regionStart" << "regionStep" << "regionEnd" << "regionTime" << "inKSpace" << "edgeEnergy" << "maximumTime" << "a2" << "equation",
												   QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "REAL" << "REAL" << "REAL" << "TEXT" << "REAL" << "REAL" << "REAL" << "INTEGER")){

			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		QStringList tablesToModify;

		foreach (QString configuration, scanConfigurations)
			tablesToModify << configuration.split(";").first();

		foreach (QString table, tablesToModify){

			QVariantList ids = databaseToUpgrade_->retrieve(table % "_table", "id");
			QVariantList regions = databaseToUpgrade_->retrieve(table % "_table", "regions");

			bool areEXAFSRegions = regions.first().toString().contains("exafsVersion1.0");

			int scanAxisId = databaseToUpgrade_->insertOrUpdate(0,
																"AMScanAxis_table",
																QStringList() << "AMDbObjectTypes" << "thumbnailCount" << "thumbnailFirstId" << "name" << "axisType",
																QVariantList() << "AMScanAxis" << 0 << QVariant() << "" << 0);

			for (int i = 0, regionsSize = regions.size(); i < regionsSize; i++){

				QStringList regionComponents = regions.at(i).toString().split(",");

			}
		}
	}

	// Doing AM2DScanConfiguration.
	scanConfigurations = QStringList();
	query.prepare("SELECT inheritance FROM AMDbObjectTypes_table WHERE inheritance LIKE '%AM2DScanConfiguration%';");

	if (!databaseToUpgrade_->execQuery(query)){

		query.finish();
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (query.first()){

		do {

			scanConfigurations << query.value(0).toString();
		} while (query.next());
	}

	query.finish();

	qDebug() << scanConfigurations;
	return false;
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
