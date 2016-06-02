/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMDbObject.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/database/AMConstDbObject.h"
#include "acquaman.h"
#include "util/AMErrorMonitor.h"

#include <QMetaType>
#include "dataman/AMnDIndex.h"
#include <QVector3D>
#include <QtConcurrentRun>
#include <QStringBuilder>

// Default constructor
AMDbThumbnail::AMDbThumbnail(const QString& Title, const QString& Subtitle, ThumbnailType Type, const QByteArray& ThumbnailData)
	: title(Title), subtitle(Subtitle), type(Type), thumbnail(ThumbnailData) {
}

// This constructor takes an image of any size and saves it as a PNG type. (It will be saved at the current size of the image, so if you want to save at a reduced size, pass in image.scaledToWidth(240) or similar.)
AMDbThumbnail::AMDbThumbnail(const QString& Title, const QString& Subtitle, const QImage& image)
	: title(Title), subtitle(Subtitle) {

	if(image.isNull()) {
		type = InvalidType;
		thumbnail = QByteArray();
	}
	else {
		QBuffer bout;
		bout.open(QIODevice::WriteOnly);
		if(image.save(&bout, "PNG")) {
			type = PNGType;
			thumbnail = bout.buffer();
		}
		else {
			type = InvalidType;
			thumbnail = QByteArray();
		}
	}
}

AMDbThumbnail::~AMDbThumbnail(){}

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
	isReloading_ = false;
	isStoring_ = false;
	id_ = 0;
	database_ = 0;
	modified_ = true;

	name_ = "Unnamed Object";

}

AMDbObject::AMDbObject(const AMDbObject &original) : QObject() {
	isReloading_ = false;
	isStoring_ = false;
	id_ = original.id_;
	database_ = original.database_;
	modified_ = original.modified_;
	name_ = original.name_;
}

AMDbObject::~AMDbObject(){}

AMDbObject& AMDbObject::operator=(const AMDbObject& other) {
	if(this != &other) {
		id_ = other.id_;
		database_ = other.database_;
		name_ = other.name_;
		setModified(other.modified_);
	}

	return *this;
}

#include <QMetaClassInfo>
QString AMDbObject::dbObjectAttribute(const QString& key) const {
	return AMDbObjectSupport::dbObjectAttribute(this->metaObject(), key);
}


QString AMDbObject::dbPropertyAttribute(const QString& propertyName, const QString& key) const {
	return AMDbObjectSupport::dbPropertyAttribute(this->metaObject(), propertyName, key);
}

QMap<QString, AMDbLoadErrorInfo*> AMDbObject::loadingErrors() const{
	return loadingErrors_;
}


bool AMDbObject::isReloading() const{
	return isReloading_;
}

bool AMDbObject::isStoring() const{
	return isStoring_;
}

// returns the name of the database table where objects like this should be/are stored
QString AMDbObject::dbTableName() const {
	return AMDbObjectSupport::s()->tableNameForClass(this->metaObject());
}

// If this class has already been registered in the AMDbObject system, returns a pointer to the AMDbObjectInfo describing this class's persistent properties.  If the class hasn't been registered, returns 0
const AMDbObjectInfo* AMDbObject::dbObjectInfo() const {
	return AMDbObjectSupport::s()->objectInfoForClass( type() );
}


// This member function updates a scan in the database (if it exists already in that database), otherwise it adds it to the database.
bool AMDbObject::storeToDb(AMDatabase* db, bool generateThumbnails) {

	if(!db){
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_STORE_TO_DB_INVALID_DB, "Could not store to database, the database is invalid. Please report this problem to the Acquaman developers."));
		return false;
	}

	const AMDbObjectInfo* myInfo = dbObjectInfo();
	if(!myInfo){
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_STORE_TO_DB_CLASS_NOT_REGISTERED, "Could not store to database, the class is not registered in the database. Please report this problem to the Acquaman developers."));
		return false;	// class has not been registered yet in the database system.
	}

	QTime saveTime;
	saveTime.start();

	// For performance when storing many child objects, we can speed things up (especially with SQLite, which needs to do a flush and reload of the db file on every write) by doing all the updates in one big transaction. This also ensure consistency.  Because storeToDb() calls could be nested, we don't want to start a transaction if one has already been started.
	bool openedTransaction = false;
	if(db->supportsTransactions() && !db->transactionInProgress()) {
		if(db->startTransaction()) {
			openedTransaction = true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_START_TRANSACTION_TO_SAVE_OBJECT, "Could not start a transaction to save the object '" % myInfo->tableName % ":" % QString::number(id()) % "' in the database. Please report this problem to the Acquaman developers."));
			return false;
		}
	}

	isStoring_ = true;

	// If this object has never been stored to this database before, we could optimize some things.
	bool neverSavedHere = (id()<1 || db !=database());

	QVariantList values;	// list of values to store
	QStringList keys;	// list of keys (column names) to store

	// determine and append the type. (Necessary to know for later, when storing objects of different types in the same table)
	keys << "AMDbObjectType";
	values << type();

	// store all the columns:
	//////////////////////////////////////////////////
	for(int i=0; i<myInfo->columnCount; i++) {

		int columnType = myInfo->columnTypes.at(i);
		QByteArray columnNameBA = myInfo->columns.at(i).toAscii();
		const char* columnName = columnNameBA.constData();

		// add column name to key list... UNLESS the type is an AMDbObjectList. In that case, it gets its own table instead of a column.
		if(columnType != qMetaTypeId<AMDbObjectList>() && columnType != qMetaTypeId<AMConstDbObjectList>())
			keys << myInfo->columns.at(i);

		// add value to values list. First, some special processing is needed for StringList, IntList, and DoubleList types, to join their values into a single string. Other property types simply get written out in their native QVariant form. EXCEPTION: AMDbObjectList doesn't get written here, it gets its own table later.

		if(columnType == qMetaTypeId<AMnDIndex>()) {

			AMnDIndex output = property(columnName).value<AMnDIndex>();
			QStringList resultString;
			for(int i=0; i<output.rank(); i++)
				resultString << QString("%1").arg(output[i]);
			values << resultString.join(AMDbObjectSupport::listSeparator());

		}
		else if(columnType == qMetaTypeId<AMIntList>()) {
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

		else if(columnType == qMetaTypeId<QVector3D>()) {
			QVector3D val = property(columnName).value<QVector3D>();
			QStringList resultString;
			resultString << QString::number(val.x()) << QString::number(val.y()) << QString::number(val.z());
			values << resultString.join(AMDbObjectSupport::listSeparator());
		}

		else if(columnType == qMetaTypeId<AMQVector3DVector>()) {
			AMQVector3DVector vectorList = property(columnName).value<AMQVector3DVector>();
			QStringList resultString;
			foreach(QVector3D d, vectorList)
				resultString << QString("%1%2%3%2%4").arg(d.x()).arg(AMDbObjectSupport::listSeparator()).arg(d.y()).arg(d.z());
			values << resultString.join(AMDbObjectSupport::vectorSeparator());
		}


		else if(columnType == QVariant::StringList || columnType == QVariant::List) {	// string lists, or lists of QVariants that can (hopefully) be converted to strings.
			values << property(columnName).toStringList().join(AMDbObjectSupport::stringListSeparator());
		}

		else if(columnType == qMetaTypeId<AMConstDbObject*>()){
			AMConstDbObject *constObject = property(columnName).value<AMConstDbObject*>();
			if(constObject && !constObject->tableName().isEmpty() && constObject->id() > 0 && db != constObject->database())
				values << QString("%1%2%3%4%5%6").arg("|$^$|").arg(constObject->database()->connectionName()).arg("|$^$|").arg(constObject->tableName()).arg(AMDbObjectSupport::listSeparator()).arg(constObject->id());
			else if(constObject && !constObject->tableName().isEmpty() && constObject->id() > 0)
				values << QString("%1%2%3").arg(constObject->tableName()).arg(AMDbObjectSupport::listSeparator()).arg(constObject->id());
			else
				values << QString();
		}

		// special case: pointers to AMDbObjects: we actually store the object in the database, and then store a string "tableName semicolon id"... which will let us re-load it later.
		else if(columnType == qMetaTypeId<AMDbObject*>()) {
			AMDbObject* obj = property(columnName).value<AMDbObject*>();
			if(obj && obj!=this) {	// if its a valid object, and not ourself (avoid recursion)

				// NBTA David Aug 23, 2013
				bool constStore = (AMDbObjectSupport::dbPropertyAttribute(metaObject(), columnName, "constStore") == QString("true"));
				// Handle situations where the object to be stored is already stored in another database (use redirection)
				if(obj->database() && (obj->database() != db) ){

					if( (!obj->modified() && (obj->id() >= 1)) ||  constStore){

						values << QString("%1%2%3%4%5%6").arg("|$^$|").arg(obj->database()->connectionName()).arg("|$^$|").arg(obj->dbTableName()).arg(AMDbObjectSupport::listSeparator()).arg(obj->id());
					}
					else{
						if(obj->storeToDb(obj->database()))
							values << QString("%1%2%3%4%5%6").arg("|$^$|").arg(obj->database()->connectionName()).arg("|$^$|").arg(obj->dbTableName()).arg(AMDbObjectSupport::listSeparator()).arg(obj->id());
						else
							values << QString();// storing empty string: indicates failure to save object here.
					}
				}

				else if( (!obj->modified() && obj->database()==db && obj->id() >=1) || constStore){	// if it's not modified, and already part of this database... don't need to store it. Just remember where it is...
					values << QString("%1%2%3").arg(obj->dbTableName()).arg(AMDbObjectSupport::listSeparator()).arg(obj->id());
				}
				else {
					if(obj->storeToDb(db))
						values << QString("%1%2%3").arg(obj->dbTableName()).arg(AMDbObjectSupport::listSeparator()).arg(obj->id());
					else
						values << QString();// storing empty string: indicates failure to save object here.
				}
			}
			else
				values << QString();// storing empty string: indicates invalid object to save here.
		}

		// special case: lists of AMDbObject pointers. Interpreted as a one-to-many (or maybe many-to-many) relationship.
		else if(columnType == qMetaTypeId<AMDbObjectList>() || columnType == qMetaTypeId<AMConstDbObjectList>()) {
			// don't do anything here. Instead, we'll save all the objects, and add their location entries, once we know our id
			// most importantly, DON'T add anything to values, since we didn't add a matching key.
		}

		else if(columnType == qMetaTypeId<AMHighPrecisionDateTime>()){
			values << property(columnName).value<AMHighPrecisionDateTime>().dateTime().toString("yyyy-MM-ddThh:mm:ss.zzz");
		}
		// everything else
		else
			values << property(columnName);
	}
	////////////////////////////////////////



	// Add thumbnail info (just the count for now: 0) We will update later once we store thumbnails (possibly in another thread).
	keys << "thumbnailCount";
	values << 0;

	// store type, thumbnailCount, and all metadata into the table.
	int retVal;
	// If saving into same database, can use existing id():
	if(database() == db)
		retVal = db->insertOrUpdate(id(), myInfo->tableName, keys, values);
	// otherwise, use id of 0 to insert new.
	else
		retVal = db->insertOrUpdate(0, myInfo->tableName, keys, values);


	// Did the update succeed?
	if(retVal == 0) {
		if(openedTransaction)
			db->rollbackTransaction();	// this is good for consistency. Even all the child object changes will be reverted if this save failed.
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_STORE_TO_DB_INSERT_OR_UPDATE_FAILED, QString("Could not store to db (%1), the insert or update call failed. Please report this problem to the Acquaman developers.").arg(db->connectionName())));
		isStoring_ = false;
		return false;
	}

	// Success! We have our new / old id:
	id_ = retVal;
	database_ = db;


	// AMDbObjectList associated objects save
	////////////////////////////////////////////
	for(int i=0; i<myInfo->columnCount; i++) {
		if(myInfo->columnTypes.at(i) == qMetaTypeId<AMDbObjectList>()) {	// only do this for AMDbObjectList types...

			QByteArray columnNameBA = myInfo->columns.at(i).toAscii();
			const char* columnName = columnNameBA.constData();

			AMDbObjectList objList = property(columnName).value<AMDbObjectList>();
			QString auxTableName = myInfo->tableName + "_" + myInfo->columns.at(i);
			// delete old entries for this object and property if necessary:
			QList<int> matchList = db->objectsMatching(auxTableName, "id1", id());
			if(matchList.count() > 0)
				db->deleteRows(auxTableName, QString("id1 = '%1'").arg(id()));

			QStringList clist;
			clist << "id1" << "table1" << "id2" << "table2";
			QVariantList vlist;
			vlist << id() << myInfo->tableName << int(0) << "tableName2";	// int(0) and "tableName2" are dummy variables for now.
			foreach(AMDbObject* obj, objList) {
				if(obj && obj!=this) {	// verify that this is a valid object, and not ourself (to avoid infinite recursion)
					if(  (!obj->modified() && obj->database() == db && obj->id() >=1) ) {	// if this object is unmodified and already stored in the database, just remember its location. (This avoids storeToDb()'ing objects that are unmodified and do not need re-saving.)
						vlist[2] = obj->id();
						vlist[3] = obj->dbTableName();
						db->insertOrUpdate(0, auxTableName, clist, vlist);
					}
					else if( obj->storeToDb(db) ) {	// otherwise, store the object and remember its location
						vlist[2] = obj->id();
						vlist[3] = obj->dbTableName();
						db->insertOrUpdate(0, auxTableName, clist, vlist);
					}
					else {	// problem storing the object...
						AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_ERROR_STORING_CHILD_OBJECT, QString("While storing '%1' to the database, there was an error trying to store its child object '%2'").arg(this->name()).arg(obj->name())));
						// We let this slide and don't give up on ourself and the rest of the child objects.
					}
				}
			}
		}
		else if(myInfo->columnTypes.at(i) == qMetaTypeId<AMConstDbObjectList>()){
			QByteArray columnNameBA = myInfo->columns.at(i).toAscii();
			const char* columnName = columnNameBA.constData();

			AMConstDbObjectList objList = property(columnName).value<AMConstDbObjectList>();
			QString auxTableName = myInfo->tableName + "_" + myInfo->columns.at(i);
			// delete old entries for this object and property if necessary:
			QList<int> matchList = db->objectsMatching(auxTableName, "id1", id());
			if(matchList.count() > 0)
				db->deleteRows(auxTableName, QString("id1 = '%1'").arg(id()));

			QStringList clist;
			clist << "id1" << "table1" << "id2" << "table2";
			QVariantList vlist;
			vlist << id() << myInfo->tableName << int(0) << "tableName2";	// int(0) and "tableName2" are dummy variables for now.
			foreach(AMConstDbObject* constObject, objList) {
				if(constObject){
					vlist[2] = constObject->id();
					vlist[3] = constObject->tableName();
					db->insertOrUpdate(0, auxTableName, clist, vlist);
				}
			}
		}
	}


	// Thumbnail save (if we're supposed to do it in this thread)
	///////////////////////////////////////////

	if(generateThumbnails && thumbnailCount() > 0 && !shouldGenerateThumbnailsInSeparateThread())
		updateThumbnailsInCurrentThread(neverSavedHere);

	// NOTE: currently there are a few situations where we are "leaking" thumbnails: leaving old stale thumbnails in the database. Ex: When the thumbnailCount() was non-zero on a previous save to this database, and is now 0. Or when the thumbnailCount() was non-zero on a previous save to the database, and generateThumbnails has been forced to false this time. That's not such a big deal... they're not referenced by anything, and they'll get removed next time we store valid thumbnails.

	// finalizing... Commit the transaction if we opened it.
	if(openedTransaction) {
		if(db->commitTransaction()) {
			// we were just stored to the database, so our properties must be in sync with it.
			setModified(false);
			if(shouldGenerateThumbnailsInSeparateThread() && generateThumbnails && thumbnailCount() > 0) {
				QtConcurrent::run(&AMDbObject::updateThumbnailsInSeparateThread, db, id_, myInfo->tableName, neverSavedHere);
			}
			isStoring_ = false;
			emit storedToDb();
			return true;
		}
		else {
			db->rollbackTransaction();
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_COMPLETE_TRANSACTION_TO_SAVE_OBJECT, "Could not commit a transaction to save the object '" % myInfo->tableName % ":" % QString::number(id()) % "' in the database. Please report this problem to the Acquaman developers."));
			isStoring_ = false;
			return false;
		}
	}
	// if we didn't open the transaction, no need to commit it.
	else {
		// we were just stored to the database, so our properties must be in sync with it.
		setModified(false);
		if(shouldGenerateThumbnailsInSeparateThread() && generateThumbnails && thumbnailCount() > 0) {
			QtConcurrent::run(&AMDbObject::updateThumbnailsInSeparateThread, db, id_, myInfo->tableName, neverSavedHere);
		}
		isStoring_ = false;
		emit storedToDb();
		return true;
	}
}


// load a AMDbObject (set its properties) by retrieving it based on id.
bool AMDbObject::loadFromDb(AMDatabase* db, int sourceId) {
	// All valid database id's start at 1. This is an optimization to omit the db query if it won't find anything.
	if(sourceId < 1){
		AMErrorMon::debug(this, AMDBOBJECT_CANNOT_LOAD_FROM_DB_INVALID_ID, QString("Could not load from database, the database sourceId (%1) is invalid. Please report this problem to the Acquaman developers.").arg(sourceId));
		return false;
	}

	const AMDbObjectInfo* myInfo = dbObjectInfo();
	if(!myInfo){
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_LOAD_FROM_DB_NO_VALUES_RETRIEVED_FROM_TABLE, QString("Could not load from database, the request to retrieve values (sourceId: %1) returned empty. Please report this problem to the Acquaman developers.").arg(sourceId)));
		return false;	// class hasn't been registered yet with the database system.
	}

	QDateTime beforeTime = QDateTime::currentDateTime();

	// Retrieve all columns from the database.
	// optimization: not necessary to retrieve anything with the doNotLoad attribute set. Also, if the type is AMDbObjectList, there is no actual database column for this "column"... instead, its an auxiliary table.
	QStringList keys;	// keys is the set of database columns to retrieve, all the columns that are loadable, and are not of type AMDbObjectList.
	for(int i=0; i<myInfo->columnCount; i++)
		if(myInfo->columnTypes.at(i) != qMetaTypeId<AMDbObjectList>() && myInfo->columnTypes.at(i) != qMetaTypeId<AMConstDbObjectList>() && myInfo->isLoadable.at(i))
			keys << myInfo->columns.at(i);

	QVariantList values = db->retrieve( sourceId, myInfo->tableName, keys);

	if(values.isEmpty()){
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_LOAD_FROM_DB_NO_VALUES_RETRIEVED_FROM_TABLE, "Could not load from database, the request to retrieve values returned empty. Please report this problem to the Acquaman developers."));
		return false;
	}

	isReloading_ = true;
	// if we just successfully loaded out of here, then we have our new id() and database().
	id_ = sourceId;
	database_ = db;

	int ri = 0;	// the AMDbObjectInfo::columnCount will not match the number of columns we retrieved, given that some are omitted. This is the index in the retrieved columns 'values'. It will become offset from 'i' in the loop below if there are any non-loadable columns, or AMDbObjecList columns which don't have actual database columns.
	// go through all results and restore properties
	for(int i=0; i<myInfo->columnCount; i++) {

		// do not re-load this column?
		if(!myInfo->isLoadable.at(i))
			continue;

		QByteArray columnNameBA = myInfo->columns.at(i).toAscii();
		const char* columnName = columnNameBA.constData();
		int columnType = myInfo->columnTypes.at(i);

		// special action necessary to convert StringList, IntList, and DoubleList types which have been returned as strings, as well as re-load AMDbObjects or lists of AMDbObjects that are owned by this object. Determine based on column type:

		// if its an AMDbObjectList property, it doesn't have an actual column. Look in the auxiliary table instead.
		if(columnType == qMetaTypeId<AMDbObjectList>() || columnType == qMetaTypeId<AMConstDbObjectList>()) {

			if(columnType == qMetaTypeId<AMDbObjectList>()){
				// grab current AMDbObjectList using property() and check if existing count and types match. In that case, can call loadFromDb() on each of them.
				// otherwise, create new objects with createAndLoadObjectAt(), and then call setProperty().
				AMDbObjectList reloadedObjects;
				AMDbObjectList existingObjects = property(columnName).value<AMDbObjectList>();

				QString auxTableName = myInfo->tableName + "_" + myInfo->columns.at(i);
				QList<int> storedObjectRows = db->objectsMatching(auxTableName, "id1", id());

				bool canUseExistingObjects = (storedObjectRows.count() == existingObjects.count());	// one prereq for reloading using existing objects: count is the same.
				QStringList storedObjectTables;
				QList<int> storedObjectIds;
				QStringList clist;  clist << "id2" << "table2";
				for(int r=0; r<storedObjectRows.count(); r++) {
					QVariantList objectLocation = db->retrieve(storedObjectRows.at(r), auxTableName, clist);
					if(objectLocation.isEmpty()){
						AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_LOAD_FROM_DB_AMDBOBJECTLIST_TABLE_LOCATION_INVALID, "Could not load from database, the request to get an AMDbObjectList points to an invalid table location. Please report this problem to the Acquaman developers."));
						isReloading_ = false;
						return false;
					}
					QString objectTable = objectLocation.at(1).toString();
					int objectId = objectLocation.at(0).toInt();
					storedObjectTables << objectTable;
					storedObjectIds << objectId;

					// second prereq for re-using existing objects is that current types and stored types match.
					canUseExistingObjects = (canUseExistingObjects &&
								 existingObjects.at(r)->type() == AMDbObjectSupport::typeOfObjectAt(db, objectTable, objectId) );
				}

				if(canUseExistingObjects) {
					for(int r=0; r<existingObjects.count(); r++)
						existingObjects.at(r)->loadFromDb(db, storedObjectIds.at(r));
				}
				else {
					for(int r=0; r<storedObjectIds.count(); r++)
						reloadedObjects << AMDbObjectSupport::s()->createAndLoadObjectAt(db, storedObjectTables.at(r), storedObjectIds.at(r));
					setProperty(columnName, QVariant::fromValue(reloadedObjects));
				}
			}
			else if(columnType == qMetaTypeId<AMConstDbObjectList>()){
				// grab current AMDbObjectList using property() and check if existing count and types match. In that case, can call loadFromDb() on each of them.
				// otherwise, create new objects with createAndLoadObjectAt(), and then call setProperty().
				AMConstDbObjectList reloadedObjects;
				AMConstDbObjectList existingObjects = property(columnName).value<AMConstDbObjectList>();

				QString auxTableName = myInfo->tableName + "_" + myInfo->columns.at(i);
				QList<int> storedObjectRows = db->objectsMatching(auxTableName, "id1", id());

				bool canUseExistingObjects = (storedObjectRows.count() == existingObjects.count());	// one prereq for reloading using existing objects: count is the same.
				QStringList storedObjectTables;
				QList<int> storedObjectIds;
				QStringList clist;  clist << "id2" << "table2";
				for(int r=0; r<storedObjectRows.count(); r++) {
					QVariantList objectLocation = db->retrieve(storedObjectRows.at(r), auxTableName, clist);
					if(objectLocation.isEmpty()){
						AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_LOAD_FROM_DB_AMCONSTDBOBJECTLIST_TABLE_LOCATION_INVALID, "Could not load from database, the request to get an AMConstDbObjectList points to an invalid table location. Please report this problem to the Acquaman developers."));
						isReloading_ = false;
						return false;
					}
					QString objectTable = objectLocation.at(1).toString();
					int objectId = objectLocation.at(0).toInt();
					storedObjectTables << objectTable;
					storedObjectIds << objectId;
				}

				if(canUseExistingObjects) {
					for(int r=0; r<existingObjects.count(); r++)
						existingObjects.at(r)->setLazyLoadingInfo(db, storedObjectTables.at(r), storedObjectIds.at(r));
				}
				else {
					for(int r=0; r<storedObjectIds.count(); r++)
						reloadedObjects << new AMConstDbObject(db, storedObjectTables.at(r), storedObjectIds.at(r));
					setProperty(columnName, QVariant::fromValue(reloadedObjects));
				}
			}
		}
		// in all other cases, we're using up actual columns in the result set, so ri should be incremented after all of these:
		else {

			if(columnType == qMetaTypeId<AMDbObject*>()) {	// stored owned AMDbObject. reload from separate location in database.

				// Determine the database to load from, in case this is a redirected object
				AMDatabase *databaseToUse;
				QString columnValue = values.at(ri).toString();
				if(columnValue.contains("|$^$|") && values.at(ri).toString().count("|$^$|") == 2 ){ // check to determine if this AMDbObject is actually in a differen database (in case this is in the actions database and some information it needs is in the user database)
					QString databaseName = columnValue.split("|$^$|").at(1);
					columnValue = columnValue.split("|$^$|").last();
					databaseToUse = AMDatabase::database(databaseName);
					if(!databaseToUse){
						AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_LOAD_FROM_DB_BAD_DB_REDIRECT, QString("Could not load from database, id %1 in table %2 (%3 database) attempted to redirect to a bad database named %4. Please report this problem to the Acquaman developers.").arg(sourceId).arg(myInfo->tableName).arg(db->connectionName()).arg(databaseName)));
						isReloading_ = false;
						return false;	// bad redirection to another database
					}
				}
				else // not a redirected object
					databaseToUse = db;

				QStringList objectLocation = columnValue.split(AMDbObjectSupport::listSeparator());	// location was saved as string: "tableName semicolon id"
				if(objectLocation.count() == 2) {
					QString tableName = objectLocation.at(0);
					int dbId = objectLocation.at(1).toInt();
					AMDbObject* existingObject = property(columnName).value<AMDbObject*>();
					// have a valid existing object, and its type matches the type to load? Just call loadFromDb() and keep the existing object.
					if(existingObject && existingObject->type() == AMDbObjectSupport::typeOfObjectAt(databaseToUse, tableName, dbId))
						existingObject->loadFromDb(databaseToUse, dbId);
					else {
						AMDbObject* reloadedObject = AMDbObjectSupport::s()->createAndLoadObjectAt(databaseToUse, tableName, dbId);
						if(reloadedObject)
							setProperty(columnName, QVariant::fromValue(reloadedObject));
						else{
							setProperty(columnName, QVariant::fromValue((AMDbObject*)0));	// if it wasn't reloaded successfully, you'll still get a setProperty call, but it will be with a null pointer.
							if(AMErrorMon::lastErrorCode() == AMDBOBJECTSUPPORT_CANNOT_LOAD_OBJECT_NOT_REGISTERED_TYPE)
								loadingErrors_.insert(QString(columnName), new AMDbLoadErrorInfo(databaseToUse->connectionName(), tableName, dbId));
						}
					}
				}
				else
					setProperty(columnName, QVariant::fromValue((AMDbObject*)0));	// if it wasn't reloaded successfully, you'll still get a setProperty call, but it will be with a null pointer.

			}
			else if(columnType == qMetaTypeId<AMConstDbObject*>()){
				// Determine the database to load from, in case this is a redirected object
				AMDatabase *databaseToUse;
				QString columnValue = values.at(ri).toString();
				if(columnValue.contains("|$^$|") && values.at(ri).toString().count("|$^$|") == 2 ){ // check to determine if this AMDbObject is actually in a differen database (in case this is in the actions database and some information it needs is in the user database)
					QString databaseName = columnValue.split("|$^$|").at(1);
					columnValue = columnValue.split("|$^$|").last();
					databaseToUse = AMDatabase::database(databaseName);
					if(!databaseToUse){
						AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_LOAD_FROM_DB_BAD_CONSTDB_REDIRECT, QString("Could not load constDbObject from database, id %1 in table %2 (%3 database) attempted to redirect to a bad database named %4. Please report this problem to the Acquaman developers.").arg(sourceId).arg(myInfo->tableName).arg(db->connectionName()).arg(databaseName)));
						isReloading_ = false;
						return false;	// bad redirection to another database
					}
				}
				else // not a redirected object
					databaseToUse = db;

				AMConstDbObject *constObject = 0;

				QStringList objectLocation = columnValue.split(AMDbObjectSupport::listSeparator());	// location was saved as string: "tableName semicolon id"
				if(objectLocation.count() == 2) {
					QString tableName = objectLocation.at(0);
					int dbId = objectLocation.at(1).toInt();
					constObject = new AMConstDbObject(databaseToUse, tableName, dbId);
					setProperty(columnName, QVariant::fromValue(constObject));
				}
				else{
					// if it wasn't reloaded successfully, you'll still get a setProperty call, but it will be with a null pointer.
					setProperty(columnName, QVariant::fromValue((AMConstDbObject*)0));
//					constObject = new AMConstDbObject(0);
				}

			}
			else if(columnType == qMetaTypeId<AMnDIndex>()) {
				AMnDIndex ndIndex;
				QStringList stringList = values.at(ri).toString().split(AMDbObjectSupport::listSeparator(), QString::SkipEmptyParts);
				foreach(QString i, stringList)
					ndIndex.append(i.toInt());
				setProperty(columnName, QVariant::fromValue(ndIndex));
			}
			else if(columnType == qMetaTypeId<AMIntList>()) {	// integer lists: must convert back from separated string.
				AMIntList intList;
				QStringList stringList = values.at(ri).toString().split(AMDbObjectSupport::listSeparator(), QString::SkipEmptyParts);
				foreach(QString i, stringList)
					intList << i.toInt();
				setProperty(columnName, QVariant::fromValue(intList));
			}
			else if(columnType == qMetaTypeId<AMDoubleList>()) {	// double lists: must convert back from separated string.
				AMDoubleList doubleList;
				QStringList stringList = values.at(ri).toString().split(AMDbObjectSupport::listSeparator(), QString::SkipEmptyParts);
				foreach(QString d, stringList)
					doubleList << d.toDouble();
				setProperty(columnName, QVariant::fromValue(doubleList));
			}
			else if(columnType == qMetaTypeId<QVector3D>()) {
				QVector3D vector;
				QStringList stringList = values.at(ri).toString().split(AMDbObjectSupport::listSeparator(), QString::SkipEmptyParts);
				if(stringList.size() == 3) {
					vector = QVector3D(stringList.at(0).toDouble(), stringList.at(1).toDouble(), stringList.at(2).toDouble());
				}
				else
					AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_3D_POINT_MISSING_3_NUMBERS, "Couldn't find 3 numbers when attempting to load a 3D geometry point from the database."));
				setProperty(columnName, QVariant::fromValue(vector));
			}
			else if(columnType == qMetaTypeId<AMQVector3DVector>())
			{
				AMQVector3DVector vector3DList;
				QStringList stringList = values.at(ri).toString().split(AMDbObjectSupport::vectorSeparator(), QString::SkipEmptyParts);
				foreach(QString i, stringList){
					QStringList subList = i.split(AMDbObjectSupport::listSeparator());
					QVector3D vector;
					if(subList.size() == 3)
					{
						vector = QVector3D(subList.at(0).toDouble(), subList.at(1).toDouble(), subList.at(2).toDouble());
					}
					else
					{
						AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_3D_POINT_MISSING_3_NUMBERS_IN_AMQVECTOR3DVECTOR, "Couldn't find 3 numbers when attempting to load a 3D geometry point for an AMQVector3DVector from the database."));
					}
					vector3DList<<vector;
				}
				setProperty(columnName, QVariant::fromValue(vector3DList));
			}
			else if(columnType == QVariant::StringList || columnType == QVariant::List) {	// string list, and anything-else-lists saved as string lists: must convert back from separated string.
				setProperty(columnName, values.at(ri).toString().split(AMDbObjectSupport::stringListSeparator(), QString::SkipEmptyParts));
			}
			else if(columnType == qMetaTypeId<AMHighPrecisionDateTime>()){
				AMHighPrecisionDateTime hpDateTime;
				hpDateTime.setDateTime(values.at(ri).toDateTime());
				setProperty(columnName, QVariant::fromValue(hpDateTime));
			}
			else {	// the simple case.
				setProperty(columnName, values.at(ri));
			}

			ri++;// we just used up this result column, so move on to the next.
		}
	}

	// we were just loaded out of the database, so we must be in-sync.
	setModified(false);

	isReloading_ = false;
	emit loadedFromDb();

	QDateTime afterTime = QDateTime::currentDateTime();

	return true;
}

bool AMDbObject::loadFromDb(AMDatabase* db, const QString &whereClause)
{
	// All valid database id's start at 1. This is an optimization to omit the db query if it won't find anything.
	if(whereClause.length() == 0){
		AMErrorMon::debug(this, AMDBOBJECT_CANNOT_LOAD_FROM_DB_INVALID_ID, "Could not load from database, the given where clause is empty. Please report this problem to the Acquaman developers.");
		return false;
	}

	const AMDbObjectInfo* myInfo = dbObjectInfo();
	if(!myInfo){
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_LOAD_FROM_DB_CLASS_NOT_REGISTERED, "Could not load from database, the class is not registered with the database. Please report this problem to the Acquaman developers."));
		return false; // class hasn't been registered yet with the database system.
	}

	QList<int> objectId = db->objectsWhere(myInfo->tableName, whereClause);
		if (objectId.count() != 1) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_LOAD_FROM_DB_NO_VALUES_RETRIEVED_FROM_TABLE, QString("Could not load from database, the retrieved object is empty or not unique (%1). Please report this problem to the Acquaman developers.").arg(objectId.count())));
		return false;
	}

	return loadFromDb(db, objectId.at(0));
}

// This global function enables using the insertion operator to add objects to the database
///		ex: *Database::db() << myScan
// Because AMDbObject::storeToDb() is virtual, this version can be used properly for all sub-types of AMDbObject.
AMDatabase& operator<<(AMDatabase& db, AMDbObject& s) {
	s.storeToDb(&db);
	return db;
}

void AMDbObject::dissociateFromDb(bool shouldDissociateChildren)
{
	id_ = 0;
	database_ = 0;

	const AMDbObjectInfo* myInfo = dbObjectInfo();
	if(!myInfo)
		return;	// class has not been registered yet in the database system. Nothing to do.

	if(shouldDissociateChildren) {

		// Dissociating children. Go through all columns...
		for(int i=0; i<myInfo->columnCount; i++) {

			QByteArray columnNameBA = myInfo->columns.at(i).toAscii();
			const char* columnName = columnNameBA.constData();

			if(myInfo->columnTypes.at(i) == qMetaTypeId<AMDbObject*>()) {	// single child objects
				AMDbObject* obj = property(columnName).value<AMDbObject*>();
				if(obj && obj!=this)
					obj->dissociateFromDb(true);
			}
			if(myInfo->columnTypes.at(i) == qMetaTypeId<AMDbObjectList>()) {	// Lists of child objects
				AMDbObjectList objList = property(columnName).value<AMDbObjectList>();
				foreach(AMDbObject* obj, objList) {
					if(obj && obj!=this)
						obj->dissociateFromDb(true);
				}
			}
		}
	}
}

#include <QCoreApplication>

void AMDbObject::updateThumbnailsInSeparateThread(AMDatabase *db, int id, const QString& dbTableName, bool neverSavedHereBefore) {

	// Step 1: try to load the object.
	AMDbObject* object = AMDbObjectSupport::s()->createAndLoadObjectAt(db, dbTableName, id);
	if(!object) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, AMDBOBJECT_ERROR_LOADING_OBJECT_TO_CREATE_THUMBNAILS, QString("AMDbObject: error trying to load object with ID %1 out of '%2' to create thumbnails. Please report this bug to the Acquaman developers.").arg(id).arg(dbTableName)));
		return;
	}

	// Find out how many thumbnails we're supposed to have:
	int thumbsCount = object->thumbnailCount();
	if(thumbsCount == 0) {
		return;	// nothing else to do...
	}

	QTime renderTime;
	renderTime.start();
	// Generating the thumbnails could take some time, especially for things that do complicated drawing (ie: scans with lots of points or multi-dim data). Let's do it all before hitting the database
	QList<AMDbThumbnail> thumbnails;
	for(int i=0; i<thumbsCount; i++)
		thumbnails << object->thumbnail(i);

	AMErrorMon::debug(0, AMDBOBJECT_DEBUG_OUTPUT, QString("%1 took %2ms to create thumbnails for saving.").arg(object->type()).arg(renderTime.elapsed()) );

	QCoreApplication::postEvent(AMDbObjectSupport::s(), new AMDbThumbnailsGeneratedEvent(thumbnails, db, dbTableName, id, neverSavedHereBefore));

	// And now we're done with the object...
	object->deleteLater();
}

void AMDbObject::updateThumbnailsInCurrentThread(bool neverSavedHereBefore)
{
	QString databaseTableName = dbTableName();

	// Find out how many thumbnails we're supposed to have:
	int thumbsCount = thumbnailCount();
	if(thumbsCount == 0) {
		return;	// nothing else to do...
	}
	// Generating the thumbnails could take some time, especially for things that do complicated drawing (ie: scans with lots of points or multi-dim data). Let's do it all before hitting the database
	QList<AMDbThumbnail> thumbnails;
	for(int i=0; i<thumbsCount; i++)
		thumbnails << thumbnail(i);

	// The remainder of this should happen in one database transaction. This ensures consistency, and it also increases performance because a database commit (and time consuming flush-to-disk, in the case of SQLite) doesn't have to happen for each thumbnail insert -- just once at the end.
	// Note that there might be a transaction open already and we cannot nest transactions. Therefore, only start a transaction if not open already.
	bool openedTransaction = false;
	if(database()->supportsTransactions() && !database()->transactionInProgress()) {
		if(database()->startTransaction()) {
			openedTransaction = true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_START_TRANSACTION_TO_SAVE_THUMBNAILS, "Could not start a transaction to save the thumbnails for object '" % databaseTableName % ":" % QString::number(id()) % "' in the database. Please report this problem to the Acquaman developers."));
			return;
		}
	}

	// Do we need to delete or reuse some existing thumbnails?
	bool reuseThumbnailIds = false;
	QList<int> existingThumbnailIds;

	if(!neverSavedHereBefore) {	// optimization: only need to do this if we've been saved here before.

		// Find out if there are any thumbnails for this object already in the DB:
		existingThumbnailIds = database()->objectsWhere(AMDbObjectSupport::thumbnailTableName(), QString("objectId = %1 AND objectTableName = '%2'").arg(id()).arg(databaseTableName));

		reuseThumbnailIds = (existingThumbnailIds.count() == thumbsCount);	// can reuse if the number of old and new ones matches.
		// need to check that the existingThumbnailIds are consecutive as well... ie: in a sequential block. Otherwise can't reuse them.
		// normally this is the case, unless we've forgotten / orphaned some. Doesn't hurt to check.

		// If we shouldn't reuse existing rows in the thumbnail table: delete old before appending new ones.
		if(!reuseThumbnailIds) {
			database()->deleteRows(AMDbObjectSupport::thumbnailTableName(), QString("objectId = %1 AND objectTableName = '%2'").arg(id()).arg(databaseTableName));
		}
	}

	QVariantList values;	// list of values to store
	QStringList keys;	// list of keys (column names) to store
	int firstThumbnailId = 0;

	// Save each thumbnail:
	for(int i=0; i<thumbsCount; i++) {
		const AMDbThumbnail& t = thumbnails.at(i);

		keys.clear();
		values.clear();

		keys << "objectId";
		values << id();
		keys << "objectTableName";
		values << databaseTableName;
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

		int retVal;
		if(reuseThumbnailIds) {
			retVal = database()->insertOrUpdate(i+existingThumbnailIds.at(0), AMDbObjectSupport::thumbnailTableName(), keys, values);
		}
		else {
			retVal = database()->insertOrUpdate(0, AMDbObjectSupport::thumbnailTableName(), keys, values);
		}
		if(retVal == 0) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, AMDBOBJECT_ERROR_SAVING_THUMBNAILS, QString("AMDbObject: error trying to save thumbnails for object with ID %1 in table '%2'. Please report this bug to the Acquaman developers.").arg(id()).arg(databaseTableName)));
			if(openedTransaction) {
				database()->rollbackTransaction();
				return;
			}
		}

		if(i == 0)	// when inserting the first one... remember the id of this first thumbnail.
			firstThumbnailId = retVal;
	}

	// now that we know where the thumbnails are, update this in the object table
	if(!database()->update(id(),
						   databaseTableName,
						   QStringList() << "thumbnailCount" << "thumbnailFirstId",
						   QVariantList() << thumbsCount << firstThumbnailId)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, AMDBOBJECT_ERROR_STORING_UPDATED_THUMBNAIL_COUNT_AND_FIRST_ID, QString("AMDbObject: error trying to store the updated thumbnail count and firstThumbnailId for database object %1 in table '%2'. Please report this bug to the Acquaman developers.").arg(id()).arg(databaseTableName)));
		if(openedTransaction) {
			database()->rollbackTransaction();
			return;
		}
	}

	// only commit the transaction if we started it.
	if(openedTransaction && !database()->commitTransaction()) {
		database()->rollbackTransaction();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDBOBJECT_CANNOT_COMPLETE_TRANSACTION_TO_SAVE_THUMBNAILS, "Could not commit a transaction to save the thumbnails for object '" % databaseTableName % ":" % QString::number(id()) % "' in the database. Please report this problem to the Acquaman developers."));
	}
}

 AMDbThumbnailsGeneratedEvent::~AMDbThumbnailsGeneratedEvent(){}
AMDbThumbnailsGeneratedEvent::AMDbThumbnailsGeneratedEvent(const QList<AMDbThumbnail> &_thumbnails, AMDatabase *_db, const QString &_dbTableName, int _dbObjectId, bool _neverSavedHereBefore)
	: QEvent((QEvent::Type)AM::ThumbnailsGeneratedEvent), thumbnails(_thumbnails), db(_db), dbTablename(_dbTableName), dbObjectId(_dbObjectId), neverSavedHereBefore(_neverSavedHereBefore)
{
}

void AMDbObject::setModified(bool isModified)
{
	// First one is to allow the db object to be reset after being loaded from the database.
	if (database() && database()->isReadOnly()){

		if (modified_)
			emit modifiedChanged(modified_ = false);
	}

	else if (isModified != modified_)
		emit modifiedChanged(modified_ = isModified);
}

