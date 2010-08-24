#include "dataman/AMDatabaseDefinition.h"

void AMDatabaseDefinition::ensureTableForDbObjects(const QString& tableName, AMDatabase* db) {

	db->ensureTable(tableName, QString("typeId,thumbnailCount,thumbnailFirstId").split(','), QString("INTEGER,INTEGER,INTEGER").split(','));
}

void AMDatabaseDefinition::initializeDatabaseTables(AMDatabase* db) {


	// This table registers the different data types we have:
	db->ensureTable(objectTypeTableName(), QString("className,description,columnNames,tableName").split(','), QString("TEXT,TEXT,TEXT,TEXT").split(','));

	// These tables hold AMDbObjects. The first is for all types of user-data objects. Runs, experiments, elements, and samples are stored separately.
	ensureTableForDbObjects(objectTableName(), db);
	ensureTableForDbObjects(runTableName(), db);
	ensureTableForDbObjects(experimentTableName(), db);
	ensureTableForDbObjects(detectorTableName(), db);
	ensureTableForDbObjects(controlSetTableName(), db);
	ensureTableForDbObjects(samplePlateTableName(), db);
	ensureTableForDbObjects(sampleTableName(), db);
	ensureTableForDbObjects(facilityTableName(), db);

	db->ensureTable(elementTableName(), QString("typeId,thumbnailCount,thumbnailFirstId,symbol,name,atomicNumber").split(','), QString("INTEGER,INTEGER,INTEGER,TEXT,TEXT,INTEGER").split(','));

	// This table stores thumbnails for all these object types:
	db->ensureTable(thumbnailTableName(), QString("objectId,objectTableName,number,type,title,subtitle,thumbnail").split(','), QString("INTEGER,TEXT,INTEGER,TEXT,TEXT,TEXT,BLOB").split(','));

	// This table stores key-value pairs of information about the database and about the user
	db->ensureTable(databaseInformationTableName(), QString("key,value").split(','), QString("TEXT,TEXT").split(','));

	// These tables provide links between experiments and user-data objects, and samples and elements.
	db->ensureTable(experimentEntriesTableName(), QString("objectId,experimentId").split(','), QString("INTEGER,INTEGER").split(','));
	db->createIndex(experimentEntriesTableName(), "objectId,experimentId");
	db->createIndex(experimentEntriesTableName(), "experimentId,objectId");
	db->ensureTable(sampleElementEntriesTableName(), QString("sampleId,elementId").split(','), QString("INTEGER,INTEGER").split(','));
	db->createIndex(sampleElementEntriesTableName(), "sampleId,elementId");
	db->createIndex(sampleElementEntriesTableName(), "elementId,sampleId");





	// add userName and databaseVersion to DatabaseInformation
	QVariant v1, v2;
	QList<const QVariant*> vlist;
	vlist << &v1 << &v2;

	QStringList clist;
	clist << "key" << "value";

	v1.setValue(QString("userName"));
	v2.setValue(AMUserSettings::userName);

	db->insertOrUpdate(0, databaseInformationTableName(), clist, vlist);

	v1.setValue(QString("databaseVersion"));
	v2.setValue(version());
	db->insertOrUpdate(0, databaseInformationTableName(), clist, vlist);

}

/// Database support function: registers a subclass of AMDbObject so that the database is ready to store and access objects of this type. This only needs to be called once for each object type that you want to store in the database.
/*! Expectations:
	- column names and types will be retrieved from the AMMetaMetaData information for this object, by calling AMDbObject::metaDataAllKeys().
	- column types (as indicated in the AMMetaMetaData entry) are either QVariant::Int, QVariant::Double, QVariant::ByteArray, or you're okay with them being saved as QVariant::String using the default QVariant::toString() implementation.  See AMDatabase::metaType2DbType() for more information on the types that will be used to convert/store entries in the database.
	- columns used as foreign keys to another table will be of type QVariant::Int, and their name will end in "Id" (ex: "runId"). This function will automatically create indexes on these columns to speed up relational joins.
	*/
void AMDatabaseDefinition::registerType(const AMDbObject* prototype, AMDatabase* db) {

	if(prototype->typeId(db)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("The class %1 has already been registered in the database. Skipping duplicate registration.").arg(prototype->type())));
		return;
	}

	QStringList columnNames;
	foreach(AMMetaMetaData col, prototype->metaDataAllKeys() ) {
		// Make sure that the column for this entry exists...
		db->ensureColumn(prototype->databaseTableName(), col.key, AMDatabase::metaType2DbType(col.type));
		columnNames << col.key;
		// Create indexes on INTEGER type columns that are used as foreign keys
		if(col.type == QVariant::Int && col.key.endsWith("Id"))
			db->createIndex(prototype->databaseTableName(), col.key);
	}

	QVariant v1, v2, v3, v4;
	QList<const QVariant*> vlist;
	vlist << &v1 << &v2 << &v3 << &v4;

	QStringList clist;
	clist << "className" << "description" << "columnNames" << "tableName";

	v1.setValue(prototype->type());
	v2.setValue(prototype->typeDescription());
	v3.setValue(columnNames.join(stringListSeparator()));
	v4.setValue(prototype->databaseTableName());

	db->insertOrUpdate(0,objectTypeTableName(), clist, vlist);

}

/// Return the type (ie: class name) of an object stored at 'id' in the main object table. (Returns empty string if not found.)
QString AMDatabaseDefinition::objectType(AMDatabase* db, int id) {
	// create a query on our database connection:
	QSqlQuery q = db->query();

	// Prepare the query. Todo: sanitize name?
	q.prepare(QString("SELECT className FROM %1 WHERE id IN (SELECT typeId FROM %2 WHERE id = ?)").arg(objectTypeTableName()).arg(objectTableName()));
	q.bindValue(0, id);

	// run query and return true if succeeded at finding id:
	if(q.exec() && q.first()) {

		return q.value(0).toString();
	}

	else {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -7, QString("could not find a scan at id %1.").arg(id)));
		return QString();
	}
}
