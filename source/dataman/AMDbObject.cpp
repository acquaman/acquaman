#include "AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"
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

	metaData_["name"] = "Untitled";

}

/// returns the name of the database table where objects like this should be stored
QString AMDbObject::databaseTableName() const {
	return AMDatabaseDefinition::objectTableName();
}

/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.

#include <QDebug>
bool AMDbObject::storeToDb(AMDatabase* db) {

	QList<const QVariant*> values;
	QStringList keys;
	QList<QVariant*> listValues;

	// determine and append the typeId
	keys << "typeId";
	QVariant typeVariant(this->typeId(db));
	values << &typeVariant;

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
	}

	// Add thumbnail info (just the count for now)
	keys << "thumbnailCount";
	QVariant thumbCountVariant(this->thumbnailCount());
	values << &thumbCountVariant;

	// store typeId, thumbnailCount, and all metadata into main object table
	int retVal = db->insertOrUpdate(id(), databaseTableName(), keys, values);

	// delete all the variants we had to temporarily create for lists
	foreach(QVariant* lv, listValues)
		delete lv;


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

	// now that we know where the thumbnails are, update this in our actual table
	if(thumbnailCount() > 0)
		db->update(id_, databaseTableName(), "thumbnailFirstId", firstThumbnailIndex);

	setModified(false);
	return true;

}

/// returns the typeId of this scan's registered type in a database. If it hasn't been registered as a type yet, this will return 0.
/*! Althought this function doesn't look like it's virtual, it calls type() and returns the typeId of the most detailed subclass.*/
int AMDbObject::typeId(AMDatabase* db) const {
	QSqlQuery q = db->query();
	q.prepare("SELECT id FROM ObjectTypes WHERE className = ?");
	q.bindValue(0, type());
	if(q.exec() && q.next())
		return q.value(0).toInt();
	else
		return 0;
}

/// load a AMDbObject (set its properties) by retrieving it based on id.
bool AMDbObject::loadFromDb(AMDatabase* db, int sourceId) {

	QStringList keys;
	QList<QVariant*> values;

	foreach(AMMetaMetaData md, this->metaDataAllKeys()) {
		keys << md.key;
		values << &metaData_[md.key];
	}

	if( db->retrieve( sourceId, databaseTableName(), keys, values) ) {
		id_ = sourceId;
		database_ = db;

		// special action needed: StringList types have been returned as strings... Need to convert back to stringLists. Other list types have also been returned as strings. Need to convert them back to their actual list types.
		foreach(AMMetaMetaData md, this->metaDataAllKeys()) {

			// For lists that SHOULD be StringLists...
			if(md.type == QVariant::StringList) {
				metaData_[md.key] = metaData_.value(md.key).toString().split(AMDatabaseDefinition::stringListSeparator());
			}

			// For lists that should be anything else (ints, doubles, etc.)
			else if(md.type == (int)AM::IntList || md.type == (int)AM::DoubleList || md.type == QVariant::List) {
				QStringList stringListForm = metaData_.value(md.key).toString().split(AMDatabaseDefinition::listSeparator());
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
		}

		setModified(false);
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
