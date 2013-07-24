#include "AMDbUpgrade1Pt3.h"
/*
AMDbUpgrade1Pt3::AMDbUpgrade1Pt3(QString databaseNameToUpgrade, QObject *parent) :
    AMDbUpgrade(databaseNameToUpgrade,parent)
{
}

QStringList AMDbUpgrade1Pt3::upgradeFromTags() const{
    return QStringList() << "AMDbUpgrade1.2";
}


bool AMDbUpgrade1Pt2::upgradeNecessary() const{
    QList<int> matchingAMSampleType;

    // Check to make sure that this AMDbObjectType is in the AMDbObjectTypes_table
    matchingAMSampleType = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "AMSample");

    // So we found that, make sure that the columns in the actual table are correct and this actually needs to be upgraded
    if(matchingAMSampleType.count() > 0 ){

        QSqlQuery query = databaseToUpgrade_->query();
        query.prepare(QString("PRAGMA table_info(%1);").arg("AMSample_table"));
        databaseToUpgrade_->execQuery(query);
        QStringList columnNames;
        if (query.first()){

            do {
                columnNames << query.value(1).toString();
            }while(query.next());
        }
        query.finish();

        if(columnNames.count() == 9 && columnNames.contains("id") && columnNames.contains("AMDbObjectType") && columnNames.contains("thumbnailCount")
                && columnNames.contains("thumbnailFirstId") && columnNames.contains("name") && columnNames.contains("dateTime")
                && columnNames.contains("notes") && columnNames.contains("image") && columnNames.contains("elementList")){
            return true;
        }
    }

    return false;
}

bool AMDbUpgrade1Pt3::upgradeImplementation(){
    bool success = true;

    QMap<QString, QString> parentTablesToColumnsNames;
    parentTablesToColumnsNames.insert("SGMXASScanConfiguration_table", "detectorConfigs");
    parentTablesToColumnsNames.insert("SGMFastScanConfiguration_table", "detectorConfigs");
    QMap<QString, int> indexTablesToIndexSide;
    indexTablesToIndexSide.insert("AMDetectorInfoSet_table_detectorInfos", 1);

    // Use dbObjectClassBecomes to upgrade each detectorInfo to its new counterpart
    success &= AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "AMDetectorInfoSet", "AMOldDetectorInfoSet", parentTablesToColumnsNames, indexTablesToIndexSide);

    return success;
}
*/
