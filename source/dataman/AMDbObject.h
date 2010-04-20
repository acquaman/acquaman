#ifndef ACQMAN_DBOBJECT_H
#define ACQMAN_DBOBJECT_H

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include "dataman/AMDatabase.h"

/// This class is the base class for all user-data objects that can be stored in the database.  A generic Scan inherits from this class.
/*! It provides a database id, handling of inserts/retrieves, and very basic metadata that should be common to everything you might want to store:
- a user-chosen "name" and "number"
- a date/time stamp

\todo Write more complete documentation.

<b>Notes for subclassing AMDbObject, to create more detailed storable data types:</b>
- Must re-implement loadFromDb() and storeToDb()
- Must re-implement dbColumnNames(), dbColumnTypes(), dbTableName(), and dbPrepareTables if using them in loadFromDb() and storeToDb().

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
	QString name() const { return name_;}
	/// Returns a user-given number
	int number() const { return number_;}
	/// Returns creation time
	QDateTime dateTime() const {return dateTime_;}


	// These functions provide support for storing and retrieving from the database.
	// ===================================

	/// The type (aka class name) of this object (corresponding to the type of the most detailed subclass)
	/// This will be stored in the top table of the database and used to figure out the real type of the object when loading
	QString type() { return this->metaObject()->className(); }	// metaObject() is virtual, so this will produce the class name of the most detailed subclass.

	/// The name of the table that will store the object's properties:
	static QString dbTableName() { return AMSettings::dbObjectTableName; }

	/// A list of the column names required to store the object's properties. (note: the key column 'id' is always included; don't specify it here.)
	static const QStringList& dbColumnNames() {
		// Insert if not filled already:
		if(dbColumnNames_.isEmpty()) {
			dbColumnNames_ = QString("type,name,number,dateTime").split(',');
		}
		return dbColumnNames_;
	}
	/// A list of the column types recommended to store the object's properties. (note: this must have the same number of items and correspond to dbColumnNames())
	static QStringList dbColumnTypes() { return QString("TEXT,TEXT,INTEGER,TEXT").split(','); }

	/// A static function to make sure the database is ready to hold us.
	/// When re-implementing, make sure to call base-class implementation first.
	static void dbPrepareTables(AMDatabase* db) { db->ensureTable(dbTableName(), dbColumnNames(), dbColumnTypes()); }
	// ======================================

signals:

public slots:
	/// Sets user given name
	void setName(const QString &name) { name_ = name;}
	/// Sets appended number
	void setNumber(int number) { number_ = number;}
	/// set the date/time:
	void setDateTime(const QDateTime& dt) { dateTime_ = dt; }

	/// Load yourself from the database. (returns true on success)
	/// Detailed subclasses of Scan must re-implement this to retrieve all of their unique data fields.
	/// When doing so, always call the parent class implemention first.
	virtual bool loadFromDb(AMDatabase* db, int id);
	/// Store or update self in the database. (returns true on success)
	/// Detailed subclasses of Scan must re-implement this to store all of their unique data.
	/// When doing so, always call the parent class implemention first.
	virtual bool storeToDb(AMDatabase* db);

private:
	/// User defined scan name
	QString name_;
	/// Number to be appended to scan name
	int number_;
	/// Start time of original scan
	QDateTime dateTime_;

	/// unique database id
	int id_;
	/// List of column names required to have in DB
	static QStringList dbColumnNames_;
};

/// This global function enables using the insertion operator to add scans to the database
///		ex: *Database::db() << myScan
/// Because Scan::storeToDb() is virtual, this version can be used properly for all sub-types of Scans.
AMDatabase& operator<<(AMDatabase& db, AMDbObject& s);

#endif // DBOBJECT_H
