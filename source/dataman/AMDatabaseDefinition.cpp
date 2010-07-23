#include "dataman/AMDatabaseDefinition.h"

void AMDatabaseDefinition::initializeDatabaseTables(AMDatabase* db) {
	/*
	db->ensureTable(objectTypeTableName(), QString("className,description,columnNames").split(','), QString("TEXT,TEXT,TEXT").split(','));
	db->ensureTable(objectTableName(), QString("typeId,dateTime,name,number,runId,thumbnailCount,thumbnailFirstId").split(','), QString("INTEGER,TEXT,TEXT,INTEGER,INTEGER,INTEGER,INTEGER").split(','));
	db->ensureTable(runTableName(), QString("dateTime,name").split(','), QString("TEXT,TEXT").split(','));
	db->ensureTable(experimentTableName(), QString("dateTime,name").split(','), QString("TEXT,TEXT").split(','));
	db->ensureTable(experimentEntriesTableName(), QString("objectId,experimentId").split(','), QString("INTEGER,INTEGER").split(','));

	db->ensureTable(databaseInformationTableName(), QString("key,value").split(','), QString("TEXT,TEXT").split(','));

	db->ensureTable(elementTableName(), QString("symbol,name,atomicNumber").split(','), QString("TEXT,TEXT,INTEGER").split(','));
	db->ensureTable(sampleTableName(), QString("name,dateTime,notes,thumbnailType,thumbnail").split(','), QString("TEXT,TEXT,TEXT,TEXT,BLOB").split(','));
	db->ensureTable(sampleElementEntriesTableName(), QString("sampleId,elementId").split(','), QString("INTEGER,INTEGER").split(','));
*/

	// This table registers the different data types we have:
	db->ensureTable(objectTypeTableName(), QString("className,description,columnNames,tableName").split(','), QString("TEXT,TEXT,TEXT,TEXT").split(','));

	// These tables hold AMDbObjects. The first is for all types of user-data objects. Runs, experiments, elements, and samples are stored separately.
	db->ensureTable(objectTableName(), QString("typeId,thumbnailCount,thumbnailFirstId").split(','), QString("INTEGER,INTEGER,INTEGER").split(','));
	db->ensureTable(runTableName(), QString("typeId,thumbnailCount,thumbnailFirstId,dateTime,name").split(','), QString("INTEGER,INTEGER,INTEGER,TEXT,TEXT").split(','));
	db->ensureTable(experimentTableName(), QString("typeId,thumbnailCount,thumbnailFirstId").split(','), QString("INTEGER,INTEGER,INTEGER").split(','));
	db->ensureTable(elementTableName(), QString("typeId,thumbnailCount,thumbnailFirstId,symbol,name,atomicNumber").split(','), QString("INTEGER,INTEGER,INTEGER,TEXT,TEXT,INTEGER").split(','));
	db->ensureTable(sampleTableName(), QString("typeId,thumbnailCount,thumbnailFirstId").split(','), QString("INTEGER,INTEGER,INTEGER").split(','));

	// This table stores thumbnails for all these object types:
	db->ensureTable(thumbnailTableName(), QString("objectId,objectTableName,number,type,title,subtitle,thumbnail").split(','), QString("INTEGER,TEXT,INTEGER,TEXT,TEXT,TEXT,BLOB").split(','));

	// This table stores key-value pairs of information about the database and about the user
	db->ensureTable(databaseInformationTableName(), QString("key,value").split(','), QString("TEXT,TEXT").split(','));

	// These tables provide links between experiments and user-data objects, and samples and elements.
	db->ensureTable(experimentEntriesTableName(), QString("objectId,experimentId").split(','), QString("INTEGER,INTEGER").split(','));
	db->ensureTable(sampleElementEntriesTableName(), QString("sampleId,elementId").split(','), QString("INTEGER,INTEGER").split(','));




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
void AMDatabaseDefinition::registerType(const AMDbObject* prototype, AMDatabase* db) {

	if(prototype->typeId(db)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("The class %1 has already been registered in the database. Skipping duplicate registration.").arg(prototype->type())));
		return;
	}

	QStringList columnNames;
	foreach(AMMetaMetaData col, prototype->metaDataAllKeys() ) {
		db->ensureColumn(objectTableName(), col.key, AMDatabase::metaType2DbType(col.type));
		columnNames << col.key;
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
