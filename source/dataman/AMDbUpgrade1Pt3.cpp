#include "AMDbUpgrade1Pt3.h"

#include "util/AMErrorMonitor.h"

AMDbUpgrade1Pt3::AMDbUpgrade1Pt3(QString databaseNameToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList AMDbUpgrade1Pt3::upgradeFromTags() const{
	return QStringList() << "AMDbUpgrade1.2";
}

bool AMDbUpgrade1Pt3::upgradeNecessary() const{
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

	return false;
}

bool AMDbUpgrade1Pt3::upgradeImplementation(){
	bool success = true;

	// Change the column name from sampleId to sample
	success &= AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "AMScan_table", "sampleId", "sample", "TEXT");
	// If that worked, change all cases where the value was "-1" to "" (empty string)
	if(success)
		success &= databaseToUpgrade_->update("AMScan_table", "sample='-1'", "sample", "");
	else
		AMErrorMon::alert(this, AMDBUPGRADE1PT3_COULD_NOT_CHANGE_COLUMN_NAME, "Could not change the sampleID column to sample in AMScan_table.");
	// If that worked, grab all of the other values and update them from "#" to "AMSample_table;#"
	if(success){
		// Grab id and sample column for all rows in AMScan_table where the sample isn't empty string
		QSqlQuery query = databaseToUpgrade_->select("AMScan_table", "id,sample", "sample!=''");
		databaseToUpgrade_->execQuery(query);
		QMap<int, QString> results;
		// Iterate and place in a map
		if (query.first()){

			do {
				results.insert(query.value(0).toInt(), query.value(1).toString());
			}while(query.next());
		}
		query.finish();

		// Start a transaction, for each value in the map update the sample column for that id from "#" to "AMSample_table;#"
		databaseToUpgrade_->startTransaction();
		QMap<int, QString>::const_iterator i = results.constBegin();
		while (i != results.constEnd() && success) {
			success &= databaseToUpgrade_->update(i.key(), "AMScan_table", "sample", QString("AMSample_table;%1").arg(i.value()));
			++i;
		}
		// Rollback if we failed, commit if we succeeded
		if(!success){
			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, AMDBUPGRADE1PT3_COULD_NOT_UPDATE_TO_TABLE_BASED_ID, "Could not change the sample column value to a table based value for one of the indexes in AMScan_table.");
		}
		else
			databaseToUpgrade_->commitTransaction();

	}
	else
		AMErrorMon::alert(this, AMDBUPGRADE1PT3_COULD_NOT_UPDATE_EMPTY_VALUES, "Could not update the empty values from -1 to an empty string in AMScan_table.");

	return success;
}

AMDbUpgrade* AMDbUpgrade1Pt3::createCopy() const{
	AMDbUpgrade* retVal = new AMDbUpgrade1Pt3(this->databaseNameToUpgrade());
	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();
	return retVal;
}

QString AMDbUpgrade1Pt3::upgradeToTag() const{
	// Upgrade tag is:
	return QString("AMDbUpgrade1.3");
}

QString AMDbUpgrade1Pt3::description() const{
	return QString("Upgrade AMScan to use an AMConstDbObject for the sample rather than sampleId.");
}
