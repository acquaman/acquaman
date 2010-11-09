#include "AMDbObject.h"
#include "dataman/AMDbObjectSupport.h"
#include "acquaman.h"

#include <QMetaType>

/// Default constructor
AMDbThumbnail::AMDbThumbnail(const QString& Title, const QString& Subtitle, ThumbnailType Type, const QByteArray& ThumbnailData)
	: title(Title), subtitle(Subtitle), type(Type), thumbnail(ThumbnailData) {
}

/// This constructor takes a pixmap of any size and saves it as a PNG type. (It will be saved at the current size of the pixmap, so if you want to save at a reduced size, pass in pixmap.scaledToWidth(240) or similar.)
AMDbThumbnail::AMDbThumbnail(const QString& Title, const QString& Subtitle, const QPixmap& pixmap)
	: title(Title), subtitle(Subtitle) {

	if(pixmap.isNull()) {
		type = InvalidType;
		thumbnail = QByteArray();
	}
	else {
		//removed: QPixmap p2 = (pixmap.width() == 240) ? pixmap : pixmap.scaledToWidth(240, Qt::SmoothTransformation);
		QBuffer bout;
		bout.open(QIODevice::WriteOnly);
		if(pixmap.save(&bout, "PNG")) {
			type = PNGType;
			thumbnail = bout.buffer();
		}
		else {
			type = InvalidType;
			thumbnail = QByteArray();
		}
	}
}



QString AMDbThumbnail::typeString() const {
	switch(type) {
	case PNGType:
		return "PNG";
		break;
	case InvalidType:
	default:
		return "Invalid";
		break;
	}
}

AMDbObject::AMDbObject(QObject *parent) : QObject(parent) {
	id_ = 0;
	database_ = 0;
	setModified(true);

	name_ = "Unnamed Object";

}

#include <QMetaClassInfo>
QString AMDbObject::dbObjectAttribute(const QString& key) const {
	return AMDbObjectSupport::dbObjectAttribute(this->metaObject(), key);
}


QString AMDbObject::dbPropertyAttribute(const QString& propertyName, const QString& key) const {
	return AMDbObjectSupport::dbPropertyAttribute(this->metaObject(), propertyName, key);
}


/// returns the name of the database table where objects like this should be/are stored
QString AMDbObject::dbTableName() const {
	return AMDbObjectSupport::tableNameForClass(this->metaObject());
}

/// If this class has already been registered in the AMDbObject system, returns a pointer to the AMDbObjectInfo describing this class's persistent properties.  If the class hasn't been registered, returns 0;
const AMDbObjectInfo* AMDbObject::dbObjectInfo() const {
	return AMDbObjectSupport::objectInfoForClass( type() );
}
/*
const AMDbObjectInfo* AMDbObject::dbObjectInfo() const {
	if( AMDbObjectSupport::registeredClasses()->contains( type() ) ) {
		return &((AMDbObjectInfo&)((*AMDbObjectSupport::registeredClasses())[ type() ]));
	}
	return 0;
}*/


/// This member function updates a scan in the database (if it exists already in that database), otherwise it adds it to the database.
bool AMDbObject::storeToDb(AMDatabase* db) {

	if(!db)
		return false;

	const AMDbObjectInfo* myInfo = dbObjectInfo();
	if(!myInfo)
		return false;	// class has not been registered yet in the database system.

	///////////////////////////////////
	// Thumbnail saving optimization:
	// If we've been previously saved to this database, and the number of thumbnails we had before matches what we have now, then flag to save them in place. (Otherwise we have to delete and reinsert the new thumbnails).
	///////////////////////////////////
	bool neverSavedHere = (id()<1 || database() != db);
	bool reuseThumbnailIds = false;
	int reuseThumbnailStartId;
	int oldThumbnailCount = -1;

	// If we've been saved in this database before...
	if( id() > 0 && database() == db) {

		// How many thumbnails did we use to have?
		QVariant oldThumbnailCountV = db->retrieve(id(), myInfo->tableName, "thumbnailCount");

		if(oldThumbnailCountV.isValid()) {
			oldThumbnailCount = oldThumbnailCountV.toInt();

			// Do we have more than 0 thumbnails, and the same number of thumbnails as the last time we were saved?
			if(thumbnailCount() > 0 && thumbnailCount() == oldThumbnailCount) {
				// That's good. We can re-use those spots.  What are the ids of these spots?
				QVariant oldThumbnailFirstId = db->retrieve(id(), myInfo->tableName, "thumbnailFirstId");
				if(oldThumbnailFirstId.isValid()) {
					reuseThumbnailStartId = oldThumbnailFirstId.toInt(&reuseThumbnailIds);
				}
			}
		}
	}
	/////////////////////////////////////



	QVariantList values;	// list of values to store
	QStringList keys;	// list of keys (column names) to store

	// determine and append the type. (Necessary to know for later, when storing objects of different types in the same table)
	keys << "AMDbObjectType";
	values << type();

	// store all the columns:
	//////////////////////////////////////////////////
	for(int i=0; i<myInfo->columnCount; i++) {

		// add column name to key list
		keys << myInfo->columns.at(i);

		// add value to values list. First, some special processing is needed for StringList, IntList, and DoubleList types, to join their values into a single string. Other property types simply get written out in their native QVariant form.
		int columnType = myInfo->columnTypes.at(i);
		QByteArray columnNameBA = myInfo->columns.at(i).toAscii();
		const char* columnName = columnNameBA.constData();

		/// AAAAAHHHHH!!! determine whether columnType is QProperty's QVariant::type() or QVariant::userType(). Handle as req'd.
		/// \todo add AMDbObjectList
		if(columnType == qMetaTypeId<AMIntList>()) {
			AMIntList intList = property(columnName).value<AMIntList>();
			QStringList resultString;
			foreach(int i, intList)
				resultString << QString("%1").arg(i);
			values << resultString.join(AMDbObjectSupport::listSeparator());
		}

		else if(columnType == qMetaTypeId<AMDoubleList>()) {
			AMDoubleList doubleList = property(columnName).value<AMDoubleList>();
			QStringList resultString;
			foreach(double d, doubleList)
				resultString << QString("%1").arg(d);
			values << resultString.join(AMDbObjectSupport::listSeparator());
		}

		/// \todo determine if QVariantList gets handled properly here...
		else if(columnType == QVariant::StringList || columnType == QVariant::List) {
			values << property(columnName).toStringList().join(AMDbObjectSupport::stringListSeparator());
		}

		else
			values << property(columnName);
	}
	////////////////////////////////////////



	// Add thumbnail info (just the count for now)
	keys << "thumbnailCount";
	values << thumbnailCount();

	// store type, thumbnailCount, and all metadata into the table.
	int retVal;
	// If saving into same database, can use existing id():
	if(database() == db)
		retVal = db->insertOrUpdate(id(), myInfo->tableName, keys, values);
	// otherwise, use id of 0 to insert new.
	else
		retVal = db->insertOrUpdate(0, myInfo->tableName, keys, values);


	// Did the update succeed?
	if(retVal == 0)
		return false;

	// Success! We have our new / old id:
	id_ = retVal;
	database_ = db;

	// Thumbnail save
	///////////////////////////////////////////
	// First, if we WERE saved here before, and we're NOT reusing the same thumbnail spots, and we DID have thumbnails stored previously... delete the old ones
	if(!neverSavedHere && !reuseThumbnailIds && oldThumbnailCount != 0) {
		// qDebug() << "Thumbnail save: deleting old ones before inserting new spots";
		db->deleteRows(AMDbObjectSupport::thumbnailTableName(), QString("objectId = %1 AND objectTableName = '%2'").arg(id_).arg(myInfo->tableName));
	}

	// store thumbnails in thumbnail table:
	QVariant firstThumbnailIndex;
	for(int i=0; i<thumbnailCount(); i++) {
		AMDbThumbnail t = thumbnail(i);

		keys.clear();
		values.clear();

		keys << "objectId";
		values << id_;
		keys << "objectTableName";
		values << myInfo->tableName;
		keys << "number";
		values << i;
		keys << "type";
		values << t.typeString();
		keys << "title";
		values << t.title;
		keys << "subtitle";
		values << t.subtitle;
		keys << "thumbnail";
		values << t.thumbnail;

		if(reuseThumbnailIds) {
			retVal = db->insertOrUpdate(i+reuseThumbnailStartId, AMDbObjectSupport::thumbnailTableName(), keys, values);
			// qDebug() << "Thumbnail save: reusing spots at " << reuseThumbnailStartId+i;
		}
		else {
			retVal = db->insertOrUpdate(0, AMDbObjectSupport::thumbnailTableName(), keys, values);
			// qDebug() << "Thumbnail save: Inserting new spots" << retVal;
		}
		if(retVal == 0)
			return false;

		if(i == 0)	// when inserting the first one... remember the id of this first thumbnail.
			firstThumbnailIndex = QVariant(retVal);
	}

	// now that we know where the thumbnails are, update this in our actual table
	if(thumbnailCount() > 0)
		db->update(id_, myInfo->tableName, "thumbnailFirstId", firstThumbnailIndex);
	/////////////////////////////////

	// we were just stored to the database, so our properties must be in sync with it.
	setModified(false);
	return true;
}



/// load a AMDbObject (set its properties) by retrieving it based on id.
bool AMDbObject::loadFromDb(AMDatabase* db, int sourceId) {

	// All valid database id's start at 1. This is an optimization to omit the db query if it won't find anything.
	if(sourceId < 1)
		return false;

	const AMDbObjectInfo* myInfo = dbObjectInfo();
	if(!myInfo)
		return false;	// class hasn't been registered yet with the database system.


	/// Retrieve all columns from the database. (\todo Future optimization: don't retrieve columns that we don't need. Anything with the doNotLoad attribute set is not necessary)
	QVariantList values = db->retrieve( sourceId, myInfo->tableName, myInfo->columns);

	if(values.isEmpty())
		return false;

	// if we just successfully loaded out of here, then we have our new id() and database().
	id_ = sourceId;
	database_ = db;

	// go through all results and restore properties
	for(int i=0; i<myInfo->columnCount; i++) {

		// do not re-load this column?
		if(!myInfo->isLoadable.at(i))
			continue;

		QByteArray columnNameBA = myInfo->columns.at(i).toAscii();
		const char* columnName = columnNameBA.constData();
		int columnType = myInfo->columnTypes.at(i);

		// special action necessary to convert StringList, IntList, and DoubleList types which have been returned as strings.

		/// AAAAAHHHHH!!! determine whether columnType is QProperty's QVariant::type() or QVariant::userType(). Handle as req'd.
		/// \todo add AMDbObjectList
		if(columnType == qMetaTypeId<AMIntList>()) {	// integer lists
			AMIntList intList;
			QStringList stringList = values.at(i).toString().split(AMDbObjectSupport::listSeparator());
			foreach(QString i, stringList)
				intList << i.toInt();
			setProperty(columnName, QVariant::fromValue(intList));
		}
		else if(columnType == qMetaTypeId<AMDoubleList>()) {	// double lists
			AMDoubleList doubleList;
			QStringList stringList = values.at(i).toString().split(AMDbObjectSupport::listSeparator());
			foreach(QString d, stringList)
				doubleList << d.toDouble();
			setProperty(columnName, QVariant::fromValue(doubleList));
		}
		/// \todo determine if QVariantList gets handled properly here...
		else if(columnType == QVariant::StringList || columnType == QVariant::List) {	// string list, and anything-else-lists saved as string lists
			setProperty(columnName, values.at(i).toString().split(AMDbObjectSupport::stringListSeparator()));
		}
		else {
			setProperty(columnName, values.at(i));
		}
	}

	// we were just loaded out of the database, so we must be in-sync.
	setModified(false);

	/*! \bug removed: foreach(QString key, keys)
			emit metaDataChanged(key);*/
	emit loadedFromDb();
	return true;


}



/// This global function enables using the insertion operator to add objects to the database
///		ex: *Database::db() << myScan
/// Because AMDbObject::storeToDb() is virtual, this version can be used properly for all sub-types of AMDbObject.
AMDatabase& operator<<(AMDatabase& db, AMDbObject& s) {
	s.storeToDb(&db);
	return db;
}
