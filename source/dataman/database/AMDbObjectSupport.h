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


#ifndef AMDBOBJECTSUPPORT_H
#define AMDBOBJECTSUPPORT_H

#include <QObject>
#include <QString>
#include <QMetaObject>
#include <QHash>
#include <QSet>
#include <QMutex>
#include <QReadWriteLock>

#include "acquaman.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectDefinitions.h"

#define AMDBOBJECTSUPPORT_ERROR_CREATING_TABLE -1
#define AMDBOBJECTSUPPORT_ERROR_CREATING_COLUMN -2
#define AMDBOBJECTSUPPORT_ERROR_CREATING_INDEX -3
#define AMDBOBJECTSUPPORT_ERROR_REGISTERING_CLASS_TYPE_TABLE -4
#define AMDBOBJECTSUPPORT_ERROR_REGISTERING_CLASS_HELPER_TABLES -5
#define AMDBOBJECTSUPPORT_ERROR_MULTIPLE_VERSIONS_OF_CLASS -6
#define AMDBOBJECTSUPPORT_CLASS_ALREADY_REGISTERED -7
#define AMDBOBJECTSUPPORT_DATABASE_ALREADY_REGISTERED -8
#define AMDBOBJECTSUPPORT_ERROR_INITIALIZING_AUXILIARY_TABLE -16
#define AMDBOBJECTSUPPORT_CANNOT_LOAD_OBJECT_CANNOT_DETERMINE_TYPE -90
#define AMDBOBJECTSUPPORT_CANNOT_LOAD_OBJECT_NOT_REGISTERED_TYPE -91
#define AMDBOBJECTSUPPORT_CANNOT_LOAD_OBJECT_LOAD_CALL_FAILED -92
#define AMDBOBJECTSUPPORT_CANNOT_LOAD_OBJECT_NO_SUITABLE_CONSTRUCTORS -93
#define AMDBOBJECTSUPPORT_ERROR_CREATING_COLUMN_FOR_UPGRADE -102
#define AMDBOBJECTSUPPORT_ERROR_CREATING_INDEX_FOR_UPGRADE -103
#define AMDBOBJECTSUPPORT_ERROR_WHILE_ATTEMPTING_UPGRADE -104
#define AMDBOBJECTSUPPORT_ERROR_UPDATING_SCHEMA_TABLE_FOR_UPGRADE -105
#define AMDBOBJECTSUPPORT_ERROR_INITIALIZING_AUXILIARY_TABLE_FOR_UPGRADE -106
#define AMDBOBJECTSUPPORT_ERROR_WHILE_CHECKING_UPGRADE -205
#define AMDBOBJECTSUPPORT_NOTIFYING_CLASS_UPGRADE_START 300
#define AMDBOBJECTSUPPORT_NOTIFYING_CLASS_UPGRADE_COMPLETE 301
#define AMDBOBJECTSUPPORT_ERROR_SAVING_THUMBNAILS -314
#define AMDBOBJECTSUPPORT_ERROR_STORING_UPDATED_THUMBNAIL_COUNT_AND_FIRST_ID -315
#define AMDBOBJECTSUPPORT_CANNOT_INSERT_DEFAULT_VALUE_FOR_UPGRADE -403
#define AMDBOBJECTSUPPORT_CANNOT_START_TRANSACTION_TO_SAVE_THUMBNAILS -495
#define AMDBOBJECTSUPPORT_CANNOT_COMPLETE_TRANSACTION_TO_SAVE_THUMBNAILS -496
#define AMDBOBJECTSUPPORT_RECEIVED_THUMBNAIL_UPDATE_FROM_UNREGISTERED_DATABASE -967

#define AMDBOBJECTSUPPORT_CANNOT_REGISTER_CLASS_NOT_DBOBJECT -278001
#define AMDBOBJECTSUPPORT_CANNOT_REGISTER_CLASS_CANNOT_READY_DATABASE -278002
#define AMDBOBJECTSUPPORT_CANNOT_REGISTER_DATABASE_PREVIOUS_CLASSES_PROBLEM -278003
#define AMDBOBJECTSUPPORT_CANNOT_WRITE_CLASS_INHERITANCE -278004

#define AMDBOBJECTSUPPORT_DEBUG_OUTPUT 880001

class AMDbObject;

/// This value class contains essential information about a registered AMDbObject class: the class name, database table name, and a pointer to the class's static meta-object
class AMDbObjectInfo {
public:
	/// The default constructor creates an invalid AMDbObject.
 	virtual ~AMDbObjectInfo();
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

	QString classDescription;	///< human-readable description for this class. Defined with Q_CLASSINFO("AMDbObject_Attributes", "description=Your Description")
	/// The database version number of this class. Defined with Q_CLASSINFO("AMDbObject_Attributes", "version=2").  If not specified, the default version is 1. Integers only.
	/*! One problem: up'ing the version number of a base class may or may not up the version number of a subclass, depending on whether the subclass re-defines the AMDbObject_Attributes class info. Therefore, this version number is currently unused; we use a different upgrade technique for now.*/
	int version;

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

class AMDbObjectRetainer : public QObject{
Q_OBJECT
public:
	/// Normal constructor take an AMDbObject and a pointer to another object that is watching it (probably its owner or creator to start with)
	AMDbObjectRetainer(AMDbObject *object, void *watcher, QObject *parent = 0);
	/// Normal constructor take an AMDbObject and a pointer to another object that is watching it (probably its owner or creator to start with)
	AMDbObjectRetainer(AMDbObject *object, QObject *watcher, QObject *parent = 0);

public slots:
	/// Adds another watcher
	void addWatcher(QObject *watcher);
	/// Removes a watcher. If this is the last watcher, then the AMDbObject we're storing and this object will have deleteLater called on them
	void removeWatcher(QObject *watcher);

	/// Adds another watcher that isn't a QObject
	void addVoidWatcher(void *watcher);
	/// Removes a watcher that isn't a QObject. If this is the last watcher, then the AMDbObject we're storing and this object will have deleteLater called on them
	void removeVoidWatcher(void *watcher);

signals:
	/// Emitted if the object is unexpectedly destroyed
	void objectPrematurelyDestroyed();

protected slots:
	/// If a watcher is a qobject, then we can make sure that it is removed as a watcher even if the programmer forgets to
	void onWatcherDestroyed(QObject *watcher);

	/// If (somehow) the object is destroyed outside of removeWatcher, we should probably notify the watchers and delete ourself
	void onObjectDestroyed();

protected:
	/// The object of interest
	AMDbObject *object_;
	/// The watcher for this object
	QList<void*> watchers_;
};


/// The functions in the AMDbObjectSupport class provide support for registering AMDbObject classes into the AMDbObject system. This class is thread-safe.
/*!
When you register a new class with registerClass<Class>(), the class is added to the class registry, and tables are made for it (if necessary) in all registered databases.

When you register a new database with registerDatabase(), it is added to the database registry. Tables are created retroactively for previously registered classes, so you can register databases and classes in any order.
*/
class AMDbObjectSupport : public QObject {

	Q_OBJECT

public:

	// Singleton class accessor:
	/////////////////////////////////
	/// AMDbObjectSupport is a singleton class; access the only instance of it using AMDbObjectSupport::s().
	static AMDbObjectSupport* s();

	// Public Functions: Registration
	////////////////////////////////

	/// register a new database with the object-database system. This must be done before trying to save or load any AMDbObject database objects into it/from it.
	bool registerDatabase(AMDatabase* db);

	/// register a new class with the database system. This is all you need to do enable an AMDbObect subclass. Returns false if the initialization failed; true if it was completed successfully, or if the object is already registered.
	template <class T>
	bool registerClass() {	return registerClass(&(T::staticMetaObject)); }
	/// register a new class with the database system. This is all you need to do enable an AMDbObect subclass. Returns false if the initialization failed; true if it was completed successfully, or if the object is already registered.
	bool registerClass(const QMetaObject* mo);



	// Public Functions: Database Introspection
	////////////////////////

	/// Returns the name of the table that should be used to store this class. If the class is registered already, it returns the registered name. If it's not registered, but it has the dbObjectProperty 'shareTableWithClass' set to another registered class, then it returns the table used by that class. Otherwise, it provides a default table name of 'className' + '_table'.
	QString tableNameForClass(const QMetaObject* object) const;
	/// Returns the name of the database table where this class is stored.  Only works for classes that are ALREADY REGISTERED using registerClass()
	QString tableNameForClass(const QString& className) const;
	/// Convenient alternative for tableNameForClass() when you have the class type (but not an instance)
	template <class T>
	QString tableNameForClass() const {
		return tableNameForClass( &(T::staticMetaObject) );
	}


	/// Useful for database introspection, this returns the type() (ie: class name) of the object stored in database \c db, under table \c tableName, at row \c id.
	static QString typeOfObjectAt(AMDatabase* db, const QString& tableName, int id);

	/// returns a pointer to the object info for a given class with \c className, or 0 if the class has not yet been registered in the database system.
	const AMDbObjectInfo* objectInfoForClass(const QString& classsName) const;
	/// \todo Overloads for objectInfoForClass<Class>().

	// Public Functions: Dynamic Loading for AMDbObjects
	////////////////////////////////////

	/// Useful for database introspection, this creates and dynamically loads an object stored in database \c db, under table \c tableName, at row \c id. You can use qobject_cast<>() or type() to find out the detailed type of the new object.  Returns 0 if no object found.
	/*! Ownership of the newly-created object becomes the responsibility of the caller. */
	AMDbObject* createAndLoadObjectAt(AMDatabase* db, const QString& tableName, int id) const;

	// Public Functions: Searching at the SQL level
	////////////////////////////////////

	/// Convenience function for doing a select query on only some of the fields that an AMDbObject table has. The returned query has been prepared but not executed, so you still need to call exec() on it.  You can also still bindValue()s before executing it.
	QSqlQuery select(AMDatabase* db, const QString& className, const QString& columnNames, const QString& whereClause = QString()) const;
	/// Overloaded version of select(), useful when you know the class type but don't have its name.
	template <class T>
	QSqlQuery select(AMDatabase* db, const QString& columnNames, const QString& whereClause = QString()) const {
		QString className = (T::staticMetaObject).className();
		return select(db, className, columnNames, whereClause);
	}

	// Internal Helper functions
	//////////////////////////////

	/// Helper function to check if a class inherits AMDbObject.  \c mo is the Qt QMetaObject representing the class.
	static bool inheritsAMDbObject(const QMetaObject* mo);

	/// Retrieve the AMDbObjectAttribute for a given \c object and \c key. Returns empty string if not set.
	static QString dbObjectAttribute(const QMetaObject* object, const QString& key);
	/// Retrieve an object's property attribute for a given \c object, \c propertyName, and \c key. Returns empty string if not set.
	static QString dbPropertyAttribute(const QMetaObject* object, const QString& propertyName, const QString& key);

	// Naming conventions:
	///////////////////////////

	/// Separator used between strings when exporting a StringList to the database
	static QString stringListSeparator();
	/// Separator used between items when exporting all other lists to the database (changed from comma to support french localizations which use une virgule for the decimal point. maybe this needs to be fully localized.)
	static QString listSeparator();
	static QString vectorSeparator();

	/// Stores thumbnails for all AMDbObjects that use thumbnails
	static QString thumbnailTableName();
	/// name of the table that stores meta-type information for all the registered classes.
	static QString typeTableName();
	static QString allColumnsTableName();
	static QString visibleColumnsTableName();
	static QString loadColumnsTableName();
	static QString upgradesTableName();


	// Temporary tables (to be generalized?)
	///////////////////////
	static QString elementTableName();

	static QString experimentEntriesTableName();
	static QString sampleElementEntriesTableName();
	static QString controlSetEntriesTableName();

protected:

	// Internal Helper functions
	//////////////////////////////

	/// returns a const pointer to the hash of registered AMDbObject classes
	const QHash<QString, AMDbObjectInfo>* registeredClasses() const { return &registeredClasses_; }

	/// ensure that a database \c db is ready to hold objects of the class described by \c info. Will call initializeDatabaseForClass() or upgradeDatabaseForClass() if required.
	static bool getDatabaseReadyForClass(AMDatabase* db, const AMDbObjectInfo& info);
	/// Helper function: Creates table and columns for a class which has never been stored in the database before.
	static bool initializeDatabaseForClass(AMDatabase* db, const AMDbObjectInfo& info);
	/// Helper function: checks if a class can be stored in the database as-is, or if the DB needs to be upgraded.
	/*! Confirms that all columns and auxiliary tables exist, although not necessarily in the order specified. (Since upgrading a base class will tack on the new base class members at the end of the original subclass members.) */
	static bool isUpgradeRequiredForClass(AMDatabase* db, const AMDbObjectInfo& info, int typeIdInDatabase);
	/// Helper function: Upgrades an existing database from supporting an old version of a class to supporting a new version.  If new columns exist, they will be created and filled with the default value specified in AMDbObjectInfo.
	static bool upgradeDatabaseForClass(AMDatabase* db, const AMDbObjectInfo& info, int typeIdInDatabase);



	/// Ensure that a table exists with the required basic fields for holding any AMDbObject
	static bool ensureTableForDbObjects(const QString& tableName, AMDatabase* db, bool reuseDeletedIds = true);




	/// Called with an AMThumbnailsGeneratedEvent to finish updating the thumbnails for an object in a database, when the thumbnails were generated in a separate thread. We want to make sure that the database _modifications_ are all done from the main thread, to avoid the possibility of Sqlite locking contention.
	/*! (The database is threadsafe, and from a thread-safety perspective we _can_ run both select and modify queries from any thread.  But when we do this, we run into the possiblity (depending on exact timing) that two threads may both be trying to obtain an exclusive lock, and as a result neither can commit / proceed with its query.  Therefore, we try to keep all database modification commands in the main thread.  If we had a database with full serializable connections (like MySQL) this wouldn't be a problem.*/
	virtual bool event(QEvent *);


	// Internal Variables
	////////////////////////////////

private:
	/// A registry of the classes that have been registered so far (at runtime) in the database system
	QHash<QString, AMDbObjectInfo> registeredClasses_;
	/// A list of the classes that have been registered so far (at runtime) in the database system. This holds the same information as registeredClassess.  However, the list is ordered in the sequence the classes were registered, and we need to be able to go through that same order when preparing a newly-registered database for previously-registered classes.
	QList<AMDbObjectInfo> registeredClassesInOrder_;
	/// A set of databases that have been registered so far (at runtime) with the database system
	QSet<AMDatabase*> registeredDatabases_;

	/// This is a singleton class, so the constructor is private.
 	virtual ~AMDbObjectSupport();
	AMDbObjectSupport() : QObject(), registryMutex_(QReadWriteLock::Recursive) {}
	/// Single instance of this class.
	static AMDbObjectSupport* instance_;

	// Thread Safety
	///////////////////////////////
	/// Protects access to instance_ in the singleton access method.
	static QMutex instanceMutex_;

	/// Protects the registry (registeredClasses_, registeredDatabases_) for thread-safe access
	mutable QReadWriteLock registryMutex_;

private slots:
	/// We retain pointers to databases so that when new classes are registered, we can add them retro-actively. Therefore, we need to know when these databases are no longer accessible.
	void onRegisteredDatabaseDeleted();

};

class AMDbLoadErrorInfo{
public:
 	virtual ~AMDbLoadErrorInfo();
	AMDbLoadErrorInfo(QString dbName = QString(), QString tableName = QString(), int dbId = -1);

	QString dbName() const;
	QString tableName() const;
	int dbId() const;

protected:
	QString dbName_;
	QString tableName_;
	int dbId_;
};

#endif // AMDBOBJECTSUPPORT_H
