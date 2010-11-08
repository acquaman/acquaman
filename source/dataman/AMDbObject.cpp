#include "AMDbObject.h"
#include "dataman/AMDbObjectSupport.h"
#include "acquaman.h"


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
QString AMDbObject::databaseTableName() const {
	return AMDbObjectSupport::tableNameForClass(this->metaObject());
}

/// If this class has already been registered in the AMDbObject system, returns the AMDbObjectInfo describing this class's persistent properties.  If the class hasn't been registered, returns an invalid AMDbObjectInfo.
const AMDbObjectInfo AMDbObject::dbObjectInfo() const {
	QHash<QString, AMDbObjectInfo>::const_iterator i = AMDbObjectSupport::registeredClasses()->find( type() );
	if( i != AMDbObjectSupport::registeredClasses()->end() ) {
		return i.value();
	}
	return AMDbObjectInfo();
}
/*
const AMDbObjectInfo* AMDbObject::dbObjectInfo() const {
	if( AMDbObjectSupport::registeredClasses()->contains( type() ) ) {
		return &((AMDbObjectInfo&)((*AMDbObjectSupport::registeredClasses())[ type() ]));
	}
	return 0;
}*/


/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.


bool AMDbObject::storeToDb(AMDatabase* db) {

	if(!db)
		return false;

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
		QVariant oldThumbnailCountV = db->retrieve(id(), databaseTableName(), "thumbnailCount");

		if(oldThumbnailCountV.isValid()) {
			oldThumbnailCount = oldThumbnailCountV.toInt();

			// Do we have more than 0 thumbnails, and the same number of thumbnails as the last time we were saved?
			if(thumbnailCount() > 0 && thumbnailCount() == oldThumbnailCount) {
				// That's good. We can re-use those spots.  What are the ids of these spots?
				QVariant oldThumbnailFirstId = db->retrieve(id(), databaseTableName(), "thumbnailFirstId");
				if(oldThumbnailFirstId.isValid()) {
					reuseThumbnailStartId = oldThumbnailFirstId.toInt(&reuseThumbnailIds);
				}
			}
		}
	}
	/////////////////////////////////////



	QVariantList values;
	QStringList keys;

	// determine and append the typeId
	keys << "typeId";
	values << int(0); /// \bug migrate to new system. ysed to be typeId

	/*
	// store all the metadata
	foreach(AMMetaMetaData md, this->metaDataAllKeys()) {

		keys << md.key;

		// special action needed for StringList types: need to join into a single string, because we can't store a StringList natively in the DB.
		if(md.type == QVariant::StringList) {
			QVariant* slv = new QVariant(metaData_.value(md.key).toStringList().join(AMDatabaseDefinition::stringListSeparator()));
			listValues << slv;
			values << slv;
		}
		// special action also needed for all other list types: need to join into a single string, separated by AMDatabaseDefinition::listSeparator
		////////////////////////////////////
		else if(md.type == (int)AM::IntList || md.type == (int)AM::DoubleList) {
			QStringList listString;
			if(md.type == (int)AM::IntList) {
				AMIntList il = metaData_.value(md.key).value<AMIntList>();
				foreach(int i, il) {
					QString is = QString("%1").arg(i);
					listString << is;
				}
			}
			else if(md.type == (int)AM::DoubleList) {
				AMDoubleList dl = metaData_.value(md.key).value<AMDoubleList>();
				foreach(double d, dl) {
					QString ds = QString("%1").arg(d);
					listString << ds;
				}
			}
			QVariant* olv = new QVariant(listString.join(AMDatabaseDefinition::listSeparator()));
			listValues << olv;
			values << olv;
		}
		else if(md.type == QVariant::List) {
			QVariant* slv = new QVariant(metaData_.value(md.key).toStringList().join(AMDatabaseDefinition::listSeparator()));
			listValues << slv;
			values << slv;
		}
		else
			values << &metaData_[md.key];
	}*/

	// Add thumbnail info (just the count for now)
	keys << "thumbnailCount";
	values << thumbnailCount();

	// store typeId, thumbnailCount, and all metadata into main object table
	int retVal = db->insertOrUpdate(id(), databaseTableName(), keys, values);


	// Did the update succeed?
	if(retVal == 0)
		return false;

	// we have our new / old id:
	id_ = retVal;
	database_ = db;

	// Thumbnail save
	///////////////////////////////////////////
	// First, if we WERE saved here before, and we're NOT reusing the same thumbnail spots, and we DID have thumbnails stored previously... delete the old ones
	if(!neverSavedHere && !reuseThumbnailIds && oldThumbnailCount != 0) {
		// qDebug() << "Thumbnail save: deleting old ones before inserting new spots";
		db->deleteRows(AMDbObjectSupport::thumbnailTableName(), QString("objectId = %1 AND objectTableName = '%2'").arg(id_).arg(databaseTableName()));
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
		values << databaseTableName();
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

		if(i == 0)
			firstThumbnailIndex = QVariant(retVal);
	}

	// now that we know where the thumbnails are, update this in our actual table
	if(thumbnailCount() > 0)
		db->update(id_, databaseTableName(), "thumbnailFirstId", firstThumbnailIndex);
	/////////////////////////////////

	setModified(false);
	return true;

}



/// load a AMDbObject (set its properties) by retrieving it based on id.
bool AMDbObject::loadFromDb(AMDatabase* db, int sourceId) {

	// All valid database id's start at 1. This is an optimization to omit the db query if it won't find anything.
	if(sourceId < 1)
		return false;

	QStringList keys;
	QVariantList values;

	/*! \bug removed; migrate to new system
	foreach(AMMetaMetaData md, this->metaDataAllKeys()) {
		keys << md.key;
	}*/

	values = db->retrieve( sourceId, databaseTableName(), keys);
	if( !values.isEmpty() ) {
		id_ = sourceId;
		database_ = db;

		/*! \bug removed; migrate to new system
		// special action needed: StringList types have been returned as strings... Need to convert back to stringLists. Other list types have also been returned as strings. Need to convert them back to their actual list types.
		foreach(AMMetaMetaData md, this->metaDataAllKeys()) {

			// For lists that SHOULD be StringLists...
			if(md.type == QVariant::StringList) {
				if(metaData_.value(md.key).isNull())
					metaData_[md.key] = QStringList();
				else
					metaData_[md.key] = metaData_.value(md.key).toString().split(AMDatabaseDefinition::stringListSeparator());
			}

			// For lists that should be anything else (ints, doubles, etc.)
			else if(md.type == (int)AM::IntList || md.type == (int)AM::DoubleList || md.type == QVariant::List) {
				QStringList stringListForm = metaData_.value(md.key).toString().split(AMDatabaseDefinition::listSeparator(), QString::SkipEmptyParts);
				// Now we've got a stringList. Get that back into a list of integers
				if(md.type == (int)AM::IntList) {
					AMIntList il;
					foreach(QString i, stringListForm)
						il << i.toInt();
					metaData_[md.key].setValue(il);
				}
				else if(md.type == (int)AM::DoubleList) {
					AMDoubleList dl;
					foreach(QString d, stringListForm)
						dl << d.toDouble();
					metaData_[md.key].setValue(dl);
				}
				// all other lists get left as string lists. Sorry... I don't know what type you are, so I can't do much else.
				else {
					metaData_[md.key] = stringListForm;
				}
			}
		}*/

		setModified(false);
		/*! \bug removed: foreach(QString key, keys)
			emit metaDataChanged(key);*/
		emit loadedFromDb();
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
