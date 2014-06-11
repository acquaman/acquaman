#include "AMDbUpgrade1Pt4.h"

#include "util/AMErrorMonitor.h"

AMDbUpgrade1Pt4::AMDbUpgrade1Pt4(QString databaseNameToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList AMDbUpgrade1Pt4::upgradeFromTags() const{
	return QStringList() << "AMDbUpgrade1.2";
}

bool AMDbUpgrade1Pt4::upgradeNecessary() const{
	QList<int> matchingAMScanType;

	// Check to make sure that this AMDbObjectType is in the AMDbObjectTypes_table
	matchingAMScanType = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "AMScan");

	// So we found that, we need to upgrade if the sampleId column is still there and the sample column is not
	if(matchingAMScanType.count() > 0 ){

		QSqlQuery query = databaseToUpgrade_->query();
		query.prepare(QString("PRAGMA table_info(%1);").arg("AMScan_table"));
		databaseToUpgrade_->execQuery(query);
		QStringList columnNames;
		if (query.first()){

			do {
				columnNames << query.value(1).toString();
			}while(query.next());
		}
		query.finish();

		if(columnNames.contains("sampleId") && !columnNames.contains("sample"))
			return true;
	}

	// Check to make sure that this AMDbObjectType is in the AMDbObjectTypes_table
	matchingAMScanType = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "AMSamplePosition");

	// So we found that, we need to upgrade if the sampleId column is still there and the sample column is not
	if(matchingAMScanType.count() > 0 ){

		QSqlQuery query = databaseToUpgrade_->query();
		query.prepare(QString("PRAGMA table_info(%1);").arg("AMSamplePosition_table"));
		databaseToUpgrade_->execQuery(query);
		QStringList columnNames;
		if (query.first()){

			do {
				columnNames << query.value(1).toString();
			}while(query.next());
		}
		query.finish();

		if(columnNames.contains("sampleId") && !columnNames.contains("sample"))
			return true;
	}

	return false;
}

bool AMDbUpgrade1Pt4::upgradeImplementation(){
	bool success = true;

	// Change sampleId to samplePre2013 and sample (AMConstDbObject*) in AMScan and AMSamplePosition
	success &= AMDbUpgradeSupport::idColumnToConstDbObjectColumn(databaseToUpgrade_, "AMScan_table", "sampleId", "samplePre2013", "AMSample_table");
	success &= AMDbUpgradeSupport::idColumnToConstDbObjectColumn(databaseToUpgrade_, "AMSamplePosition_table", "sampleId", "sample", "AMSample_table");

	QMap<QString, QString> parentTablesToColumnsNames;
	QMap<QString, int> indexTablesToIndexSide;

	// Change AMSample to AMSamplePre2013 and update in AMScan and AMSamplePosition
	parentTablesToColumnsNames.insert("AMScan_table", "samplePre2013");
	parentTablesToColumnsNames.insert("AMSamplePosition_table", "sample");
	success &= AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "AMSample", "AMSamplePre2013", parentTablesToColumnsNames, indexTablesToIndexSide);

	// Change AMSamplePosition to AMSamplePositionPre2013 and update in auxiliary table
	parentTablesToColumnsNames.clear();
	indexTablesToIndexSide.clear();
	indexTablesToIndexSide.insert("AMSamplePlate_table_positions", 2);
	success &= AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "AMSamplePosition", "AMSamplePositionPre2013", parentTablesToColumnsNames, indexTablesToIndexSide);

	// Change AMSamplePlate to AMSamplePlatePre2013 and update in auxiliary table
	parentTablesToColumnsNames.clear();
	indexTablesToIndexSide.clear();
	indexTablesToIndexSide.insert("AMSamplePlate_table_positions", 1);
	success &= AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "AMSamplePlate", "AMSamplePlatePre2013", parentTablesToColumnsNames, indexTablesToIndexSide);
	return success;
}

AMDbUpgrade* AMDbUpgrade1Pt4::createCopy() const{
	AMDbUpgrade* retVal = new AMDbUpgrade1Pt4(this->databaseNameToUpgrade());
	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();
	return retVal;
}

QString AMDbUpgrade1Pt4::upgradeToTag() const{
	// Upgrade tag is:
	return QString("AMDbUpgrade1.4");
}

QString AMDbUpgrade1Pt4::description() const{
	return QString("Upgrade AMScan to use an AMConstDbObject for the sample rather than sampleId.");
}
