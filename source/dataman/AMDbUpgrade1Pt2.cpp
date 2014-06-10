#include "AMDbUpgrade1Pt2.h"

AMDbUpgrade1Pt2::~AMDbUpgrade1Pt2(){}

AMDbUpgrade1Pt2::AMDbUpgrade1Pt2(QString databaseNameToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList AMDbUpgrade1Pt2::upgradeFromTags() const{
	return QStringList() << "AMDbUpgrade1.1";
}

bool AMDbUpgrade1Pt2::upgradeNecessary() const{
	QList<int> matchingAMDetectorInfoSetType;

	// Check to make sure that this AMDbObjectType is in the AMDbObjectTypes_table
	matchingAMDetectorInfoSetType = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "AMDetectorInfoSet");

	// So we found that, make sure that the columns in the actual table are correct and this actually needs to be upgraded
	if(matchingAMDetectorInfoSetType.count() > 0 ){

		QSqlQuery query = databaseToUpgrade_->query();
		query.prepare(QString("PRAGMA table_info(%1);").arg("AMDetectorInfoSet_table"));
		databaseToUpgrade_->execQuery(query);
		QStringList columnNames;
		if (query.first()){

			do {
				columnNames << query.value(1).toString();
			}while(query.next());
		}
		query.finish();

		if(columnNames.count() == 7 && columnNames.contains("id") && columnNames.contains("AMDbObjectType") && columnNames.contains("thumbnailCount")
				&& columnNames.contains("thumbnailFirstId") && columnNames.contains("name") && columnNames.contains("description")
				&& columnNames.contains("activeDetectorInfos")){
			return true;
		}
	}

	return false;
}

bool AMDbUpgrade1Pt2::upgradeImplementation(){

	QMap<QString, QString> parentTablesToColumnsNames;
	parentTablesToColumnsNames.insert("SGMXASScanConfiguration_table", "detectorConfigs");
	parentTablesToColumnsNames.insert("SGMFastScanConfiguration_table", "detectorConfigs");
	QMap<QString, int> indexTablesToIndexSide;
	indexTablesToIndexSide.insert("AMDetectorInfoSet_table_detectorInfos", 1);

	databaseToUpgrade_->startTransaction();
	// Use dbObjectClassBecomes to upgrade each detectorInfo to its new counterpart
	if (!AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "AMDetectorInfoSet", "AMOldDetectorInfoSet", parentTablesToColumnsNames, indexTablesToIndexSide)){

		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	databaseToUpgrade_->commitTransaction();
	return true;
}

AMDbUpgrade* AMDbUpgrade1Pt2::createCopy() const{
	AMDbUpgrade* retVal = new AMDbUpgrade1Pt2(this->databaseNameToUpgrade());
	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();
	return retVal;
}

QString AMDbUpgrade1Pt2::upgradeToTag() const{
	// Upgrade tag is:
	return QString("AMDbUpgrade1.2");
}

QString AMDbUpgrade1Pt2::description() const{
	return QString("Upgrade AMDetectorInfoSet to AMOldDetectorInfoSet to prepare for the new AMDetector suite.");
}
