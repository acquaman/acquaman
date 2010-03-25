#include "DbObject.h"

// static dbColumnNames (each instance has same)
QStringList DbObject::dbColumnNames_;

DbObject::DbObject(QObject *parent) : QObject(parent) {
	id_ = 0;
	// Ensure the static (class-wide) dbColumnNames_ has already been filled:
	dbColumnNames();
}

/// This member function updates an in the database (if it exists already), otherwise it adds it to the database.

	// - New Objects have their id() field set to <1, unless they've been retrieved from the database, in which case they have id() = [valid database key]
	// Watch out: by creating a new object and giving it id() = [some arbitrary positive number]; you'll destroy data in the db.

bool DbObject::storeToDb(Database* db) {

	QList<const QVariant*> values;
	QVariant v0(type());
	QVariant v1(name());
	QVariant v2(number());
	QVariant v3(dateTime());
	values << &v0 << &v1 << &v2 << &v3;

	int retVal = db->insertOrUpdate(id(),dbTableName(), dbColumnNames_, values);

	if(retVal > 0) {
		id_ = retVal;
		return true;
	}
	else {
		return false;
	}
}

/// load a DbObject (set its properties) by retrieving it based on id.
bool DbObject::loadFromDb(Database* db, int sourceId) {

	// Provide memory storage for return value:
	QList<QVariant*> values;
	QVariant v0, v1, v2, v3;
	values << &v0 << &v1 << &v2 << &v3;

	if( db->retrieve( sourceId, dbTableName(), dbColumnNames_, values) ) {
		id_ = sourceId;
		// v0 ignored: the type is read-only.  It might not match ourself, but that's okay... hopefully the subclasses keep loading. Warn here?
		setName(v1.toString());
		setNumber(v2.toInt());
		setDateTime(v3.toDateTime());
		return true;
	}
	else {
		return false;
	}

}



/// This global function enables using the insertion operator to add objects to the database
///		ex: *Database::db() << myScan
/// Because DbObject::storeToDb() is virtual, this version can be used properly for all sub-types of DbObject.
Database& operator<<(Database& db, DbObject& s) {
	s.storeToDb(&db);
	return db;
}
