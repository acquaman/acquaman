#include "AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"


AMDbObject::AMDbObject(QObject *parent) : QObject(parent) {
	id_ = 0;

	metaData_["name"] = "Untitled";
	metaData_["number"] = 0;
	metaData_["dateTime"] = QDateTime::currentDateTime();

}

/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.


bool AMDbObject::storeToDb(AMDatabase* db) {

	QList<const QVariant*> values;
	QStringList keys;

	keys << "type";
	QVariant typeVariant(this->type());
	values << &typeVariant;

	foreach(AMMetaMetaData md, this->metaDataAllKeys()) {
		keys << md.key;
		values << &metaData_[md.key];
	}

	int retVal = db->insertOrUpdate(id(), AMDatabaseDefinition::objectTableName(), keys, values);

	if(retVal > 0) {
		id_ = retVal;
		return true;
	}
	else {
		return false;
	}
}

/// load a AMDbObject (set its properties) by retrieving it based on id.
bool AMDbObject::loadFromDb(AMDatabase* db, int sourceId) {

	QStringList keys;
	QList<QVariant*> values;

	foreach(AMMetaMetaData md, this->metaDataAllKeys()) {
		keys << md.key;
		values << &metaData_[md.key];
	}

	if( db->retrieve( sourceId, AMDatabaseDefinition::objectTableName(), keys, values) ) {
		id_ = sourceId;
		foreach(QString key, keys)
			emit metaDataChanged(key);
		return true;
	}
	else {
		return false;
	}

}



/// This global function enables using the insertion operator to add objects to the database
///		ex: *Database::db() << myScan
/// Because AMDbObject::storeToDb() is virtual, this version can be used properly for all sub-types of AMDbObject.
AMDatabase& operator<<(AMDatabase& db, AMDbObject& s) {
	s.storeToDb(&db);
	return db;
}
