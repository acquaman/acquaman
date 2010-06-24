#ifndef ACQMAN_DBOBJECT_H
#define ACQMAN_DBOBJECT_H

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include "dataman/AMDatabase.h"

#include <QSet>

/// Acquaman's flexible meta-data information system uses AMMetaMetaData to describe each piece of meta-data associated with a scan or database object.
class AMMetaMetaData {
public:
	AMMetaMetaData(QVariant::Type nType, const QString& nKey, bool nWriteable) : type(nType), key(nKey), writeable(nWriteable) {}

	QVariant::Type type;
	QString key;
	bool writeable;
};

/// This class is the base class for all user-data objects that can be stored in the database.  A generic Scan inherits from this class.
/*! It provides a database id, handling of inserts/retrieves, and very basic metadata that should be common to everything you might want to store:
- a user-chosen "name" and "number"
- a date/time stamp

\todo Write more complete documentation.

<b>Notes for subclassing AMDbObject, to create more detailed storable data types:</b>
- If you need to load or save anything more than your metaData_, you must re-implement loadFromDb() and storeToDb()
- Must re-implement metaDataKeys(), metaDataUniqueKeys(), and metaDataAllKeys(), calling the base class where appropriate.


<b>Meta data system</b>
DbObjects and Scans store their meta-data in a flexible, general way, similar to Qt's Property system. (Todo: decide if we should just merge this into the Qt property system.)

Finding out what pieces of meta-data are available:
- metaDataKeys()
- metaDataUniqueKeys()
- metaDataAllKeys()

Meta-data pieces have a data-type (QVariant::Type), a unique key or name to access them with (QString), and a flag to indicate whether they are settable/writeable.  They can be read with
- metaData(const QString& key), and, if available, written with
- setMetaData(const QString& key, const QVariant& value)

Scans and DbObjects normally also provide convenience functions with readable names to access the meta data values.  Internally, the meta data values are stored in a QHash<QString, QVariant>.

*/

class AMDbObject : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString id READ id)
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(int number READ number WRITE setNumber)
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)


public:
    explicit AMDbObject(QObject *parent = 0);

	/// Returns scan's unique id
	int id() const { return id_; }

	/// Returns user given name
	QString name() const { return metaData_["name"].toString();}
	/// Returns a user-given number
	int number() const { return metaData_["number"].toInt();}
	/// Returns creation time
	QDateTime dateTime() const {return metaData_["dateTime"].toDateTime();}
	/// Returns the id of the run containing this scan, or (-1) if not associated with a run. \todo return more useful run descriptive information
	int runId() const { QVariant v = metaData_["runId"]; if(v.isNull()) return -1; else return v.toInt(); }


	// Meta-data system
	/////////////////////////////////////////////

	/// Returns the available pieces of meta data for this type of object, including all inherited from base classes. (ie: own + base classes')
	static QList<AMMetaMetaData> metaDataKeys() {
		return metaDataUniqueKeys();
	}

	/// Returns the available pieces of meta data for this type of object, excluding those inherited from base classes. (ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		rv << AMMetaMetaData(QVariant::DateTime, "dateTime", true);
		rv << AMMetaMetaData(QVariant::String, "name", true);
		rv << AMMetaMetaData(QVariant::Int, "number", true);
		rv << AMMetaMetaData(QVariant::Int, "runId", false);
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
		if(metaData_.contains(key)) {
			metaData_[key] = value;
			emit metaDataChanged(key);
			return true;
		}

		return false;
	}

	// These functions provide support for storing and retrieving from the database.
	// ===================================

	/// The type (aka class name) of this object (corresponding to the type of the most detailed subclass)
	/*! This will be stored in the database and used to figure out the real type of the object when loading*/
	QString type() const{
		// metaObject() is virtual, so this will produce the class name of the most detailed subclass.
		return this->metaObject()->className();
	}

	/// Load yourself from the database. (returns true on success)
	/*! This version loads all of the meta data found for keys metaDataAllKeys().  Detailed subclasses should re-implement this if they need to load anything more specialized than their meta-data.  When doing so, always call the base class implemention first.*/
	virtual bool loadFromDb(AMDatabase* db, int id);
	/// Store or update self in the database. (returns true on success)
	/*! This version saves all of the meta data found for keys metaDataAllKeys().  Detailed subclasses should re-implement this if they need to save anything not found in the meta data. When doing so, always call the base class implementation first.
	  */
	virtual bool storeToDb(AMDatabase* db);


signals:
	void metaDataChanged(const QString& key);

public slots:
	/// Sets user given name
	void setName(const QString &name) { setMetaData("name", name);}
	/// Sets appended number
	void setNumber(int number) { setMetaData("number", number);}
	/// set the date/time:
	void setDateTime(const QDateTime& dt) { setMetaData("dateTime", dt); }
	/// associate this object with a particular run. Set to (-1) to dissociate with any run.  (Note: for now, it's the caller's responsibility to make sure the runId is valid.)
	void setRunId(int runId) { if(runId < 0) metaData_["runId"] = QVariant(); else metaData_["runId"] = runId; }



protected:
	QHash<QString, QVariant> metaData_;


private:
	/// unique database id
	int id_;

	/// [obsolted] List of column names required to have in DB
	/// \todo re-use this for optimizationof metaDataKeys():  static QStringList dbColumnNames_;
};

/// This global function enables using the insertion operator to add scans to the database
///		ex: *Database::db() << myScan
/// Because Scan::storeToDb() is virtual, this version can be used properly for all sub-types of Scans.
AMDatabase& operator<<(AMDatabase& db, AMDbObject& s);

#endif // DBOBJECT_H
