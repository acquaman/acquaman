/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMDbUpgrade.h"

#include <QDateTime>
#include <QStringBuilder>
#include "dataman/database/AMDbObjectSupport.h"

#include "util/AMErrorMonitor.h"

AMDbUpgrade::AMDbUpgrade(QString databaseNameToUpgrade, QObject *parent) :
	QObject(parent)
{
	databaseToUpgrade_ = 0; //NULL
	databaseNameToUpgrade_ = databaseNameToUpgrade;
	isResponsibleForUpgrade_ = true; // responsible by default
}

bool AMDbUpgrade::upgrade(){
	// Make sure the dependencies in upgradeFromTags are present
	if(upgradeFromTags().count() > 0){
		QString allFromStrings = upgradeFromTags().join("','");
		allFromStrings.prepend("('");
		allFromStrings.append("')");
		QList<int> allFromIDs = databaseToUpgrade_->objectsWhere("AMDbObjectUpgrades_table", QString("upgradeTag IN %1").arg(allFromStrings));
		if(allFromIDs.count() != upgradeFromTags().count()){
			AMErrorMon::error(0, -270101, QString("Database upgrade: Could not process upgrade because one or more of the following upgrades is missing %1 from %2").arg(upgradeFromTags().join(" ")).arg(databaseToUpgrade_->connectionName()));
			return false;
		}
	}
	// If so, call and return the specific upgradeImplementation for the subclass
	return upgradeImplementation();
}

bool AMDbUpgrade::updateUpgradeTable(bool isNecessary, bool duringCreation){
	// Create the list of columns and values to be put into the new upgrade table rows for each requested database
	QStringList columnNames;
	columnNames << "description" << "upgradeTag" << "necessaryUpgrade" << "upgradeDate" << "duringCreation";
	QVariantList upgradeValues;
	QString isNecessaryString = "false";
	if(isNecessary)
		isNecessaryString = "true";
	QString duringCreationString = "false";
	if(duringCreation)
		duringCreationString = "true";
	upgradeValues << QVariant(description()) << QVariant(upgradeToTag()) << QVariant(isNecessaryString) << QVariant(QDateTime::currentDateTime()) << QVariant(duringCreationString);

	// Add the new row (ensure this is successful)
	int upgradeId = databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectUpgrades_table", columnNames, upgradeValues);
	if(upgradeId < 1){
		AMErrorMon::error(0, -270102, QString("Database upgrade: Could not write to the database upgrades table for %1").arg(databaseToUpgrade_->connectionName()));
		return false;
	}
	return true;
}

bool AMDbUpgrade::upgradeRequired() const{
	// Check to make sure each has the upgrade table using the PRAGMA query
	QSqlQuery q = databaseToUpgrade_->query();
	q.prepare("PRAGMA table_info(AMDbObjectUpgrades_table)");
	if(!AMDatabase::execQuery(q)) {
		q.finish();
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -270103, QString("Database support: There was an error while trying to read meta data on the upgrades table.")));
		return false;
	}
	QStringList upgradeTableColumns;
	while(q.next()){
		upgradeTableColumns << q.value(1).toString();
	}
	q.finish();

	// If the upgrade table is missing, make it and log that at least one requested database is missing the table
	if(upgradeTableColumns.count() == 0){
		databaseToUpgrade_->ensureTable(AMDbObjectSupport::upgradesTableName(),
							   QString("description,upgradeTag,necessaryUpgrade,upgradeDate,duringCreation").split(','),
							   QString("TEXT,TEXT,TEXT,TEXT,TEXT").split(','),
							   false);

		// Return true (upgrade is required) if the table was missing
		return true;
	}

	// Check to make sure that the specified upgrade tag is present. If not, return true (database upgrade is required).
	QList<int> thisTagID = databaseToUpgrade_->objectsWhere("AMDbObjectUpgrades_table", QString("upgradeTag='%1'").arg(upgradeToTag()));
	if(thisTagID.count() == 0)
		return true;
	return false;
}

bool AMDbUpgrade::loadDatabaseFromName(){
	AMDatabase *database;
	database = AMDatabase::database(databaseNameToUpgrade_);
	if(!database)
		return false;
	databaseToUpgrade_ = database;
	return true;
}

bool AMDbUpgrade::isResponsibleForUpgrade() const{
	return isResponsibleForUpgrade_;
}

QString AMDbUpgrade::databaseNameToUpgrade() const{
	return databaseNameToUpgrade_;
}

AMDatabase* AMDbUpgrade::databaseToUpgrade() const{
	return databaseToUpgrade_;
}

void AMDbUpgrade::setDatabaseNameToUpgrade(const QString &databaseNameToUpgrade){
	databaseNameToUpgrade_ = databaseNameToUpgrade;
}

bool AMDbUpgrade::setDatabaseNameToUpgradeAndLoad(const QString &databaseNameToUpgrade){
	setDatabaseNameToUpgrade(databaseNameToUpgrade);
	return loadDatabaseFromName();
}

void AMDbUpgrade::setIsResponsibleForUpgrade(bool isResponsibleForUpgrade){
	isResponsibleForUpgrade_ = isResponsibleForUpgrade;
}

bool AMDbUpgradeSupport::dbObjectClassBecomes(AMDatabase *databaseToEdit, const QString &originalClassName, const QString &newClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide){
	//AMDatabase *userDb = AMDatabase::database("user");
	AMDatabase *userDb = databaseToEdit;

	QString originalTableName = originalClassName%"_table";
	QString newTableName = newClassName%"_table";

	QList<int> matchingOriginial;
	QList<int> matchingNew;

	// Find the number of objects matching the original and new class names in the AMDbObjectTypes table
	matchingOriginial = userDb->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", originalClassName);
	matchingNew = userDb->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", newClassName);


	// If they're both non-zero, then roll the database back by converting all of the "new" side instances to "original" side instances by using the classMerge function.
	if(matchingOriginial.count() > 0 && matchingNew.count() > 0){
		if(!dbObjectClassMerge(userDb, originalClassName, newClassName, parentTablesToColumnNames, indexTablesToIndexSide))
			return false;

		matchingNew = userDb->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", newClassName);
	}

	// If there are no instances of the new class, then we can upgrade in this fashion
	if(matchingOriginial.count() > 0 && matchingNew.count() == 0){
		// Start the transaction, we can rollback to here if things go badly
		userDb->startTransaction();

		// Update the AMDbObjectTypes_table to replace the AMDbObjectType and table name with the new values
		QStringList columnsToChange;
		QVariantList changesToMake;
		columnsToChange << "AMDbObjectType" << "tableName";
		changesToMake << QVariant(newClassName) << QVariant(newTableName);
		if(!userDb->update(matchingOriginial.at(0), "AMDbObjectTypes_table", columnsToChange, changesToMake)){
			userDb->rollbackTransaction();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -270, QString("Database support: There was an error trying to update the AMDbObjectTypes table for (%1) to become %2.").arg(originalClassName).arg(newClassName)));
			return false;
		}

		// Loop over all the parent tables to column names map values
		QMap<QString, QString>::const_iterator j = parentTablesToColumnNames.constBegin();
		while (j != parentTablesToColumnNames.constEnd()) {
			// Grab the list of indices we're interested in and for all those indices update to the new class name while maintaining the original index value (format: table_name;index_value)
			QList<int> parentTableIndices = userDb->objectsWhere(j.key(), QString("%1 LIKE '%2;%'").arg(j.value()).arg(originalTableName));
			for(int x = 0; x < parentTableIndices.count(); x++){
				QString indexStringToUse = userDb->retrieve(parentTableIndices.at(x), j.key(), j.value()).toString();
				int indexToUse = indexStringToUse.split(';').at(1).toInt();
				if(!userDb->update(parentTableIndices.at(x), j.key(), j.value(), QVariant(QString("%1;%2").arg(newTableName).arg(indexToUse)))){
					userDb->rollbackTransaction();
					AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -281, QString("Database support: There was an error trying to update the %1 column for %2 table at id %2.").arg(j.value()).arg(j.key()).arg(parentTableIndices.at(x))));
					return false;
				}
			}
			++j;
		}

		// Loop over all the index table to column map values
		QMap<QString, int>::const_iterator i = indexTablesToIndexSide.constBegin();
		while (i != indexTablesToIndexSide.constEnd()) {
			// Grab the list of indices we're interested in and for all those indices update to the new class name
			QList<int> indexTableIndices = userDb->objectsWhere(i.key(), QString("table%1='%2'").arg(i.value()).arg(originalTableName));
			for(int x = 0; x < indexTableIndices.count(); x++){
				if(!userDb->update(indexTableIndices.at(x), i.key(), QString("table%1").arg(i.value()), QVariant(newTableName))){
					userDb->rollbackTransaction();
					AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -271, QString("Database support: There was an error trying to update the %1 table at id %2.").arg(i.key()).arg(indexTableIndices.at(x))));
					return false;
				}
			}
			++i;
		}

		// Go to the actual class table (the original one) and update the AMDbObjectType column
		if(!userDb->update(originalTableName, "AMDbObjectType='"%originalClassName%"'", "AMDbObjectType", QVariant(newClassName))){
			userDb->rollbackTransaction();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -272, QString("Database support: There was an error trying to update the %1 table for new AMDbObjectType %2.").arg(originalTableName).arg(newClassName)));
			return false;
		}

		// Finally, rename the table from the original name to the new name
		QSqlQuery q = userDb->query();
		q.prepare("ALTER table "%originalTableName%" RENAME to "%newTableName);
		if(!AMDatabase::execQuery(q)) {
			q.finish();
			userDb->rollbackTransaction();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -273, QString("Database support: There was an error while trying to update table %1 to become %2.").arg(originalClassName).arg(newClassName)));
			return false;
		}

		// If there were no problems then commit the transaction
		userDb->commitTransaction();
	}
	// If there are no instances of the original class then no upgrade is necessary
	else if(matchingOriginial.count() == 0 && matchingNew.count() > 0){
	}
	// Neither class name can be found, did you type something in wrong or maybe they aren't in this database
	else{
	}
	return true;
}

bool AMDbUpgradeSupport::dbObjectClassMerge(AMDatabase *databaseToEdit, const QString &mergeToClassName, const QString &mergeFromClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide){
	//AMDatabase *userDb = AMDatabase::database("user");
	AMDatabase *userDb = databaseToEdit;

	QString mergeToTableName = mergeToClassName%"_table";
	QString mergeFromTableName = mergeFromClassName%"_table";

	// Grab the list of toIDs and fromIDs so we can get counts and offsets
	QList<int> allToIDs = userDb->objectsMatching(mergeToTableName, "AMDbObjectType", mergeToClassName);
	QList<int> allFromIDs = userDb->objectsMatching(mergeFromTableName, "AMDbObjectType", mergeFromClassName);
	int toCount = allToIDs.count();

	// Start the transaction so we can rollback to here if something goes wrong
	userDb->startTransaction();

	// Query and record all the column names (except id) from the from table. We need this string list to pass around.
	QSqlQuery q = userDb->query();
	q.prepare("PRAGMA table_info("%mergeFromTableName%")");
	if(!AMDatabase::execQuery(q)) {
		q.finish();
		userDb->rollbackTransaction();
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -274, QString("Database support: There was an error while trying to read meta data on table %1.").arg(mergeFromTableName)));
		return false;
	}
	QStringList allTableColumns;
	while(q.next()){
		allTableColumns << q.value(1).toString();
	}
	allTableColumns.removeFirst();
	q.finish();

	// Insert all the "from" side items into the "to" side table with an offset in their ids
	for(int x = 0; x < allFromIDs.count(); x++){
		QVariantList valuesFromFromTable = userDb->retrieve(allFromIDs.at(x), mergeFromTableName, allTableColumns);
		int fromIDInOldTable = userDb->insertOrUpdate(toCount+allFromIDs.at(x), mergeToTableName, allTableColumns, valuesFromFromTable );
		if(fromIDInOldTable == 0){
			userDb->rollbackTransaction();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -275, QString("Database support: There was an error while trying to insert into to table format %1.").arg(mergeToTableName)));
			return false;
		}
	}

	// Loop over all the parent tables to column names map values
	QMap<QString, QString>::const_iterator j = parentTablesToColumnNames.constBegin();
	while (j != parentTablesToColumnNames.constEnd()) {
		// Grab the list of indices we're interested in and for all those indices update to the new class name while updating the original index value for the correct offset (format: table_name;index_value)
		QList<int> parentTableIndices = userDb->objectsWhere(j.key(), QString("%1 LIKE '%2;%'").arg(j.value()).arg(mergeFromTableName));
		for(int x = 0; x < parentTableIndices.count(); x++){
			QString indexStringToUse = userDb->retrieve(parentTableIndices.at(x), j.key(), j.value()).toString();
			int indexToUse = indexStringToUse.split(';').at(1).toInt() + toCount;
			if(!userDb->update(parentTableIndices.at(x), j.key(), j.value(), QVariant(QString("%1;%2").arg(mergeToTableName).arg(indexToUse)))){
				userDb->rollbackTransaction();
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -282, QString("Database support: There was an error trying to update the %1 column for %2 table at id %2.").arg(j.value()).arg(j.key()).arg(parentTableIndices.at(x))));
				return false;
			}
		}
		++j;
	}

	// Loop over all of the index tables we're interested in so we can change any "from" side items to refer to the "original" side table with their revamped ids.
	QMap<QString, int>::const_iterator i = indexTablesToIndexSide.constBegin();
	while (i != indexTablesToIndexSide.constEnd()) {
		QList<int> indexTableIndices = userDb->objectsWhere(i.key(), QString("table%1='%2'").arg(i.value()).arg(mergeFromTableName));
		QStringList updateColumns;
		QString idString = QString("id%1").arg(i.value());
		updateColumns << idString << QString("table%1").arg(i.value());
		for(int x = 0; x< indexTableIndices.count(); x++){
			int currentId2 = userDb->retrieve(indexTableIndices.at(x), i.key(), idString).toInt() + toCount;
			QVariantList updateValues;
			updateValues << QVariant(currentId2) << QVariant(mergeToTableName);
			if(!userDb->update(indexTableIndices.at(x), i.key(), updateColumns, updateValues)){
				userDb->rollbackTransaction();
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -276, QString("Database support: There was an error trying to update the %1 table at id %2.").arg(i.key()).arg(indexTableIndices.at(x))));
				return false;
			}
		}
		++i;
	}

	// Grab the id of the from class from the AMDbObjectTypes table. We need this to get rid of items in the related tables
	QList<int> fromClassDbObjectIds = userDb->objectsWhere("AMDbObjectTypes_table", "AMDbObjectType = '"%mergeFromClassName%"'");
	if(fromClassDbObjectIds.count() != 1){
		userDb->rollbackTransaction();
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -277, QString("Database support: There was an error trying to find %1 in the AMDbObjectTypes table.").arg(mergeFromClassName)));
		return false;
	}
	int fromClassDbObjectId = fromClassDbObjectIds.at(0);

	// Delete the from class from the AMDbObjectsType table.
	if(!userDb->deleteRow(fromClassDbObjectId, "AMDbObjectTypes_table")){
		userDb->rollbackTransaction();
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -278, QString("Database support: There was an error trying to rollback the AMDbObjectTypes table for %1.").arg(mergeFromClassName)));
		return false;
	}

	// Loop over a list of tables related to AMDbObjectTypes. Remove all instances of the TypeId we just grabbed from the last db call.
	QStringList relatedAMDbObjectTypesTables;
	relatedAMDbObjectTypesTables << "allColumns" << "visibleColumns" << "loadColumns";
	for(int x = 0; x < relatedAMDbObjectTypesTables.count(); x++){
		if(userDb->deleteRows("AMDbObjectTypes_"%relatedAMDbObjectTypesTables.at(x), QString("TypeId='%1'").arg(fromClassDbObjectId)) == 0){
			userDb->rollbackTransaction();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -279, QString("Database support: There was an error trying to rollback the AMDbObjectTypes related table %1.").arg("AMDbObjectTypes_"%relatedAMDbObjectTypesTables.at(x))));
			return false;
		}
	}

	// Actually delete the "from" side class table.
	q = userDb->query();
	q.prepare("DROP TABLE "%mergeFromTableName);
	if(!AMDatabase::execQuery(q)) {
		q.finish();
		userDb->rollbackTransaction();
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -280, QString("Database support: There was an error while trying to rollback table %1.").arg(mergeFromTableName)));
		return false;
	}
	q.finish();

	// Commit this transaction
	userDb->commitTransaction();
	return true;
}

bool AMDbUpgradeSupport::changeColumnName(AMDatabase *databaseToEdit, const QString &tableName, const QString &oldColumnName, const QString &newColumnName, const QString &newType)
{
	// This method renames the table, makes a new one with the old name with the column named correctly.  Then it copies all of the data into the new table and deletes the old table.  Finally, the AMDbObjectTypes tables need to be updated.
	AMDatabase *db = databaseToEdit;
	db->startTransaction();

	QString tempName = "temp_table";
	QSqlQuery query = db->query();

	// Change the name of the old table.
	query.prepare(QString("ALTER TABLE %1 RENAME TO %2;").arg(tableName).arg(tempName));
	if (!db->execQuery(query)){

		query.finish();
		db->rollbackTransaction();
		AMErrorMon::alert(0, AMDBUPGRADESUPPORT_COULD_NOT_RENAME_TABLE, QString("Could not rename %1.").arg(tableName));
		return false;
	}

	query.finish();

	// Get the column names, make the change and create a new table with the original name but with the new column names.
	query.prepare(QString("PRAGMA table_info(%1);").arg(tempName));
	db->execQuery(query);
	QStringList oldColumnNames;
	QStringList columnTypes;
	if (query.first()){

		do {

			oldColumnNames << query.value(1).toString();
			columnTypes << query.value(2).toString();
		}while(query.next());
	}
	query.finish();

	QStringList newColumnNames(oldColumnNames);
	int indexOfOldColumnName = newColumnNames.indexOf(oldColumnName);
	newColumnNames.replace(indexOfOldColumnName, newColumnName);

	if (!newType.isNull())
		columnTypes.replace(indexOfOldColumnName, newType);

	QString columnNamesAndTypes;

	for (int i = 0, size = newColumnNames.size(); i < size; i++)
		columnNamesAndTypes += QString("%1 %2, ").arg(newColumnNames.at(i)).arg(columnTypes.at(i));

	columnNamesAndTypes.chop(2);

	query.prepare(QString("CREATE TABLE %1 (%2);").arg(tableName).arg(columnNamesAndTypes));
	if (!db->execQuery(query)){

		query.finish();
		db->rollbackTransaction();
		AMErrorMon::alert(0, AMDBUPGRADESUPPORT_COULD_NOT_CREATE_NEW_TABLE, QString("Could not create a new table."));
		return false;
	}

	query.finish();

	// Move the data from the old table to the new table.
	query.prepare(QString("INSERT INTO %1 (%2) SELECT %3 FROM %4;").arg(tableName).arg(newColumnNames.join(", ")).arg(oldColumnNames.join(", ")).arg(tempName));

	if (!db->execQuery(query)){

		query.finish();
		db->rollbackTransaction();
		AMErrorMon::alert(0, AMDBUPGRADESUPPORT_COULD_NOT_COPY_FROM_OLD_TABLE, QString("Could not copy the data from the old table."));
		return false;
	}

	query.finish();

	// Delete the old table.
	query.prepare(QString("DROP TABLE %1;").arg(tempName));

	if (!db->execQuery(query)){

		query.finish();
		db->rollbackTransaction();
		AMErrorMon::alert(0, AMDBUPGRADESUPPORT_COULD_NOT_DELETE_OLD_TABLE, QString("Could not delete the old table."));
		return false;
	}

	query.finish();

	// Finally, we need to update the AMDbObjectTypes_* tables (allColumns, visibleColumns, loadColumns).  The still have the old names and will corrupt the database if not updated.
	QList<int> ids = db->objectsMatching("AMDbObjectTypes_table", "tableName", tableName);

	if (ids.size() == 1){

		bool success = true;
		success &= db->update("AMDbObjectTypes_allColumns", QString("typeId=%1 AND columnName='%2'").arg(ids.first()).arg(oldColumnName), "columnName", newColumnName);
		success &= db->update("AMDbObjectTypes_visibleColumns", QString("typeId=%1 AND columnName='%2'").arg(ids.first()).arg(oldColumnName), "columnName", newColumnName);
		success &= db->update("AMDbObjectTypes_loadColumns", QString("typeId=%1 AND columnName='%2'").arg(ids.first()).arg(oldColumnName), "columnName", newColumnName);

		if (!success){

			db->rollbackTransaction();
			AMErrorMon::alert(0, AMDBUPGRADESUPPORT_COULD_NOT_UPDATE_AMDBOBJECTTYPE_TABLES, "Could not update the AMDbObjectTypes associated tables.");
			return false;
		}
	}

	else{

		db->rollbackTransaction();
		AMErrorMon::alert(0, AMDBUPGRADESUPPORT_DUPLICATE_COLUMNS_IN_AMDBOBJECTTYPE_TABLES, "Somehow there are duplicate columns in the table.");
		return false;
	}

	db->commitTransaction();

	return true;
}
