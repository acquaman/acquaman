#include "AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"


AMDbObject::AMDbObject(QObject *parent) : QObject(parent) {
	id_ = 0;
	database_ = 0;

	metaData_["name"] = "Untitled";

}

/// returns the name of the database table where objects like this should be stored
QString AMDbObject::databaseTableName() const {
	return AMDatabaseDefinition::objectTableName();
}

/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.


bool AMDbObject::storeToDb(AMDatabase* db) {

	QList<const QVariant*> values;
	QStringList keys;
	QList<QVariant*> stringListValues;

	// determine and append the typeId
	keys << "typeId";
	QVariant typeVariant(this->typeId(db));
	values << &typeVariant;

	// store all the metadata
	foreach(AMMetaMetaData md, this->metaDataAllKeys()) {

		keys << md.key;

		// special action needed for StringList types: need to join into a single string, because we can't store a StringList natively in the DB.
		if(md.type == QVariant::StringList) {
			QVariant* slv = new QVariant(metaData_[md.key].toStringList().join(AMDatabaseDefinition::stringListSeparator()));
			stringListValues << slv;
			values << slv;
		}
		else
			values << &metaData_[md.key];
	}

	// Add thumbnail info (just the count for now)
	keys << "thumbnailCount";
	QVariant thumbCountVariant(this->thumbnailCount());
	values << &thumbCountVariant;

	// store typeId, thumbnailCount, and all metadata into main object table
	int retVal = db->insertOrUpdate(id(), databaseTableName(), keys, values);

	// delete all the stringList variants we had to temporarily create
	foreach(QVariant* slv, stringListValues)
		delete slv;

	// Did the update succeed?
	if(retVal == 0)
		return false;

	// we have our new / old id:
	id_ = retVal;

	// add all thumbnails.
	// First, remove all old thumbnails:
	db->deleteRows(AMDatabaseDefinition::thumbnailTableName(), QString("objectId = %1 AND objectTableName = '%2'").arg(id_).arg(databaseTableName()));

	// store thumbnails in thumbnail table:
	QVariant firstThumbnailIndex;
	for(int i=0; i<thumbnailCount(); i++) {
		AMDbThumbnail t = thumbnail(i);

		QVariant vobjectId(id_), vobjectTableName(databaseTableName()), vnumber(i), vtype(t.typeString()), vtitle(t.title), vsubtitle(t.subtitle), vthumbnail(t.thumbnail);
		keys.clear();
		values.clear();

		keys << "objectId";
		values << &vobjectId;
		keys << "objectTableName";
		values << &vobjectTableName;
		keys << "number";
		values << &vnumber;
		keys << "type";
		values << &vtype;
		keys << "title";
		values << &vtitle;
		keys << "subtitle";
		values << &vsubtitle;
		keys << "thumbnail";
		values << &vthumbnail;

		retVal = db->insertOrUpdate(0, AMDatabaseDefinition::thumbnailTableName(), keys, values);
		if(retVal == 0)
			return false;

		if(i == 0)
			firstThumbnailIndex = QVariant(retVal);
	}

	// now that we know where the thumbnails are, update this in our
	db->update(id_, AMDatabaseDefinition::objectTableName(), "thumbnailFirstId", firstThumbnailIndex);
	return true;

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
		database_ = db;

		// special action needed: StringList types have been returned as strings... Need to convert back to stringLists.
		foreach(AMMetaMetaData md, this->metaDataAllKeys()) {
			if(md.type == QVariant::StringList) {
				metaData_[md.key] = metaData_[md.key].toString().split(AMDatabaseDefinition::stringListSeparator());
			}
		}

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
