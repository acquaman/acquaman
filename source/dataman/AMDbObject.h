#ifndef ACQMAN_DBOBJECT_H
#define ACQMAN_DBOBJECT_H

#include "acquaman.h"
#include <QObject>
#include <QDateTime>
#include <QStringList>
#include "dataman/AMDatabase.h"

#include <QSet>
#include <QPixmap>
#include <QBuffer>

/*
/// Acquaman's flexible meta-data information system uses AMMetaMetaData to describe each piece of meta-data associated with a scan or database object.
class AMMetaMetaData {
public:
	AMMetaMetaData(QVariant::Type nType, const QString& nKey, bool nWriteable) : type(nType), key(nKey), writeable(nWriteable) {}
	AMMetaMetaData(AM::AcquamanType nType, const QString& nKey, bool nWriteable) : type((QVariant::Type)nType), key(nKey), writeable(nWriteable) {}

	QVariant::Type type;
	QString key;
	bool writeable;
};*/


/// Thumbnails are fast little blobs of data used as icons or images to visually represent AMDbObjects.
class AMDbThumbnail {
public:
	/// The thumbnail types currently supported are \c InvalidType, which is a blank thumbnail, and \c PNGType, which is a PNG image (Recommended size 240 x 180 px).
	/*! \todo Future thumbnail types could include vector formats and plot-based data formats */
	enum ThumbnailType { InvalidType, PNGType };

	/// Default constructor
	AMDbThumbnail(const QString& Title = QString(), const QString& Subtitle = QString(), ThumbnailType Type = InvalidType, const QByteArray& ThumbnailData = QByteArray());

	/// This constructor takes a pixmap of any size and saves it as a PNG type. (It will be saved at the current size of the pixmap, so if you want to save at a reduced size, pass in pixmap.scaledToWidth(240) or similar.)
	AMDbThumbnail(const QString& Title, const QString& Subtitle, const QPixmap& pixmap);

	QString title, subtitle;
	ThumbnailType type;
	QByteArray thumbnail;

	QString typeString() const;
};


// Macro for declaring AMDbObject object attributes. \c paramString is set of "key=value;key=value..." arguments
/* For example:
  \code
  AM_DBOBJECTINFO("description=Xray Absorption Scan; version=2; doNotReuseIds=true")
  \endcode

  It's a bummer this doesn't work. Unfortunately moc runs before the C++ pre-processor, so you can't put moc statements like Q_CLASSINFO() into a macro, or use macros inside Q_CLASSINFO statements.
  */
// #define AM_DBOBJECTINFO(paramString) Q_CLASSINFO("AMDbObject_Attributes", paramString)

// Macro for declaring AMDbObject attributes for the property \c propertyName. \c paramString is a set of "key=value;key=value..." arguments
/* For example:
  \code
  AM_DBPROPERTYINFO(numCounts, "doNotLoad=true;hidden=true")
  AM_DBPROPERTYINFO(sampleId, "createIndex=true")
  \endcode

  Also doesn't work : (
  */
// #define AM_DBPROPERTYINFO(propertyName, paramString) Q_CLASSINFO(#propertyName, paramString)

/// This is the base class for all persistent user-data objects that can be stored in the database.  A generic AMScan inherits from this class.
/*!
<b>Introduction to the AMDbObject persistent object system</b>

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
	- \c version: if provided, gives the version number (integer) of this class, for the purpose of software upgrades. If you upgrade a class for which users have existing stored objects in their database, increase the version number (> 1), so that existing databases know that they must be upgraded.  If unspecified, the default version is '1'.
	- \c doNotReuseIds: if equal to "true", the auto-increment property on the primary key will be set so that new objects will never reuse the ids of old deleted ones.  Otherwise, after a table has had rows created and deleted, the ids of previously deleted rows may be re-used.
	- \c shareTableWithClass=[\c className]: if defined, uses the same database table for storage as [\c className].  Note that [\c className] must be registered with the AMDbObject system prior to registering this class.

	Note that subclasses inherit the base class definition for the whole attribute string, unless re-defined.  You can only specify the AMDbObject_Attributes once per class; multiple Q_CLASSINFO definitions are not allowed.


- Register the class on every database you wish to use it in by calling AMDbObjectSupport::registerClass<Class>(AMDatabase* database) at runtime.  It's harmless to register a class multiple times, but it must be registered before calling storeToDb() or loadFromDb().
\code
AMDatabase* myWorkingDatabase;
AMDbObjectSupport::registerClass<MyDbObject>(myWorkingDatabase);
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
AMDbObject* newSomeKindaObject = AMDbObjectSupport::createAndLoadObjectAt(myWorkingDatabase, tableName, id);
\endcode
You can then use qobject_cast<>() or type() to test the type of the newly-created object.

\note This functionality depends on the detailed class providing either a default constructor, or a constructor that accepts a database and id. In either case, the constructor must be declared with the Q_INVOKABLE flag. It's recommended that all AMDbObjects provide one or another of these constructors.

\todo how to re-implement the modified() and metaDataChanged() signal, now that we're using the property system, instead of setMetaData()?


<b>Composition of AMDbObjects</b>

Special handling of two property types provides support for composite AMDbObjects that "own" or "have" other AMDbObjects as member variables.  (Note that this functionality is provided for ownership situations only; it is distinct from the concept of association (ex: one-to-one, one-to-many, or many-to-many, such as between Scans and Experiments).  The distinction here is that a scan object does not 'own' all of the experiments it is part of, nor does an experiment 'own' all the scans it contains. Association relationships must be handled separately.)

To automatically store AMDbObject member variables, declare a Q_PROPERTY of the type AMDbObject*.  The read function should simply return a pointer to your member object.

When you call storeToDb() on the parent object,
- the read property is used to access a pointer to the member object. If the pointer is not 0, and the object has been modified(), we call storeToDb() on the member object as well.  Then we store the table name and storage id of the member object, so that it can be reloaded later.

Calling loadFromDb() could behave in two different ways:
	- The read property is used again to access a pointer to the member object. If it receives a valid pointer, and the type() of the existing object matches the type of the stored object, we call loadFromDb() on the existing object. setProperty() is never called.
	- If the read property returns a null pointer, or if the type() of the existing object does NOT match the type of the object stored in the database, we cannot call loadFromDb().  Instead, we create a brand new object using AMDbObjectSupport::createAndLoadObjectAt() based on the stored object. Then we call setProperty() with a pointer to the newly created object.  (Note that if createAndLoadObjectAt() fails, setProperty() will pass in a null pointer, and you should check for this.)  \note It's the responsibility of the setProperty() write function to delete the old existing object (if required), take ownership of the new object, and re-connect any signal/slot connections to the newly created object.

	In normal usage, where the type of the member AMDbObject is constant, the second version of the loadFromDb() behaviour will only occur when re-loading the parent object from the database for the first time (if the member object hasn't been created by the constructor, and the read property returns a null pointer). In all subsequent storeToDb()/loadFromDb() calls, the existing member object will simply be stored and loaded transparently with the parent.


To store sets of AMDbObject member variables, you can declare a Q_PROPERTY of the type AMDbObjectList (ie: QList<AMDbObject*>).  The read function should return a list of pointers to your member objects.

storeToDb() and loadFromDb() work the same as in the case of single variables.
	- The read property is used to access a list of pointers to member objects. Each valid object is storeToDb()'d, and the table and id locations for each are stored with the parent object for future reference. (Here we use an auxilliary table called 'MainTableName_propertyName' to remember the stored locations.)

	- loadFromDb() also starts by calling the read property function to get a list of the existing member objects.  If the number of existing objects matches the number of stored objects, AND all the existing objects are valid, AND the types match for all pairs of (existing,stored) objects, then loadFromDb() is called on each.
	- Otherwise, if there are any discrepencies, a whole new set of objects is created with AMDbObjectSupport::createAndLoadObjectAt() based on the stored versions, and setProperty() is called with the list of new objects.  (Note that if createAndLoadObjectAt() fails, there may be null pointers in this list.)  It's the responsibility of the setPropert() write function to delete the old existing objects (if required) and re-establish connections to the new objects.   Note that it is NOT possible to reuse some of the existing objects in the list, but not others.

*/

class AMDbObjectInfo;

class AMDbObject : public QObject
{
	Q_OBJECT

	/// AMDbObject provides a single property: a name() for this instance.
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

	// QObject's have one property to start: objectName().  Don't store QObject::objectName() by default
	Q_CLASSINFO("objectName", "doNotStore=true")



public:
	Q_INVOKABLE explicit AMDbObject(QObject *parent = 0);

	/// Returns an object's unique id
	int id() const { return id_; }

	/// Returns a user given name for this instance.
	QString name() const { return name_; }

	// These functions provide support for storing and retrieving from the database.
	// ===================================

	/// The type (class name) of this object (corresponding to the type of the most detailed subclass).
	/*! This will be stored in the database and used to figure out the real type of the object when loading*/
	QString type() const{
		// metaObject() is virtual, so this will produce the class name of the most detailed subclass.
		return this->metaObject()->className();
	}



	/// returns the name of the database table where objects like this are stored.
	QString dbTableName() const;

	/// If this class has already been registered in the AMDbObject system, returns a pointer to the AMDbObjectInfo describing this class's persistent properties.  If the class hasn't been registered, returns 0;
	const AMDbObjectInfo* dbObjectInfo() const;

	/// If an object has been loaded from a database (or created and stored to a database), this will tell you which database it's in. Returns 0 if this scan instance wasn't loaded out of a database.
	AMDatabase* database() const {
		return database_;
	}

	/// Load yourself from the database. (returns true on success)
	/*! This version loads all of the object's properties (which don't have the \c doNotStore or \c doNotLoad attributes set).  Detailed subclasses should re-implement this if they need to load anything that is not a QProperty.  When doing so, always call the base class implemention first.*/
	virtual bool loadFromDb(AMDatabase* db, int id);

	/// Store or update self in the database. (returns true on success).  After storing, the object's id() will be set to match where it was stored.
	/*! This version saves all of the object's properties (which don't have the \c doNotStore attribute set).  Detailed subclasses should re-implement this if they need to save anything that is not a defined QProperty. When doing so, always call the base class implementation first.
	  */
	virtual bool storeToDb(AMDatabase* db);

	/// Returns truen when this in-memory object has been modified from the version in the database (or when there is no version in the database).  Subclasses should help keep this correct by calling setModified(true) when their properties are changed.
	bool modified() const { return modified_; }

	/// Access the value of one of the AMDbObject_Attributes (by name).  If the attribute hasn't been set, returns an empty string.
	/*! Common attributes are:
	  - doNotReuseIds
	  - shareTableWithClass
	  - description
	  - version

	  /// \todo This is slow -- it probably needs to be optimized.
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




signals:
	/// Removed: Emitted whenever a meta-data item is changed. \c key is the name of the meta-data.
	// removeod: void metaDataChanged(const QString& key);

	/// Emitted when this scan is fully re-loaded from the database
	void loadedFromDb();
	/// Emitted when the modified() state changes. Indicates that this object is in-sync or out-of-sync with the database version.
	void modifiedChanged(bool isModified);

	/// Emitted when the name() changes
	void nameChanged(const QString& newName);

public slots:
	/// Sets user given name
	void setName(const QString &name) { name_ = name; setModified(true); emit nameChanged(name_); }



protected:

	/// Subclasses should call this to set or un-set the modified flag.  Handles emission of the modifiedChanged() signal when required.
	void setModified(bool isModified) {
		if(isModified != modified_)
			emit modifiedChanged(modified_ = isModified);
		modified_ = isModified;
	}


private:
	/// unique database id
	int id_;

	/// pointer to the database where this object came from/should be stored. (If known)
	AMDatabase* database_;

	/// The modified flag is true whenever the object's persistent data does not match what is stored in the database.  It is set automatically inside setProperty(). If you modify the persistent data in other ways, be sure to call setModified(true).
	bool modified_;

	/// stores the name property
	QString name_;

};

/// This global function enables using the insertion operator to store objects in a database. It simply calls AMDbObject::storeToDb() with the given \c database.
/*!		For example: *Database::db() << myScan
Because Scan::storeToDb() is virtual, this version can be used properly for all sub-types of AMDbObject.
*/
AMDatabase& operator<<(AMDatabase& db, AMDbObject& s);

Q_DECLARE_METATYPE(AMDbObject*)

#endif // DBOBJECT_H
