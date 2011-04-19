/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMDbObjectSupport.h"

#include "dataman/AMDbObject.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaClassInfo>
#include "util/AMErrorMonitor.h"

#include <QStringBuilder>

#include <QDebug>

// fill the className, tableName, metaObject, columns, columnTypes, isVisible, isLoadable, and doNotReuseIds properties based on a prototype AMDbObject.
AMDbObjectInfo::AMDbObjectInfo(AMDbObject* prototype) {
	initWithMetaObject(prototype->metaObject());
}

// fill the className, tableName, metaObject, columns, columnTypes, isVisile, isLoadable, and doNotReuseIds properties based on a prototype object's QMetaObject. (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
AMDbObjectInfo::AMDbObjectInfo(const QMetaObject* classMetaObject) {
	initWithMetaObject(classMetaObject);
}

void AMDbObjectInfo::initWithMetaObject(const QMetaObject *classMetaObject) {
	metaObject = classMetaObject;
	className = metaObject->className();
	tableName = AMDbObjectSupport::tableNameForClass(metaObject);

	doNotReuseIds = ( AMDbObjectSupport::dbObjectAttribute(metaObject, "doNotReuseIds") == QString("true") );
	classDescription = AMDbObjectSupport::dbObjectAttribute(metaObject, "description");
	sharedTable = !AMDbObjectSupport::dbObjectAttribute(metaObject, "shareTableWithClass").isEmpty();

	bool versionSet;
	int versionInt = AMDbObjectSupport::dbObjectAttribute(metaObject, "version").toInt(&versionSet);
	version = versionSet ? versionInt : 1;

	int numProperties = metaObject->propertyCount();
	columnCount = 0;

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

		columnCount++;
		columns << propertyName;
		columnTypes << metaObject->property(i).userType();	// need userType() instead of type(), because the type() for all user-defined types would be simply QVariant::UserType.  Note that for normal types, userType() == type().
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

	// Retrieve the AMDbObjectAttribute for a given \c object and \c key
	QString dbObjectAttribute(const QMetaObject* mo, const QString& key) {
		int i = mo->indexOfClassInfo("AMDbObject_Attributes");
		if(i < 0) {
			//qDebug() << "AMDBOBJECT" << mo->className() << ": no dbobject attributes set";
			return QString();
		}
		QString allAttributes( mo->classInfo(i).value() );
		QStringList attributeList = allAttributes.split(QChar(';'), QString::SkipEmptyParts);// split attributes by semicolon
		// search for a matching attribute:
		for(int i=0; i<attributeList.count(); i++) {
			if(attributeList.at(i).trimmed().startsWith(key+"="))
				return attributeList.at(i).section(QChar('='), 1);// return section after "key=".
		}
		//qDebug() << "AMDBOBJECT" << mo->className() << ": could not find object attribute " << key;
		return QString();
	}

	// Retrieve an object's property attribute for a given \c object, \c propertyName, and \c key. Returns empty string if not set.
	QString dbPropertyAttribute(const QMetaObject* mo, const QString& propertyName, const QString& key) {
		int i = mo->indexOfClassInfo(propertyName.toAscii().constData());
		if(i < 0) {
			//qDebug() << "AMDBOBJECT" << mo->className() << ": no property attributes set for " << propertyName;
			return QString();	// property attributes not found for this property
		}
		QString allAttributes( mo->classInfo(i).value() );
		QStringList attributeList = allAttributes.split(QChar(';'), QString::SkipEmptyParts);// split attributes by semicolon
		// search for matching attribute:
		for(int i=0; i<attributeList.count(); i++) {
			if(attributeList.at(i).startsWith(key+"="))
				return attributeList.at(i).section(QChar('='), 1);// return section after "key=".
		}
		//qDebug() << "AMDBOBJECT" << mo->className() << ": could not find property attribute " << propertyName << key;

		return QString();
	}


	// Returns the name of the table that should be used to store this class. If the class is registered already, it returns the registered name. If it's not registered, but it has the dbObjectProperty 'shareTableWithClass' set to another registered class, then it returns the table used by that class. Otherwise, it provides a default table name of 'className' + '_table'.
	QString tableNameForClass(const QMetaObject* classMetaObject) {
		QString className = classMetaObject->className();

		// if we're registered already, let's just look this up in the registry.
		QHash<QString, AMDbObjectInfo>::const_iterator iInfo = registeredClasses()->find(className);
		if(iInfo != registeredClasses()->end())
			return iInfo.value().tableName;

		// ok, we're not registered. Instead, let's answer where we SHOULD be stored:

		// are supposed to use the table of another registered class?
		QString sharedClassName = dbObjectAttribute(classMetaObject, "shareTableWithClass");
		if( !sharedClassName.isEmpty() ) {
			iInfo = registeredClasses()->find(sharedClassName);
			if(iInfo != registeredClasses()->end())
				return iInfo.value().tableName;
		}
		// default is to use the class name (which is always unique) plus "_table":
		return className + "_table";
	}



	QString tableNameForClass(const QString& className) {
		QHash<QString, AMDbObjectInfo>::const_iterator iInfo = registeredClasses()->find(className);
		if(iInfo != registeredClasses()->end())
			return iInfo.value().tableName;

		return QString();
	}



	bool registerDatabase(AMDatabase* db) {
		// ensure type-metaInfo table
		if( db->ensureTable(	typeTableName(),
								QString("AMDbObjectType,tableName,description,version").split(','),
								QString("TEXT,TEXT,TEXT,INTEGER").split(','),
								false) )
			db->createIndex(typeTableName(), "AMDbObjectType");

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
		db->ensureTable(elementTableName(), QString("AMDbObjectType,thumbnailCount,thumbnailFirstId,symbol,name,atomicNumber").split(','), QString("TEXT,INTEGER,INTEGER,TEXT,TEXT,INTEGER").split(','));


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
		return true;	// \todo error checking on creating type table
	}

	bool getDatabaseReadyForClass(AMDatabase* db, const AMDbObjectInfo& info) {

		// have the tables already been created in this database for this class? check the types table:
		QList<int> foundRows = db->objectsMatching(typeTableName(), "AMDbObjectType", info.className );

		// Possibility 1: This type has never been stored in this database... need to create new table for it, create columns and indexes in that table, add row for this class in the type information table, and add column entries in the allColumns, visibleColumns, and loadColumns tables.
		/////////////////////////
		if(foundRows.count() == 0) {

			db->startTransaction();	// exiting this function for any error should roll back to here.

			// create table...
			// \bug attempts to re-create table when sharing tables with other already-registered objects...
			if ( !info.sharedTable && !ensureTableForDbObjects(info.tableName, db, !info.doNotReuseIds) ) {
				db->rollbackTransaction();
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, QString("Database support: There was an error trying to create a table in the database for class %1.").arg(info.className)));
				return false;
			}
			// go through properties and create columns for each, (with some exceptions...)
			for(int i=0; i<info.columns.count(); i++) {

				// if type of property is AMDbObject* (it 'owns' another AMDbObject), then actually create two columns: one for table where object is stored, one for id
				/* Not doing this... Keeping it simple and sticking with one column.
				if( info.columnTypes.at(i) == qMetaTypeId<AMDbObject*>() ) {
					if( !db->ensureColumn(info.tableName,
										  info.columns.at(i) + "_id",
										  "INTEGER")
						|| !db->ensureColumn(info.tableName,
											 info.columns.at(i) + "_table",
											 "TEXT")) {
						// allow failures on shared tables... the columns might have already been created for previous classes.
						/// \todo For more reliability, could ensure that _unique columns_ in shared-table classes have actually been created.
						if(!info.sharedTable) {
							db->rollbackTransaction();
							AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -2, QString("Database support: There was an error trying to create a column (%1) in the database for class %2.").arg(info.columns.at(i)).arg(info.className)));
							return false;
						}
					}
				}
				*/

				// if type of property is AMDbObjectList (ie: it 'owns' a set of other AMDbObjects), then don't create a column at all. Instead, create an auxilliary table.  Table name is our table name + "_propertyName".
				if( info.columnTypes.at(i) == qMetaTypeId<AMDbObjectList>() ) {
					QString auxTableName = info.tableName + "_" + info.columns.at(i);
					if( !db->ensureTable(auxTableName,
										 QString("id1,table1,id2,table2").split(','),
										 QString("INTEGER,TEXT,INTEGER,TEXT").split(','))
						|| !db->createIndex(auxTableName, "id1")
						|| !db->createIndex(auxTableName, "id2") ) {

						// allow failures on shared tables... the columns might have already been created for previous classes.
						/// \todo For more reliability, could ensure that _unique columns_ in shared-table classes have actually been created.
						if(!info.sharedTable) {
							db->rollbackTransaction();
							AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -16, QString("Database support: There was an error trying to create an auxiliary table (%1) in the database for class %2.").arg(auxTableName).arg(info.className)));
							return false;
						}
					}
				}

				// otherwise, handle as usual: create one column with the name and type of the property.
				else {
					if( !db->ensureColumn(info.tableName,
										  info.columns.at(i),
										  db->metaType2DbType((QVariant::Type)info.columnTypes.at(i))) ) {
						// allow failures on shared tables... the columns might have already been created for previous classes.
						/// \todo For more reliability, could ensure that _unique columns_ in shared-table classes have actually been created.
						if(!info.sharedTable) {
							db->rollbackTransaction();
							AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -2, QString("Database support: There was an error trying to create a column (%1) in the database for class %2.").arg(info.columns.at(i)).arg(info.className)));
							return false;
						}
					}

					// create index on column, if required.
					if(info.isIndexed.at(i)) {
						if( !db->createIndex(info.tableName, info.columns.at(i)) ) {
							if(!info.sharedTable) {
								AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, QString("Database support: There was an error trying to create an index (%1) in the database for class %2.").arg(info.columns.at(i)).arg(info.className)));
								db->rollbackTransaction();
								return false;
							}
						}
					}
				}
			}	// end of loop over properties

			// add to type table:
			QStringList typeTableCols;
			typeTableCols << "AMDbObjectType" << "tableName" << "description" << "version";
			QVariantList typeTableValues;
			typeTableValues << info.className << info.tableName << info.classDescription << info.version;
			int typeId = db->insertOrUpdate(0, typeTableName(), typeTableCols, typeTableValues);

			if(typeId < 1) {
				db->rollbackTransaction();
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -4, QString("Database support: There was an error trying to register the class '%1' in the database").arg(info.className)));
				return false;
			}

			// add entry for each column in allColumns, and (potentially) visibleColumns, loadColumns
			QStringList clist;
			clist << "typeId" << "columnName";
			QVariantList vlist;
			vlist << typeId << "colName";
			for(int i=0; i<info.columns.count(); i++) {	// loop over columns

				// lists of AMDbObjects use aux. tables, not columns. So no column entry should appear for these.
				if(info.columnTypes.at(i) == qMetaTypeId<AMDbObjectList>())
					continue;

				vlist[1] = info.columns.at(i);// takes on the name of this column

				bool success = db->insertOrUpdate(0, allColumnsTableName(), clist, vlist); // always add to the 'allColumns' table.

				if(info.isVisible.at(i) && info.columnTypes.at(i) != qMetaTypeId<AMDbObject*>())	// no matter what, AMDbObject* reference columns aren't user-visible. There's nothing user-meaningful about a 'tableName;id' string.
					success = success && db->insertOrUpdate(0, visibleColumnsTableName(), clist, vlist);

				if(info.isLoadable.at(i)) // if loadable, add to 'loadColumns' table.
					success = success && db->insertOrUpdate(0, loadColumnsTableName(), clist, vlist);

				if(!success) {
					db->rollbackTransaction();
					AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -5, QString("Database support: There was an error trying to register the class '%1' in the database").arg(info.className)));
					return false;
				}
			}

			db->commitTransaction();
			return true;
		}

		// Possibility 2: this class has already been installed in this database.
		////////////////////////////////////////////
		if( foundRows.count() == 1) {
			int id = foundRows.at(0);
			QStringList typeInfoCols;
			typeInfoCols << "AMDbObjectType" << "tableName" << "version";
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


	// upgrade an existing database from supporting an old version of a class to supporting a new version.
	bool upgradeDatabaseForClass(AMDatabase* db, const AMDbObjectInfo& info, int existingVersion) {
		Q_UNUSED(db)
		Q_UNUSED(info)
		Q_UNUSED(existingVersion)

		// \todo implement upgrade functionality...
		return true;
	}


	// Support table names: (AMDbObject___ prefix ensures they don't collide with potential class table names)
	QString thumbnailTableName() { return "AMDbObjectThumbnails_table"; }
	// name of the table that stores meta-type information for all the registered classes.
	QString typeTableName() { return "AMDbObjectTypes_table"; }
	QString allColumnsTableName() { return "AMDbObjectTypes_allColumns"; }
	QString visibleColumnsTableName() { return "AMDbObjectTypes_visibleColumns"; }
	QString loadColumnsTableName() { return "AMDbObjectTypes_loadColumns"; }


	bool ensureTableForDbObjects(const QString& tableName, AMDatabase* db, bool reuseDeletedIds) {

		return db->ensureTable(tableName, QString("AMDbObjectType,thumbnailCount,thumbnailFirstId").split(','), QString("TEXT,INTEGER,INTEGER").split(','), reuseDeletedIds);
	}



	// Separator used between strings when exporting a StringList to the database
	QString stringListSeparator() { return "|@^@|"; }
	// Separator used between items when exporting all other lists to the database (changed from comma to support french localizations which use une virgule for the decimal point. maybe this needs to be fully localized.)
	QString listSeparator() { return ";"; }


	// Temporary tables (to be generalized)
	///////////////////////
	QString elementTableName() { return "Elements"; }

	QString experimentEntriesTableName() { return "ObjectExperimentEntries"; }
	QString sampleElementEntriesTableName() { return "SampleElementEntries"; }
	QString controlSetEntriesTableName() { return "ControlSetEntries"; }
	///////////////////////////



	// Useful for database introspection, this returns the type() (ie: class name) of the object stored in database \c db, under table \c tableName, at row \c id.
	QString typeOfObjectAt(AMDatabase* db, const QString& tableName, int id) {

		if(!db || id<1)
			return QString();

		QVariant typeString = db->retrieve(id, tableName, "AMDbObjectType");
		if(typeString.isValid())
			return typeString.toString();
		else
			return QString();
	}

	// returns a pointer to the object info for a given class with \c className, or 0 if the class has not been registered in the database system.
	const AMDbObjectInfo* objectInfoForClass(const QString& className) {
		QHash<QString, AMDbObjectInfo>::const_iterator iInfo = registeredClasses()->find(className);
		if(iInfo != registeredClasses()->end())
			return &(iInfo.value());

		return 0;
	}

	// Useful for database introspection, this creates and dynamically loads an object stored in database \c db, under table \c tableName, at row \c id. You can use qobject_cast<>() or type() to find out the detailed type of the new object.  Returns 0 if no object found.
	/* Ownership of the newly-created object becomes the responsibility of the caller. */
	AMDbObject* createAndLoadObjectAt(AMDatabase* db, const QString& tableName, int id) {

		QString className = typeOfObjectAt(db, tableName, id);
		if(className.isEmpty()) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -90, QString("[AMDbObjectSupport] Could not load the object with ID %1 from the table '%2', because we couldn't determine the type of the object.").arg(id).arg(tableName)));
			return 0;
		}

		const AMDbObjectInfo* objInfo = objectInfoForClass(className);
		if(!objInfo) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -91, QString("[AMDbObjectSupport] Could not load the object with ID %1 from the table '%2', because the class '%3' hasn't yet been registered in the database system.").arg(id).arg(tableName).arg(className)));
			return 0;
		}

		// this should never happen, so we won't bother to check. But if an object was somehow stored in the wrong table, AMDbObject::loadFromDb() will fail.
		// if(tableName != objInfo->tableName)
		// return 0;

		const QMetaObject* mo = objInfo->metaObject;

		// first try a constructor that takes a database and an id. We assume this one calls loadFromDb() automatically.
		AMDbObject* newObject;

		newObject = qobject_cast<AMDbObject*>( mo->newInstance(	Q_ARG(AMDatabase*, db),
																Q_ARG(int, id)) );
		if(newObject)
			return newObject;

		// now try a default constructor taking no arguments:
		newObject = qobject_cast<AMDbObject*>( mo->newInstance() );
		if(newObject) {
			if(newObject->loadFromDb(db, id))
				return newObject;
			else
				delete newObject;	// loading failed, and we're not going to return anything. Make sure not to leak the newly-created object.
		}

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -93, QString("[AMDbObjectSupport] Could not load the object with ID %1 from the table '%2', because there are no suitable constructors.").arg(id).arg(tableName)));
		return 0;
	}




	QSqlQuery select(AMDatabase* db, const QString& className, const QString& columnNames, const QString& whereClause) {

		return db->select(tableNameForClass(className), columnNames, whereClause);
	}

}	// END OF NAMESPACE AMDbObjectSupport

