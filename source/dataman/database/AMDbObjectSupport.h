/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMDBOBJECTSUPPORT_H
#define AMDBOBJECTSUPPORT_H

#include <QString>
#include <QMetaObject>
#include <QHash>
#include <QSet>
#include "acquaman.h"
#include "dataman/database/AMDatabase.h"
#include "util/AMErrorMonitor.h"


class AMDbObject;

/// This value class contains essential information about a registered AMDbObject class: the class name, database table name, and a pointer to the class's static meta-object
class AMDbObjectInfo {
public:
	/// The default constructor creates an invalid AMDbObject.
	AMDbObjectInfo() {
		metaObject = 0;
	}
	/// fill the className, tableName, metaObject, columns, columnTypes, isVisible, isLoadable, and doNotReuseIds properties based on a prototype AMDbObject.
	AMDbObjectInfo(AMDbObject* prototype);
	/// fill the className, tableName, metaObject, columns, columnTypes, isVisile, isLoadable, and doNotReuseIds properties based on a prototype object's QMetaObject. (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMDbObjectInfo(const QMetaObject* classMetaObject);

	/// Indicates this AMDbObjectInfo represents a valid object.
	bool isValid() const { return (metaObject != 0); }

	QString className;	///< the class name (C++ type name) of these objects
	QString tableName;	///< table used to store these objects
	bool sharedTable;	///< true if this class does not create a table of its own, but shares another class's table. (Note: this is only true for secondary classes that share an existing table; NOT for the first/original class which makes its own.)
	const QMetaObject* metaObject;	///< QMetaObject pointer with the complete class meta-information. The remaining values can be determined by parsing the metaObject's classInfo parameters, but are stored here for efficiency.

	QString classDescription;	///< human-readable description for this class. Defined with AM_DBOBJECTINFO("description=_____")
	int version;///< The database version number of this class. If you upgrade

	bool doNotReuseIds;	///< indicates that row ids should not be reused when objects are deleted.

	int columnCount; ///< number of fields/columns that are stored in the db. The following lists all have a count() of columnCount.
	QStringList columns;	///< names of all the persistent fields/columns stored for this class
	AMIntList columnTypes;///< for all fields, the QVariant::Type type of the stored data
	QList<bool> isVisible;///< for all fields, whether this column should be exposed to users in default table views.
	QList<bool> isLoadable;///< for all fields, whether the database value should be restored when the object is re-loaded from the database.  If false, the object's value will be stored to the db in storeToDb(), but not re-loaded when calling loadFromDb().
	QList<bool> isIndexed; ///< for all fields, whether the column should be indexed for fast search access.

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject* classMetaObject);

};

/// The functions in the AMDbObjectSupport namespace provides support for registering AMDbObject classes into the AMDbObject system.
/*!
When you register a new class with registerClass<Class>(), the class is added to the class registry, and tables are made for it (if necessary) in all registered databases.

When you register a new database with registerDatabase(), it is added to the database registry. Tables are NOT created retroactively for previously registered classes, so its important to register all the databases before registering any classes.
*/

class AMDbObject;

namespace AMDbObjectSupport
{
	/// \todo hide these from the public interface.
	extern QHash<QString, AMDbObjectInfo> registeredClasses_;
	extern QSet<AMDatabase*> registeredDatabases_;



	/// Retrieve the AMDbObjectAttribute for a given \c object and \c key. Returns empty string if not set.
	QString dbObjectAttribute(const QMetaObject* object, const QString& key);

	/// Retrieve an object's property attribute for a given \c object, \c propertyName, and \c key. Returns empty string if not set.
	QString dbPropertyAttribute(const QMetaObject* object, const QString& propertyName, const QString& key);

	/// Returns the name of the table that should be used to store this class. If the class is registered already, it returns the registered name. If it's not registered, but it has the dbObjectProperty 'shareTableWithClass' set to another registered class, then it returns the table used by that class. Otherwise, it provides a default table name of 'className' + '_table'.
	QString tableNameForClass(const QMetaObject* object);

	/// Returns the name of the database table where this class is stored.  Only works for classes that are ALREADY REGISTERED using registerClass()
	QString tableNameForClass(const QString& className);

	/// Convenient alternative for tableNameForClass() when you have the class type (but not an instance)
	template <class T>
			QString tableNameForClass() {
		return tableNameForClass( &(T::staticMetaObject) );
	}


	/// returns a const pointer to the hash of registered AMDbObject classes
	const QHash<QString, AMDbObjectInfo>* registeredClasses();

	/// returns a pointer to the object info for a given class with \c className, or 0 if the class has not yet been registered in the database system.
	const AMDbObjectInfo* objectInfoForClass(const QString& classsName);
	/// \todo Overloads for objectInfoForClass<Class>().



	/// ensure that a database \c db is ready to hold objects of the class described by \c info
	bool getDatabaseReadyForClass(AMDatabase* db, const AMDbObjectInfo& info);

	/// upgrade an existing database from supporting an old version of a class to supporting a new version.
	bool upgradeDatabaseForClass(AMDatabase* db, const AMDbObjectInfo& info, int existingVersion);


	/// Ensure that a table exists with the required basic fields for holding any AMDbObject
	bool ensureTableForDbObjects(const QString& tableName, AMDatabase* db, bool reuseDeletedIds = true);


	/// register a new database with the object-database system. This must be done before registering any AMDbObject classes with registerClass<Type>().
	bool registerDatabase(AMDatabase* db);

	/// register a new class with the database system. This is all you need to do enable an AMDbObect subclass. Returns false if the initialization failed; true if it was completed successfully, or if the object is already registered.
	template <class T>
			bool registerClass() {
		// wish I had a way to do this without needing to create an instance. Can't just call
		const QMetaObject* mo = &(T::staticMetaObject);
		// can we?


		// is this a subclass of AMDbObject? (Or an AMDbObject itself?)
		const QMetaObject* superClass = mo;
		bool inheritsDbObject;
		do {
			inheritsDbObject = (superClass->className() == QString("AMDbObject"));
		}
		while( (superClass=superClass->superClass()) && inheritsDbObject == false );
		if(!inheritsDbObject)
			return false;	// can't register a non AMDbObject subclass.


		// is it already registered? return true.
		QString className(mo->className());
		if(registeredClasses_.contains(className)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("Database Support: The class '%1' has already been registered in the database. Skipping duplicate registration.").arg(className)));
			return true;
		}

		// T* instance = new T(); // this would force AMDbObject subclasses to have a default constructor.
		AMDbObjectInfo newInfo(mo);

		bool success = true;
		QSetIterator<AMDatabase*> iDatabases(registeredDatabases_);
		while(iDatabases.hasNext()) {
			success = success && getDatabaseReadyForClass(iDatabases.next(), newInfo);
		}

		if(success) {
			registeredClasses_.insert(className, newInfo);
			return true;
		}
		else
			return false;
	}


	// Support table names:
	/// Stores thumbnails for all AMDbObjects that use thumbnails
	QString thumbnailTableName();
	/// name of the table that stores meta-type information for all the registered classes.
	QString typeTableName();
	QString allColumnsTableName();
	QString visibleColumnsTableName();
	QString loadColumnsTableName();


	// Temporary tables (to be generalized)
	///////////////////////
	QString elementTableName();

	QString experimentEntriesTableName();
	QString sampleElementEntriesTableName();
	QString controlSetEntriesTableName();
	///////////////////////////


	/// Separator used between strings when exporting a StringList to the database
	QString stringListSeparator();
	/// Separator used between items when exporting all other lists to the database (changed from comma to support french localizations which use une virgule for the decimal point. maybe this needs to be fully localized.)
	QString listSeparator();



	/// Useful for database introspection, this returns the type() (ie: class name) of the object stored in database \c db, under table \c tableName, at row \c id.
	QString typeOfObjectAt(AMDatabase* db, const QString& tableName, int id);

	/// Useful for database introspection, this creates and dynamically loads an object stored in database \c db, under table \c tableName, at row \c id. You can use qobject_cast<>() or type() to find out the detailed type of the new object.  Returns 0 if no object found.
	/*! Ownership of the newly-created object becomes the responsibility of the caller. */
	AMDbObject* createAndLoadObjectAt(AMDatabase* db, const QString& tableName, int id);



	/// Convenience function for doing a select query on only some of the fields that an AMDbObject table has.
	QSqlQuery select(AMDatabase* db, const QString& className, const QString& columnNames, const QString& whereClause = QString());


	/// Overloaded version of select(), useful when you know the class type but don't have its name.
	template <class T>
	QSqlQuery select(AMDatabase* db, const QString& columnNames, const QString& whereClause = QString()) {
		QString className = (T::staticMetaObject)->className();
		return select(db, className, columnNames, whereClause);
	}

}

#endif // AMDBOBJECTSUPPORT_H
