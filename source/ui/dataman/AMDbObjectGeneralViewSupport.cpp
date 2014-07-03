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


#include "AMDbObjectGeneralViewSupport.h"

#include <QMetaObject>

#include "dataman/database/AMDbObject.h"
#include "ui/dataman/AMDbObjectGeneralView.h"

 AMDbObjectGeneralViewObjectInfo::~AMDbObjectGeneralViewObjectInfo(){}
AMDbObjectGeneralViewObjectInfo::AMDbObjectGeneralViewObjectInfo(AMDbObject *prototypeObject, AMDbObjectGeneralView *prototypeGeneralView){
	initWithMetaObject(prototypeObject->metaObject(), prototypeGeneralView->metaObject());
}

AMDbObjectGeneralViewObjectInfo::AMDbObjectGeneralViewObjectInfo(const QMetaObject *useObjectMetaObject, const QMetaObject *useGeneralViewMetaObject){
	if(inheritsAMDbObject(useObjectMetaObject) && inheritsAMDbObjectGeneralView(useGeneralViewMetaObject))
		initWithMetaObject(useObjectMetaObject, useGeneralViewMetaObject);
	else{
		objectMetaObject = 0;
		generalViewMetaObject = 0;
	}
}

void AMDbObjectGeneralViewObjectInfo::initWithMetaObject(const QMetaObject *useObjectMetaObject, const QMetaObject *useGeneralViewMetaObject){
	objectMetaObject = useObjectMetaObject;
	generalViewMetaObject = useGeneralViewMetaObject;
	objectClassName = objectMetaObject->className();
	generalViewClassName = generalViewMetaObject->className();
}

bool AMDbObjectGeneralViewObjectInfo::inheritsAMDbObject(const QMetaObject *metaObject) const{
	// is this a subclass of AMDbObject? (Or an AMDbObject itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsAMDbObject;
	do {
		inheritsAMDbObject = (superClass->className() == QString("AMDbObject"));
	}
	while( (superClass=superClass->superClass()) && inheritsAMDbObject == false );
	return inheritsAMDbObject;
}

bool AMDbObjectGeneralViewObjectInfo::inheritsAMDbObjectGeneralView(const QMetaObject *metaObject) const{
	// is this a subclass of AMDbObjectGeneralView? (Or an AMDbObjectGeneralView itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsAMDbObjectGeneralView;
	do {
		inheritsAMDbObjectGeneralView = (superClass->className() == QString("AMDbObjectGeneralView"));
	}
	while( (superClass=superClass->superClass()) && inheritsAMDbObjectGeneralView == false );
	return inheritsAMDbObjectGeneralView;
}

namespace AMDbObjectGeneralViewSupport{
	QHash<QString, AMDbObjectGeneralViewObjectInfo> registeredClasses_;

	const QHash<QString, AMDbObjectGeneralViewObjectInfo>* registeredClasses(){
		return &registeredClasses_;
	}

	AMDbObjectGeneralView* createGeneralView(const AMDbObject *object){
		if(!object)
			return 0;

		AMDbObjectGeneralViewObjectInfo infoRequested = registeredClasses_.value(object->metaObject()->className());
		if(infoRequested.objectMetaObject){
			AMDbObjectGeneralView *generalView = qobject_cast<AMDbObjectGeneralView*>(infoRequested.generalViewMetaObject->newInstance());
			if(generalView){
				generalView->createView(object);
				return generalView;
			}
		}
		return 0;
	}

	AMDbObjectGeneralView* closestMatchingGeneralView(const AMDbObject *object){
		if(!object)
			return 0;
		const QMetaObject *superClass = object->metaObject();
		QString superClassName;
		do{
			superClassName = superClass->className();
			AMDbObjectGeneralViewObjectInfo infoRequested = registeredClasses_.value(superClassName);
			if(infoRequested.objectMetaObject){
				AMDbObjectGeneralView *generalView = qobject_cast<AMDbObjectGeneralView*>(infoRequested.generalViewMetaObject->newInstance());
				if(generalView){
					generalView->createView(object);
					return generalView;
				}
			}
			superClass = superClass->superClass();
		}while(superClass && superClassName != "QObject");
		return 0;
	}

	AMDbObjectGeneralView* closestMatchingGeneralView(const QString &databaseName, const QString &tableName, int dbId){
		AMDatabase *database = AMDatabase::database(databaseName);
		QSqlQuery q = database->select(QString("AMDbObjectTypes_table"), QString("inheritance"), QString("tableName='%1'").arg(tableName));
		bool dbQueryFailed = false;
		if(!database->execQuery(q))
			dbQueryFailed = true;
		if(!dbQueryFailed && !q.next())
			dbQueryFailed = true;
		if(dbQueryFailed){
			q.finish();
			return 0;
		}
		QString inheritanceTreeString = q.value(0).toString();
		q.finish();
		QStringList inheritanceTreeList = inheritanceTreeString.split(';');
		for(int x = 0; x < inheritanceTreeList.count(); x++){
			AMDbObjectGeneralViewObjectInfo infoRequested = registeredClasses_.value(inheritanceTreeList.at(x));
			if(infoRequested.objectMetaObject){
				AMDbObjectGeneralView *generalView = qobject_cast<AMDbObjectGeneralView*>(infoRequested.generalViewMetaObject->newInstance());
				if(generalView && generalView->canView(databaseName, tableName, dbId)){
					generalView->createView(databaseName, tableName, dbId);
					return generalView;
				}
			}
		}
		return 0;
	}
}
