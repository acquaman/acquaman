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

/// Acquaman's flexible meta-data information system uses AMMetaMetaData to describe each piece of meta-data associated with a scan or database object.
class AMMetaMetaData {
public:
	AMMetaMetaData(QVariant::Type nType, const QString& nKey, bool nWriteable) : type(nType), key(nKey), writeable(nWriteable) {}
	AMMetaMetaData(AM::AcquamanType nType, const QString& nKey, bool nWriteable) : type((QVariant::Type)nType), key(nKey), writeable(nWriteable) {}

	QVariant::Type type;
	QString key;
	bool writeable;
};


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

- You can use the Q_CLASSINFO macro to specify additional characteristics for each field. The (name, value) QClassInfo pairs should be in the form:
\code
Q_CLASSINFO("propertyName", "keyword=value;keyword=value;keyword=value...")
\endcode
	where supported keywords are:
	- \c doNotStore: if equal to "true", does not store this property in the database. (Also implies doNotLoad=true)
	- \c doNotLoad: if equal to "true", does not set this property when re-loading an object from the database.
	- \c hidden: if equal to "true", this property should not be user-visible in default tables created on the database.
	- \c createIndex: if equal to "true", creates an index on this column in the table. (You should understand the performance implications of doing this. In short, it will make searches based on this column's values much faster, but slow down inserts and deletes, and take extra space in the database.

	If unspecified, the default value for all keywords is false.

- A set of keywords is also available within the special "AMDbObject_Parameters" QClassInfo key.
\code
Q_CLASSINFO("AMDbObject_Parameters", "keyword=value;...")
\endcode
	where the supported keywords are:
	- \c doNotReuseIds: if equal to "true", the auto-increment property on the primary key will be set so that new objects will never reuse the ids of old deleted ones.  Otherwise, after a table has had rows created and deleted, the ids of previously deleted rows may be re-used.
	- \c shareTableWithClass=[\c className]: if defined, uses the same database table for storage as [\c className].  Note that [\c className] must be registered with the AMDbObject system prior to registering this class.


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
AMDbObject* newSomeKindaObject = AMDbObjectSupport::loadObject(myWorkingDatabase, tableName, id);
\endcode
You can then use qobject_cast<>() to test the type of the created object, or ask it for its metaObject()->className(), etc.

\note This functionality depends on the detailed class providing either a default constructor (or a constructor that accepts a database and id), and is declared with the Q_INVOKABLE flag. It's recommended that all AMDbObjects provide such a constructor, which initializes a new object directly from a stored copy in the database.

\todo how to re-implement the modified() signal, now that we're using the property system, instead of setMetaData()?


*/

class AMDbObject : public QObject
{
	Q_OBJECT

	/// AMDbObject provides a single property: a name() for this instance.
	Q_PROPERTY(QString name READ name WRITE setName)


public:
	explicit AMDbObject(QObject *parent = 0);

	/// Returns an object's unique id
	int id() const { return id_; }

	/// Returns user given name
	QString name() const { return metaData_.value("name").toString();}

	// Meta-data system
	/////////////////////////////////////////////

	/// Returns the available pieces of meta data for this type of object, including all inherited from base classes. (ie: own + base classes')
	static QList<AMMetaMetaData> metaDataKeys() {
		return metaDataUniqueKeys();
	}

	/// Returns the available pieces of meta data for this type of object, excluding those inherited from base classes. (ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		rv << AMMetaMetaData(QVariant::String, "name", true);
		return rv;
	}

	/// Returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	/// Returns the value of a piece of meta data:
	virtual QVariant metaData(const QString& key) const {
		if(metaData_.contains(key))
			return metaData_.value(key);
		else
			return QVariant();
	}

	/// set a meta data value:
	virtual bool setMetaData(const QString& key, const QVariant& value) {
		/// \bug need to implement writeability checking
		if(metaData_.contains(key) && metaData_.value(key) != value) {
			metaData_[key] = value;
			setModified(true);
			emit metaDataChanged(key);
			return true;
		}

		return false;
	}

	// These functions provide support for storing and retrieving from the database.
	// ===================================

	/// The type (class name) of this object (corresponding to the type of the most detailed subclass).
	/*! This will be stored in the database and used to figure out the real type of the object when loading*/
	QString type() const{
		// metaObject() is virtual, so this will produce the class name of the most detailed subclass.
		return this->metaObject()->className();
	}

	/// A human-readable description for the type of this object. (ex: "XAS Scan"). The default implementation just returns type(), the class name. You can re-implement for better usability.
	virtual QString typeDescription() const {
		return type();
	}

	/// returns the typeId of this scan's registered type in a database. If it hasn't been registered as a type yet, this will return 0.
	/*! Althought this function doesn't look like it's virtual, it calls type() and returns the typeId of the most detailed subclass.*/
	int typeId(AMDatabase* db) const;

	/// returns the name of the database table where objects like this should be stored. The default implementation stores in AMDatabaseDefinition::objectTableName().
	virtual QString databaseTableName() const;

	/// Load yourself from the database. (returns true on success)
	/*! This version loads all of the meta data found for keys metaDataAllKeys().  Detailed subclasses should re-implement this if they need to load anything more specialized than their meta-data.  When doing so, always call the base class implemention first.*/
	virtual bool loadFromDb(AMDatabase* db, int id);

	/// If an object has been loaded from a database (or created and stored to a database), this will tell you which database it's in. Returns 0 if this scan instance wasn't loaded out of a database.
	AMDatabase* database() const {
		return database_;
	}

	/// Store or update self in the database. (returns true on success).  After storing, the objects id() will be set to match where it was stored.
	/*! This version saves all of the meta data found for keys metaDataAllKeys().  Detailed subclasses should re-implement this if they need to save anything not found in the meta data. When doing so, always call the base class implementation first.
	  */
	virtual bool storeToDb(AMDatabase* db);

	/// Returns truen when this in-memory object has been modified from the version in the database (or when there is no version in the database)
	bool modified() const { return modified_; }


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
	/// Emitted whenever a meta-data item is changed. \c key is the name of the meta-data.
	void metaDataChanged(const QString& key);
	/// Emitted when this scan is fully re-loaded from the database
	void loadedFromDb();
	/// Emitted when the modified() state changes. Indicates that this object is in-sync or out-of-sync with the database version.
	void modifiedChanged(bool isModified);

public slots:
	/// Sets user given name
	void setName(const QString &name) { setMetaData("name", name);}



protected:
	QHash<QString, QVariant> metaData_;

	/// Use to set or un-set the modified flag.  Handles emission of the modifiedChanged() signal when required.
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

	/// The modified flag is true whenever the scan's meta-data does not match what is stored in the database.  It is set automatically inside setMetaData(). If you modify the metadata outside of this system, be sure to change it yourself.
	bool modified_;

};

/// This global function enables using the insertion operator to add scans to the database
///		ex: *Database::db() << myScan
/// Because Scan::storeToDb() is virtual, this version can be used properly for all sub-types of Scans.
AMDatabase& operator<<(AMDatabase& db, AMDbObject& s);

#endif // DBOBJECT_H
