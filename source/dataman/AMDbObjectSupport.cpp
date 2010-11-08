#include "AMDbObjectSupport.h"

#include "dataman/AMDbObject.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaClassInfo>
#include "AMErrorMonitor.h"

/// fill the className, tableName, metaObject, columns, columnTypes, isVisible, isLoadable, and doNotReuseIds properties based on a prototype AMDbObject.
AMDbObjectInfo::AMDbObjectInfo(AMDbObject* prototype) {
	initWithMetaObject(prototype->metaObject());
}

/// fill the className, tableName, metaObject, columns, columnTypes, isVisile, isLoadable, and doNotReuseIds properties based on a prototype object's QMetaObject. (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
AMDbObjectInfo::AMDbObjectInfo(const QMetaObject* classMetaObject) {
	initWithMetaObject(classMetaObject);
}

void AMDbObjectInfo::initWithMetaObject(const QMetaObject *classMetaObject) {
	metaObject = classMetaObject;
	className = metaObject->className();
	tableName = AMDbObjectSupport::tableNameForClass(metaObject);

	doNotReuseIds = ( AMDbObjectSupport::dbObjectAttribute(metaObject, "doNotReuseIds") == QString("true") );
	classDescription = AMDbObjectSupport::dbObjectAttribute(metaObject, "description");

	bool versionSet;
	int versionInt = AMDbObjectSupport::dbObjectAttribute(metaObject, "version").toInt(&versionSet);
	version = versionSet ? versionInt : 1;

	int numProperties = metaObject->propertyCount();

	for(int i=0; i<numProperties; i++) {
		QString propertyName = metaObject->property(i).name();

		// if we can't read this property, or if the doNotStore attribute has been set to 'true', do not store it. Ignore it completely from the AMDbObject system.
		bool doNotStore = !metaObject->property(i).isReadable() ||
						  (AMDbObjectSupport::dbPropertyAttribute(metaObject, propertyName, "doNotStore") == QString("true"));
		if(doNotStore)
			continue;

		bool hidden = (AMDbObjectSupport::dbPropertyAttribute(metaObject, propertyName, "hidden") == QString("true"));
		bool doNotLoad =  !metaObject->property(i).isWritable()
						  || (AMDbObjectSupport::dbPropertyAttribute(metaObject, propertyName, "doNotLoad") == QString("true"));
		bool createIndex = (AMDbObjectSupport::dbPropertyAttribute(metaObject, propertyName, "createIndex") == QString("true"));
		// previous behaviour: auto index for integers that end in xxxId.  This should be more general, but people need to specify it.
		/* Excerpt from AMDatabaseDefinition::registerType
		// Create indexes on INTEGER type columns that are used as foreign keys
		if(col.type == QVariant::Int && col.key.endsWith("Id"))
			db->createIndex(prototype->databaseTableName(), col.key);
			*/

		columns << propertyName;
		columnTypes << metaObject->property(i).type();
		isVisible << !hidden;
		isLoadable << !doNotLoad;
		isIndexed << createIndex;
	}
}

namespace AMDbObjectSupport {

	QHash<QString, AMDbObjectInfo> registeredClasses_;
	QSet<AMDatabase*> registeredDatabases_;


	const QHash<QString, AMDbObjectInfo>* registeredClasses() {
		return &registeredClasses_;
	}

	/// Retrieve the AMDbObjectAttribute for a given \c object and \c key
	QString dbObjectAttribute(const QMetaObject* mo, const QString& key) {
		int i = mo->indexOfClassInfo("AMDbObject_Attributes");
		if(i < 0)
			return QString();
		QString allAttributes( mo->classInfo(i).value() );
		QStringList attributeList = allAttributes.split(QChar(';'), QString::SkipEmptyParts);// split attributes by semicolon
		// search for a matching attribute:
		for(int i=0; i<attributeList.count(); i++) {
			if(attributeList.at(i).trimmed().startsWith(key+"="))
				return attributeList.at(i).section(QChar('='), 1);// return section after "key=".
		}
		return QString();
	}

	/// Retrieve an object's property attribute for a given \c object, \c propertyName, and \c key. Returns empty string if not set.
	QString dbPropertyAttribute(const QMetaObject* mo, const QString& propertyName, const QString& key) {
		int i = mo->indexOfClassInfo(propertyName.toAscii().data());
		if(i < 0)
			return QString();	// property attributes not found for this property
		QString allAttributes( mo->classInfo(i).value() );
		QStringList attributeList = allAttributes.split(QChar(';'), QString::SkipEmptyParts);// split attributes by semicolon
		// search for matching attribute:
		for(int i=0; i<attributeList.count(); i++) {
			if(attributeList.at(i).startsWith(key+"="))
				return attributeList.at(i).section(QChar('='), 1);// return section after "key=".
		}
		return QString();
	}


	/// Returns the name of the table that should be used to store this class. If the class is registered already, it returns the registered name. If it's not registered, but it has the dbObjectProperty 'shareTableWithClass' set to another registered class, then it returns the table used by that class. Otherwise, it provides a default table name of 'className' + '_table'.
	QString tableNameForClass(const QMetaObject* classMetaObject) {
		QString className = classMetaObject->className();

		// if we're registered already, let's just look this up in the registry.
		if( AMDbObjectSupport::registeredClasses()->contains(className) ) {
			const AMDbObjectInfo& info = (*AMDbObjectSupport::registeredClasses())[className];
			return info.tableName;
		}

		// ok, we're not registered. Instead, let's answer where we SHOULD be stored:

		// are supposed to use the table of another registered class?
		QString sharedClassName = dbObjectAttribute(classMetaObject, "shareTableWithClass");
		if( !sharedClassName.isEmpty() ) {
			if(AMDbObjectSupport::registeredClasses()->contains(sharedClassName)) {
				return ((AMDbObjectInfo&)(*AMDbObjectSupport::registeredClasses())[sharedClassName]).tableName;
			}
		}
		// default is to use the class name (which is always unique) plus "_table":
		return className + "_table";
	}




	bool registerDatabase(AMDatabase* db) {
		// ensure type-metaInfo table
		if( db->ensureTable(	typeTableName(),
							QString("className,tableName,typeDescription,version").split(','),
							QString("TEXT,TEXT,TEXT,INTEGER").split(','),
							false) )
			db->createIndex(typeTableName(), "className");

		// ensure supporting type tables: (These map types to column names: an entry for each type / field-name combintion)
		if( db->ensureTable(	allColumnsTableName(),
							QString("typeId,columnName").split(','),
							QString("INTEGER,TEXT").split(','),
							false) )
			db->createIndex(allColumnsTableName(), "typeId");

		if( db->ensureTable(	visibleColumnsTableName(),
							QString("typeId,columnName").split(','),
							QString("INTEGER,TEXT").split(','),
							false) )
			db->createIndex(visibleColumnsTableName(), "typeId");

		if( db->ensureTable(loadColumnsTableName(),
							QString("typeId,columnName").split(','),
							QString("INTEGER,TEXT").split(','),
							false) )
			db->createIndex(loadColumnsTableName(), "typeId");

		// This table stores thumbnails for all these object types.  It should not reuse ids, so that a set of thumbnails added will always have sequential ids.
		db->ensureTable(thumbnailTableName(), QString("objectId,objectTableName,number,type,title,subtitle,thumbnail").split(','), QString("INTEGER,TEXT,INTEGER,TEXT,TEXT,TEXT,BLOB").split(','), false);


		// temporary... this should all be cleaned up and moved and handled generically
		////////////////////////////
		db->ensureTable(elementTableName(), QString("typeId,thumbnailCount,thumbnailFirstId,symbol,name,atomicNumber").split(','), QString("INTEGER,INTEGER,INTEGER,TEXT,TEXT,INTEGER").split(','));


		// These tables provide links between experiments and user-data objects, and samples and elements.
		db->ensureTable(experimentEntriesTableName(), QString("objectId,experimentId").split(','), QString("INTEGER,INTEGER").split(','));
		db->createIndex(experimentEntriesTableName(), "objectId,experimentId");
		db->createIndex(experimentEntriesTableName(), "experimentId,objectId");

		db->ensureTable(sampleElementEntriesTableName(), QString("sampleId,elementId").split(','), QString("INTEGER,INTEGER").split(','));
		db->createIndex(sampleElementEntriesTableName(), "sampleId,elementId");
		db->createIndex(sampleElementEntriesTableName(), "elementId,sampleId");

		// This table provides optimized storage for the individual control fields in a control set
		db->ensureTable(controlSetEntriesTableName(), QString("csiId,name,ctrlValue,minimum,maximum,units,number").split(','), QString("INTEGER,TEXT,REAL,REAL,REAL,TEXT,INTEGER").split(','));
		db->createIndex(controlSetEntriesTableName(), "csiId");
		/////////////////////////////////


		registeredDatabases_.insert(db);
		return true;	/// \todo error checking on creating type table
	}

	bool getDatabaseReadyForClass(AMDatabase* db, const AMDbObjectInfo& info) {

		// have the tables already been created in this database for this class? check the types table:
		QList<int> foundRows = db->objectsMatching(typeTableName(), "className", info.className );

		// Possibility 1: This type has never been stored in this database... need to create new table for it, create columns and indexes in that table, add row for this class in the type information table, and add column entries in the allColumns, visibleColumns, and loadColumns tables.
		/////////////////////////
		if(foundRows.count() == 0) {

			db->startTransation();	// exiting this function for any error should roll back to here.

			// create table...
			/// \bug attempts to re-create table when sharing tables with other already-registered objects...
			if ( !ensureTableForDbObjects(info.tableName, db, !info.doNotReuseIds) ) {
				db->rollbackTransation();
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, QString("Database support: There was an error trying to create a table in the database for class %1.").arg(info.className)));
				return false;
			}
			// go through properties and create columns
			for(int i=0; i<info.columns.count(); i++) {

				// create column in table...
				if( !db->ensureColumn(info.tableName,
									  info.columns.at(i),
									  db->metaType2DbType((QVariant::Type)info.columnTypes.at(i))) ) {
					db->rollbackTransation();
					AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -2, QString("Database support: There was an error trying to create a column (%1) in the database for class %2.").arg(info.columns.at(i)).arg(info.className)));
					return false;
				}

				// create index on column, if required.
				if(info.isIndexed.at(i)) {
					if( !db->createIndex(info.tableName, info.columns.at(i)) ) {
						AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, QString("Database support: There was an error trying to create an index (%1) in the database for class %2.").arg(info.columns.at(i)).arg(info.className)));
						db->rollbackTransation();
						return false;
					}
				}
			}	// end of loop over properties

			// add to type table:
			QStringList typeTableCols;
			typeTableCols << "type" << "tableName" << "description" << "version";
			QVariantList typeTableValues;
			typeTableValues << info.className << info.tableName << info.classDescription << info.version;
			int typeId = db->insertOrUpdate(0, typeTableName(), typeTableCols, typeTableValues);

			if(typeId < 1) {
				db->rollbackTransation();
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -4, QString("Database support: There was an error trying to register the class '%1' in the database").arg(info.className)));
				return false;
			}

			// add entry for each column in allColumns, and (potentially) visibleColumns, loadColumns
			QStringList clist;
			clist << "typeId" << "columnName";
			QVariantList vlist;
			vlist << typeId << "colName";
			for(int i=0; i<info.columns.count(); i++) {	// loop over columns
				vlist[1] = info.columns.at(i);// takes on the name of this column
				bool success = db->insertOrUpdate(0, allColumnsTableName(), clist, vlist); // always add to the 'allColumns' table.
				if(info.isVisible.at(i))
					success = success && db->insertOrUpdate(0, visibleColumnsTableName(), clist, vlist);
				if(info.isLoadable.at(i))
					success = success && db->insertOrUpdate(0, loadColumnsTableName(), clist, vlist);

				if(!success) {
					db->rollbackTransation();
					AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -5, QString("Database support: There was an error trying to register the class '%1' in the database").arg(info.className)));
					return false;
				}
			}

			db->commitTransation();
			return true;
		}

		// Possibility 2: this class has already been installed in this database.
		////////////////////////////////////////////
		if( foundRows.count() == 1) {
			int id = foundRows.at(0);
			QStringList typeInfoCols;
			typeInfoCols << "type" << "tableName" << "version";
			QVariantList typeInfoValues = db->retrieve(id, typeTableName(), typeInfoCols);

			// check for matching version. If not matching, need to upgrade
			int storedVersion = typeInfoValues.at(2).toInt();
			if( storedVersion != info.version ) {
				return upgradeDatabaseForClass(db, info, storedVersion);
			}

			// check for matching table name. If not matching, the database has been corrupted somewhere.
			QString storedTableName = typeInfoValues.at(1).toString();
			if(storedTableName != info.tableName) {
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -6, QString("Database support: Your database stores '%1' objects in a table called '%2', but they should be stored under '%3'. This database is not compatible with the current version of Acquaman.").arg(info.className, storedTableName, info.tableName)));
				return false;
			}

			// otherwise, assuming future versions of this function adhere to creating the types table with a version #, and create the allColumns, loadColumns, and visibleColumns tables... we should be good here. Could check for matching columns, but that should be taken care of with the version system. Could check that the entries are created in the allColumns, visibleColumns, and loadColumns tables, but that would only fail if we have a bug in this function.
			return true;
		}

		// Possibility 3: more than one entry found for this class, in the type info table. This should never happen.
		else {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -6, QString("Database support: Multiple versions of the '%1' object were found in your database. This likely means that your database has been corrupted.").arg(info.className)));
			return false;
		}
	}


	/// upgrade an existing database from supporting an old version of a class to supporting a new version.
	bool upgradeDatabaseForClass(AMDatabase* db, const AMDbObjectInfo& info, int existingVersion) {
		Q_UNUSED(db)
		Q_UNUSED(info)
		Q_UNUSED(existingVersion)

		/// \todo implement upgrade functionality...
		return true;
	}


	/// Support table names:
	QString thumbnailTableName() { return "AMDbObjectThumbnails_table"; }
	/// name of the table that stores meta-type information for all the registered classes.
	QString typeTableName() { return "AMDbObjectTypes_table"; }
	QString allColumnsTableName() { return "AMDbObjectTypes_allColumns"; }
	QString visibleColumnsTableName() { return "AMDbObjectTypes_visibleColumns"; }
	QString loadColumnsTableName() { return "AMDbObjectTypes_loadColumns"; }


	bool ensureTableForDbObjects(const QString& tableName, AMDatabase* db, bool reuseDeletedIds) {

		return db->ensureTable(tableName, QString("type,thumbnailCount,thumbnailFirstId").split(','), QString("TEXT,INTEGER,INTEGER").split(','), reuseDeletedIds);
	}



	/// Separator used between strings when exporting a StringList to the database
	QString stringListSeparator() { return "|@^@|"; }
	/// Separator used between items when exporting all other lists to the database (changed from comma to support french localizations which use une virgule for the decimal point. maybe this needs to be fully localized.)
	QString listSeparator() { return ";"; }


	// Temporary tables (to be generalized)
	///////////////////////
	QString elementTableName() { return "Elements"; }

	QString experimentEntriesTableName() { return "ObjectExperimentEntries"; }
	QString sampleElementEntriesTableName() { return "SampleElementEntries"; }
	QString controlSetEntriesTableName() { return "ControlSetEntries"; }
	///////////////////////////

}

