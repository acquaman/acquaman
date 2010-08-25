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

/// This class is the base class for all user-data objects that can be stored in the database.  A generic Scan inherits from this class.
/*! It provides a database id, handling of inserts/retrieves, and very basic metadata that should be common to everything you might want to store:
- a user-chosen "name" and "number"
- a date/time stamp


<b>Meta data system</b>
DbObjects and Scans store their meta-data in a flexible, general way, similar to Qt's Property system. (Todo: decide if we should just merge this into the Qt property system.)

To find out what pieces of meta-data are available:
- metaDataKeys(): returns the available pieces of meta-data for this type of object, including all inherited from base classes (ie: own + base classes'). (static method)
- metaDataUniqueKeys(): returns the available pieces of meta-data for this type of object, excluding those inherited from base classes (ie: own only). (static method)
- metaDataAllKeys(): returns the available pieces of meta-data corresponding to the most detailed type of this instance (ie: own + base classes' + derived classes'). (virtual method)

Meta-data pieces have a data-type (QVariant::Type), a unique key or name to access them with (QString), and a flag to indicate whether they are settable/writeable.  They can be read with
- metaData(const QString& key), and, if available, written with
- setMetaData(const QString& key, const QVariant& value)

When a piece of meta-data is changed, it's appropriate to emit metaDataChanged(), passing the key of the piece that was changed. This is handled in the base class inside setMetaData().

AMDbObjects and AMScans normally also provide convenience functions with readable names to access the meta data values.  Internally, the meta data values are stored in a QHash<QString, QVariant> metaData_.

<b>Notes for subclassing AMDbObject, to create more detailed storable data types:</b>
- Include the Q_OBJECT macro so that Qt's meta-object system knows your class is a distinct type.
- You must re-implement metaDataKeys(), metaDataUniqueKeys(), and metaDataAllKeys(), calling the base class where appropriate.
- It's recommended to initialize your meta-data pieces (metaDataUniqueKeys()) inside metaData_ in the constructor.
- If you need to load or save anything more than your metaData_, you must re-implement loadFromDb() and storeToDb()
- If you want to store yourself anywhere but in the main object table, you must re-implement databaseTableName().  (For example, AMSamples overload databaseTableName() to return AMDatabaseDefinition::sampleTableName(), hence making sure that they are stored in a separate table.) \note If you do this, you must make sure that this table exists or is created by AMFirstTimeController::initializeDatabase().
- Edit
- If you want to have non-blank thumbnails, you must provide thumbnailCount() and thumbnail(int index).
*/

class AMDbObject : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString id READ id)
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

	/// If an object has been loaded from a database, this will tell you which database it came from. Returns 0 if this scan instance wasn't loaded out of a database.
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
