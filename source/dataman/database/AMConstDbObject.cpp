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


#include "AMConstDbObject.h"

#include "dataman/database/AMDbObjectSupport.h"

AMConstDbObject::AMConstDbObject(const AMDbObject *object, QObject *parent) :
	QObject(parent)
{
	initialize();
	setObject(object);
}

AMConstDbObject::AMConstDbObject(AMDatabase *db, const QString &tableName, int id, QObject *parent) :
	QObject(parent)
{
	initialize();

	setLazyLoadingInfo(db, tableName, id);
}

const AMDbObject* AMConstDbObject::object() const{
	if(!object_ && db_ && !tableName_.isEmpty() && id_ > 0)
		object_ = AMDbObjectSupport::s()->createAndLoadObjectAt(db_, tableName_, id_);
	if(object_){
		connect(object_, SIGNAL(storedToDb()), this, SLOT(onObjectStoredToDb()));
		connect(object_, SIGNAL(destroyed()), this, SLOT(onObjectDestroyed()));
	}
	return object_;
}

AMDatabase* AMConstDbObject::database() const{
	return db_;
}

QString AMConstDbObject::tableName() const{
	return tableName_;
}

int AMConstDbObject::id() const{
	return id_;
}

bool AMConstDbObject::isFullyLoaded() const{
	if(object_)
		return true;
	return false;
}

void AMConstDbObject::setObject(const AMDbObject *object){
	if(object_){
		disconnect(object_, SIGNAL(storedToDb()), this, SLOT(onObjectStoredToDb()));
		disconnect(object_, SIGNAL(destroyed()), this, SLOT(onObjectDestroyed()));
	}

	object_ = object;

	if(object_){
		connect(object_, SIGNAL(storedToDb()), this, SLOT(onObjectStoredToDb()));
		connect(object_, SIGNAL(destroyed()), this, SLOT(onObjectDestroyed()));
		db_ = object_->database();
		tableName_ = object_->dbTableName();
		id_ = object_->id();
	}
}

void AMConstDbObject::setLazyLoadingInfo(AMDatabase *db, const QString &tableName, int id){
	if(object_)
		disconnect(object_, SIGNAL(storedToDb()), this, SLOT(onObjectStoredToDb()));
	object_ = 0;

	db_ = db;
	tableName_ = tableName;
	id_ = id;
}

void AMConstDbObject::onObjectStoredToDb(){
	if(object_){
		if(db_ != object_->database())
			db_ = database();
		if(tableName_ != object_->dbTableName())
			tableName_ = object_->dbTableName();
		if(id_ != object_->id())
			id_ = object_->id();
	}
}

void AMConstDbObject::onObjectDestroyed(){
	object_ = 0; //NULL
}

void AMConstDbObject::initialize(){
	object_ = 0; //NULL

	db_ = 0; //NULL
	tableName_ = "";
	id_ = -1;
}
