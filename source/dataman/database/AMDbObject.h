/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AM_DBOBJECT_H
#define AM_DBOBJECT_H

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QSet>
#include <QImage>
#include <QBuffer>

#include "acquaman.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectDefinitions.h"

#define AMDBOBJECT_ERROR_STORING_CHILD_OBJECT -47
#define AMDBOBJECT_3D_POINT_MISSING_3_NUMBERS -57
#define AMDBOBJECT_3D_POINT_MISSING_3_NUMBERS_IN_AMQVECTOR3DVECTOR -277009
#define AMDBOBJECT_ERROR_LOADING_OBJECT_TO_CREATE_THUMBNAILS -1313
#define AMDBOBJECT_ERROR_SAVING_THUMBNAILS -1314
#define AMDBOBJECT_ERROR_STORING_UPDATED_THUMBNAIL_COUNT_AND_FIRST_ID -1315
#define AMDBOBJECT_CANNOT_START_TRANSACTION_TO_SAVE_THUMBNAILS -1495
#define AMDBOBJECT_CANNOT_COMPLETE_TRANSACTION_TO_SAVE_THUMBNAILS -1496
#define AMDBOBJECT_CANNOT_COMPLETE_TRANSACTION_TO_SAVE_OBJECT -1497
#define AMDBOBJECT_CANNOT_START_TRANSACTION_TO_SAVE_OBJECT -1498

#define AMDBOBJECT_CANNOT_STORE_TO_DB_INVALID_DB -277001
#define AMDBOBJECT_CANNOT_STORE_TO_DB_CLASS_NOT_REGISTERED -277002
#define AMDBOBJECT_CANNOT_STORE_TO_DB_INSERT_OR_UPDATE_FAILED -277003
#define AMDBOBJECT_CANNOT_LOAD_FROM_DB_INVALID_ID -277004
#define AMDBOBJECT_CANNOT_LOAD_FROM_DB_CLASS_NOT_REGISTERED -277005
#define AMDBOBJECT_CANNOT_LOAD_FROM_DB_NO_VALUES_RETRIEVED_FROM_TABLE -277006
#define AMDBOBJECT_CANNOT_LOAD_FROM_DB_AMDBOBJECTLIST_TABLE_LOCATION_INVALID -277007
#define AMDBOBJECT_CANNOT_LOAD_FROM_DB_BAD_DB_REDIRECT -277008
#define AMDBOBJECT_CANNOT_LOAD_FROM_DB_AMCONSTDBOBJECTLIST_TABLE_LOCATION_INVALID -277009
#define AMDBOBJECT_CANNOT_LOAD_FROM_DB_BAD_CONSTDB_REDIRECT -277010

#define AMDBOBJECT_DEBUG_OUTPUT 881001

/// Thumbnails are fast little blobs of data used as icons or images to visually represent AMDbObjects.
class AMDbThumbnail {
public:
	/// The thumbnail types currently supported are \c InvalidType, which is a blank thumbnail, and \c PNGType, which is a PNG image (Recommended size 240 x 180 px).
	/*! \todo Future thumbnail types could include vector formats and plot-based data formats */
	enum ThumbnailType { InvalidType, PNGType };

	/// Default constructor
	virtual ~AMDbThumbnail();
	AMDbThumbnail(const QString& Title = QString(), const QString& Subtitle = QString(), ThumbnailType Type = InvalidType, const QByteArray& ThumbnailData = QByteArray());

	/// This constructor takes an image of any size and saves it as a PNG type. (It will be saved at the current size of the image, so if you want to save at a reduced size, pass in image.scaledToWidth(240) or similar.)
	AMDbThumbnail(const QString& Title, const QString& Subtitle, const QImage& image);


	QString title, subtitle;
	ThumbnailType type;
	QByteArray thumbnail;

	QString typeString() const;
};

/// This event class contains completed thumbnails that might have been generated in another thread. It's used to pass the thumbnails out of AMDbObject::updateThumbnailsInSeparateThread() back to the main thread, where they will be updated in the database.
class AMDbThumbnailsGeneratedEvent : public QEvent {

public:
	/// Constructor
	virtual ~AMDbThumbnailsGeneratedEvent();
	AMDbThumbnailsGeneratedEvent(const QList<AMDbThumbnail>& thumbnails, AMDatabase* db, const QString& dbTableName, int dbObjectId, bool neverSavedHereBefore);

	/// The list of thumbnails that were rendered
	QList<AMDbThumbnail> thumbnails;
	/// The database where the object and thumbnails should be found
	AMDatabase* db;
	/// The name of the table in the database where the object is found
	QString dbTablename;
	/// The id of the object in that table.
	int dbObjectId;
	/// An optimization to signal that the object has never been saved in this table before
	bool neverSavedHereBefore;
};

class AMDbObjectInfo;
class AMDbLoadErrorInfo;


/// This is the base class for all persistent user-data objects that can be stored in the database.  A generic AMScan inherits from this class.  (This class is re-entrant but not thread-safe. You can have AMDbObjects in multiple threads, but you shouldn't access the same instance from multiple threads.)
/*! <b>Introduction to the AMDbObject persistent object system</b>

The AMDbObject system provides a way to make QObjects persistent, ie: storable and reloadable from a permanent database.  It is highly integrated with the Qt meta object system. Some of the features that set it apart from other C++ ORM (Object-Relational Management) systems:

- More than one database is supported: objects can be loaded out of one database, and stored into another.
- No special syntax or external files are required to specify which member variables should be persistent (the  "data definition model"), or create the database tables. The fields you wish to store can be identified within the class definition by simply declaring then as QProperties with the Q_PROPERTY macro.  Calling AMDbObjectSystem::registerType<Class>() will register the class and create the tables if required.
- Dynamic loading of an object, without knowing its type. You can call AMDbObjectSystem::loadObject() and receive a fully-initialized pointer to whatever object was stored at the specified location.
- Objects of different classes can share a storage table, if desired. At the expense of having unused/null columns, this can make searching the database for similar objects much easier.
- The database can still be accessed using raw SQL. This has the drawback of allowing stored objects to be modified without control, but it allows for high-performance searching and updates when required.
- Explicit support is provided for composition of AMDbObject subclasses: a class which contains members which are AMDbObjects can reload them automatically as it is loaded from the database.
- \todo Explicit support for One-to-one, many-to-one and one-to-many, and many-to-many relationships


<b>Database layout of AMDbObjects</b>

In most cases, all objects of a certain class are stored in their own table. The table has columns for each member variable.  Objects are uniquely identified by their row (primary key), which can be retrieved with AMDbObject::id().

The types of member variables which may be stored are:

	- Integers
	- Floating point numbers
	- QStrings
	- QDates, QTimes, and QDateTimes
	- Any other QVariant types will attempt to be stored as strings, using the toString() and fromString() operations.

List member variables (specifically, QStringList, AMIntList, and AMDoubleList) can be stored within a single table cell. (Defined separators are used to combine the list values into a string representation, and extract them back again.) More complex lists require special handling, and sometimes a separate table.

If specified, classes can share a database table with other classes. They do this by specifying the class name of an already-registered class whose table they want to share.

<b>Creating a persistent class definition</b>

To add persistent functionality to your class:

- Inherit from AMDbObject, and declare the Q_OBJECT macro.
- Declare properties (using the Q_PROPERTY) macro for all the fields you want to be persistent. Fields with only a READ method will stored in the database, but not reloaded from it. The name of the property becomes the name of the database column.
	\bug This introduces some restrictions on the allowed property names -- find a way to catch this! for ex: "value" is not allowed as a property name.

- You can use the Q_CLASSINFO macro to specify additional characteristics for each field, in the form of semicolon-separated 'key=value' pairs:
\code
Q_CLASSINFO("propertyName", "keyword1=value1;keyword2=value2;keyword3=value3...")
\endcode
	where supported keywords are:
	- \c doNotStore: if equal to "true", does not store this property in the database. (Also implies doNotLoad=true)
	- \c doNotLoad: if equal to "true", does not set this property when re-loading an object from the database.
	- \c hidden: if equal to "true", this property should not be user-visible in default tables created on the database.
	- \c createIndex: if equal to "true", creates an index on this column in the table. (You should understand the performance implications of doing this. In short, it will make searches based on this column's values much faster, but slow down inserts and deletes, and take extra space in the database.

	If unspecified, the default value for all keywords is false. Note that subclasses inherit the base class definitions, unless re-defined.

- You can use the Q_CLASSINFO macro to specify characteristics for the entire object, also in the form of semicolon-separated 'key=value' pairs:
\code
Q_CLASSINFO("AMDbObject_Attributes", "keyword1=value1;keyword2=value2;...")
\endcode
	where the supported keywords are:
	- \c description: if provided, gives a human-readable string description for this class. (ex: "XRay Absorption Scan").
	- \c version: if provided, gives the version number (integer) of this class, for the purpose of software upgrades. [This is currently unused, but might be in the future].  If not specified, the default version is 1.  If you upgrade a class for which users have existing stored objects in their database, increase the version number (> 1), so that existing databases know that they must be upgraded.  One problem: up'ing the version number of a base class may or may not up the version number of a subclass, depending on whether the subclass re-defines the AMDbObject_Attributes class info. Therefore, this version number is currently unused; we use a different upgrade technique for now.
	- \c doNotReuseIds: if equal to "true", the auto-increment property on the primary key will be set so that new objects will never reuse the ids of old deleted ones.  Otherwise, after a table has had rows created and deleted, the ids of previously deleted rows may be re-used.
	- \c shareTableWithClass=[\c className]: if defined, uses the same database table for storage as [\c className].  Note that [\c className] must be registered with the AMDbObject system prior to registering this class.

	Note that subclasses inherit the base class definition for the whole attribute string, unless re-defined.  You can only specify the AMDbObject_Attributes once per class; multiple Q_CLASSINFO definitions are not allowed.


- Register the class with the system by calling AMDbObjectSupport::s()->registerClass<Class>() at runtime.  It's harmless to register a class multiple times, but it must be registered before calling storeToDb() or loadFromDb().
\code
AMDatabase* myWorkingDatabase;
// The following two lines can be done in either order:
AMDbObjectSupport::s()->registerDatabase(myWorkingDatabase);
AMDbObjectSupport::s()->registerClass<MyDbObject>();
\endcode

- Finally, all database objects have the optional functionality of providing one or more thumbnails to describe themselves. If you want to have non-blank thumbnails, you must provide thumbnailCount() and thumbnail(int index). The default is to have no thumbnails.

<b>Storing and re-loading persistent objects</b>

To store an AMDbObject in a database, simply call
\code
AMDatabase* myWorkingDatabase;
MyDbObject bob;
bob.setSomeProperty("foo");

bob.storeToDb(myWorkingDatabase);
\endcode
If the object has previously been stored in or loaded from the given database, it will have a valid id(), and be re-saved in the same location (sql UPDATE).  If the object has never been stored in a database before (id() < 1), or has been loaded from a different database (database() != the given database), then it will be added as a new object (sql INSERT).

You can restore an object from the database (ie: copy its values from the stored version to the in-memory object) by calling
\code
MyDbObject bob;
bob.loadFromDb(myWorkingDatabase, id);
\endcode
where \c id is the row to load the object from.

If you want to reload an object from the database, but you don't know its exact detailed type, you can use the dynamic loader. It will create and loadFromDb() the appropriate object for a given database, id, and table name:
\code
AMDbObject* newSomeKindaObject = AMDbObjectSupport::s()->createAndLoadObjectAt(myWorkingDatabase, tableName, id);
\endcode
You can then use qobject_cast<>() or type() to test the type of the newly-created object.

\note This functionality depends on the detailed class providing either a default constructor that requires no arguments

 In either case, the constructor must be declared with the Q_INVOKABLE flag. It's recommended that all AMDbObjects provide one or another of these constructors.

\todo how to re-implement the modified() and metaDataChanged() signal, now that we're using the property system, instead of setMetaData()?


<b>Composition of AMDbObjects</b>

Special handling of two property types provides support for composite AMDbObjects that "own" or "have" other AMDbObjects as member variables.  (Note that this functionality is provided for ownership situations only; it is distinct from the concept of association (ex: one-to-one, one-to-many, or many-to-many, such as between Scans and Experiments).  The distinction here is that a scan object does not 'own' all of the experiments it is part of, nor does an experiment 'own' all the scans it contains. Association relationships must be handled separately.)

To automatically store AMDbObject member variables, declare a Q_PROPERTY of the type AMDbObject*.  The read function should simply return a pointer to your member object.

When you call storeToDb() on the parent object,
- the read property is used to access a pointer to the member object. If the pointer is not 0, and the object has been modified(), we call storeToDb() on the member object as well.  Then we store the table name and storage id of the member object, so that it can be reloaded later.

Calling loadFromDb() could behave in two different ways:
	- The read property is used again to access a pointer to the member object. If it receives a valid pointer, and the type() of the existing object matches the type of the stored object, we call loadFromDb() on the existing object. setProperty() is never called.
	- If the read property returns a null pointer, or if the type() of the existing object does NOT match the type of the object stored in the database, we cannot call loadFromDb().  Instead, we create a brand new object using AMDbObjectSupport::s()->createAndLoadObjectAt() based on the stored object. Then we call setProperty() with a pointer to the newly created object.  (Note that if createAndLoadObjectAt() fails, setProperty() will pass in a null pointer, and you should check for this.)  \note It's the responsibility of the setProperty() write function to delete the old existing object (if required), take ownership of the new object, and re-connect any signal/slot connections to the newly created object.

	In normal usage, where the type of the member AMDbObject is constant, the second version of the loadFromDb() behaviour will only occur when re-loading the parent object from the database for the first time (if the member object hasn't been created by the constructor, and the read property returns a null pointer). In all subsequent storeToDb()/loadFromDb() calls, the existing member object will simply be stored and loaded transparently with the parent.


To store sets of AMDbObject member variables, you can declare a Q_PROPERTY of the type AMDbObjectList (ie: QList<AMDbObject*>).  The read function should return a list of pointers to your member objects.

storeToDb() and loadFromDb() work the same as in the case of single variables.
	- The read property is used to access a list of pointers to member objects. Each valid object is storeToDb()'d, and the table and id locations for each are stored with the parent object for future reference. (Here we use an auxilliary table called 'MainTableName_propertyName' to remember the stored locations.)

	- loadFromDb() also starts by calling the read property function to get a list of the existing member objects.  If the number of existing objects matches the number of stored objects, AND all the existing objects are valid, AND the types match for all pairs of (existing,stored) objects, then loadFromDb() is called on each.
	- Otherwise, if there are any discrepencies, a whole new set of objects is created with AMDbObjectSupport::s()->createAndLoadObjectAt() based on the stored versions, and setProperty() is called with the list of new objects.  (Note that if createAndLoadObjectAt() fails, there may be null pointers in this list.)  It's the responsibility of the setPropert() write function to delete the old existing objects (if required) and re-establish connections to the new objects.   Note that it is NOT possible to reuse some of the existing objects in the list, but not others.

*/
class AMDbObject : public QObject
{
	Q_OBJECT

	/// AMDbObject provides a single property: a name() for this instance.
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

	// QObject's have one property to start: objectName().  Don't store QObject::objectName() by default
	Q_CLASSINFO("objectName", "doNotStore=true")



public:
	/// Default Constructor
	Q_INVOKABLE explicit AMDbObject(QObject *parent = 0);
	/// Copy constructor.  QObject parent/child relationships are NOT copied, but the essential characteristics (id, database, and modified state) of the AMDbObject are.  Making a copy will create an independent instance in memory. However, if the original has been previously saved to or loaded from the database, both the original and the copy will store/restore to the same location in the database (ie: they refer to the same persistent object). If you want the copy to be an independent database object, you need to call dissociateFromDb() next.
	/*! If the original has never been successfully saved or loaded (ie: id() and database() return 0) then the two instances remain fully independent objects (both in memory, and in the database after calling storeToDb() for the first time.)

	  The parent QObject is not set when using this copy constructor; the copy's parent() will be 0.  If you want the copy to share the same parent(), you must call QObject::setParent() afterward.
	  */
	AMDbObject(const AMDbObject& original);
	/// Destructor.
	virtual ~AMDbObject();

	/// Assignment operator. QObject parent/child relationships are NOT copied, but the essential characteristics (id, database, and modified state) of the AMDbObject are.  Making a copy will create an independent instance in memory. However, if the original has been previously saved to or loaded from the database, both the original and the copy will store/restore to the same location in the database (ie: they refer to the same persistent object). If you want the copy to be an independent database object, you need to call dissociateFromDb() next.
	/*! If the original has never been successfully saved or loaded (ie: id() and database() return 0) then the two instances remain fully independent objects (both in memory, and in the database after calling storeToDb() for the first time.)

	  The parent QObject is not set when using this copy operator; the copy's parent() will be 0.  If you want the copy to share the same parent(), you must call QObject::setParent() afterward.
	  */
	AMDbObject& operator=(const AMDbObject& other);

	/// Returns an object's unique id
	int id() const { return id_; }

	/// Returns a user given name for this instance.
	virtual QString name() const { return name_; }

	// These functions provide support for storing and retrieving from the database.
	// ===================================

	/// The type (class name) of this object (corresponding to the type of the most detailed subclass).
	/*! This will be stored in the database and used to figure out the real type of the object when loading*/
	QString type() const{
		// metaObject() is virtual, so this will produce the class name of the most detailed subclass.
		return this->metaObject()->className();
	}

	/// Return whether or not this object is currently being reloaded from the database
	bool isReloading() const;

	/// Returns whether or not this object is currently being stored to the database
	bool isStoring() const;


	/// returns the name of the database table where objects like this are stored.
	QString dbTableName() const;

	/// If this class has already been registered in the AMDbObject system, returns a pointer to the AMDbObjectInfo describing this class's persistent properties.  If the class hasn't been registered, returns 0
	const AMDbObjectInfo* dbObjectInfo() const;

	/// If an object has been loaded from a database (or created and stored to a database), this will tell you which database it's in. Returns 0 if this scan instance wasn't loaded out of a database.
	AMDatabase* database() const {
		return database_;
	}

	/// Load yourself from the database. (returns true on success)
	/*! This version loads all of the object's properties (which don't have the \c doNotStore or \c doNotLoad attributes set).  Detailed subclasses should re-implement this if they need to load anything that is not a QProperty.  When doing so, always call the base class implemention first.*/
	virtual bool loadFromDb(AMDatabase* db, int id);

	/// Store or update self in the database. (Returns true on success).  After storing, the object's id() will be set to match where it was stored.
	/*! This version saves all of the object's properties (which don't have the \c doNotStore attribute set).  Detailed subclasses should re-implement this if they need to save anything that is not a defined QProperty. When doing so, always call the base class implementation first.

	  Thumbnails are generated by default if the object specifies a non-zero thumbnailCount(). Set \c generateThumbnails to false to suppress this and omit creating thumbnails.
	  */
	virtual bool storeToDb(AMDatabase* db, bool generateThumbnails = true);

	/// Dissociate the database identify of an in-memory object.
	/** Dis-associates an in-memory object from any database object it might represent.  For example, if the object has been previously stored to the database, it will have a valid database() and id(); calling this function will reset both of those to 0, which means the in-memory object will no longer refer to the persistent object. You can then re-save the object to a new location by calling storeToDb().

		By default, this function also calls itself recursively on all children objects -- ie: properties of type AMDbObject* or AMDbObjectList.  This is according to our policy that children database objects should not be shared between parents.

		If this object has never been stored to the database (ie: database() and id() return 0...), then this function does nothing.
	*/
	virtual void dissociateFromDb(bool shouldDissociateChildren = true);

	/// Returns truen when this in-memory object has been modified from the version in the database (or when there is no version in the database).  Subclasses should help keep this correct by calling setModified(true) when their properties are changed.
	bool modified() const { return modified_; }

	/// Access the value of one of the AMDbObject_Attributes (by name).  If the attribute hasn't been set, returns an empty string.
	/*! Common attributes are:
	  - doNotReuseIds
	  - shareTableWithClass
	  - description
	  - version
	  */
	QString dbObjectAttribute(const QString& key) const;

	/// Access the value of one of the properties' database attributes. If the attribute hasn't been set, returns an empty string.
	/*! Common attributes are:
	  - doNotStore
	  - doNotLoad
	  - hidden
	  - createIndex
	*/
	QString dbPropertyAttribute(const QString& propertyName, const QString& key) const;



	// Thumbnail system:
	/////////////////////////////////

	/// Database Objects can have an arbitrary number of thumbnail images/plots. These are stored as binary data blobs directly inside the Db for fast access. This function indicates how many thumbnails are available/relevant.
	/*! Note that the thumbnails are stored in the database, but they are not stored inside the meta-data system for active AMDbObject instances, in order to reduce memory usage.*/
	virtual int thumbnailCount() const {
		return 0;
	}
	/// This returns a copy of the thumbnail data (for a given thumbnail). \c index must be less than thumbnailCount().  The base class returns an invalid (blank) thumbnail.
	virtual AMDbThumbnail thumbnail(int index) const {
		Q_UNUSED(index)
		return AMDbThumbnail();
	}
	/// Overload this to indicate that it is safe to (and the database system should) generate thumbnails in a separate thread after a storeToDb(). By default, this is disabled and there is no imposed requirement that database classes be thread-safe or reentrant.
	/*! Some database objects could take a long time to generate thumbnails; for performance, they might want to do this in another thread. Theferfore, the storeToDb() process can save the object without thumbnails, and then instruct it to be re-loaded in another thread to actually generate and store the thumbnails. However, it should only do that if the object's loadFromDb(), thumbnail(), and thumbnailCount() functions are full re-entrant; otherwise, this might be unsafe.
This virtual function should return true for AMDbObject classes where it is both safe and desirable to generate thumbnails in another thread. */
	virtual bool shouldGenerateThumbnailsInSeparateThread() const { return false; }

	QMap<QString, AMDbLoadErrorInfo*> loadingErrors() const;


signals:

	/// Emitted when this object is fully re-loaded from the database
	void loadedFromDb();
	/// Emitted when this object is fully stored to the database
	void storedToDb();
	/// Emitted when the modified() state changes. Indicates that this object is in-sync or out-of-sync with the database version.
	void modifiedChanged(bool isModified);

	/// Emitted when the name() changes
	void nameChanged(const QString& newName);

public slots:
	/// Sets user given name
	virtual void setName(const QString &name) {
		if(name_ != name){
			name_ = name;
			setModified(true);
			emit nameChanged(name_);
		}
	}



protected:

	/// Subclasses should call this to set or un-set the modified flag.  Handles emission of the modifiedChanged() signal when required.
	void setModified(bool isModified = true);


private:
	/// unique database id
	int id_;

	/// pointer to the database where this object came from/should be stored. (If known)
	AMDatabase* database_;

	/// The modified flag is true whenever the object's persistent data does not match what is stored in the database.  It is set automatically inside setProperty(). If you modify the persistent data in other ways, be sure to call setModified(true).
	bool modified_;

	/// stores the name property
	QString name_;

	/// This is a static helper function that will run in another thread to reload the object, generate, and save thumbnails after a db object is saved with storeToDb().
	/*! \c neverSavedHereBefore is an optimization for when we know there are no existing thumbnails.*/
	static void updateThumbnailsInSeparateThread(AMDatabase* db, int id, const QString& dbTableName, bool neverSavedHereBefore);
	/// This is a helper function used by storeToDb() to save the thumanils, in the current thread. It should only be called after the object has been stored in the main table and has a valid id() and database(). \c neverSavedHereBefore is an optimization for when we know there are no existing thumbnails.
	void updateThumbnailsInCurrentThread(bool neverSavedHereBefore);

	/// Holds whether this object is currently being reloaded from the database
	bool isReloading_;

	/// Holds whether this object is currently being stored to the database
	bool isStoring_;

	QMap<QString, AMDbLoadErrorInfo*> loadingErrors_;
};

/// This global function enables using the insertion operator to store objects in a database. It simply calls AMDbObject::storeToDb() with the given \c database.
/*!		For example: *Database::db() << myScan
Because Scan::storeToDb() is virtual, this version can be used properly for all sub-types of AMDbObject.
*/
AMDatabase& operator<<(AMDatabase& db, AMDbObject& s);

Q_DECLARE_METATYPE(AMDbObject*)

#endif // DBOBJECT_H
