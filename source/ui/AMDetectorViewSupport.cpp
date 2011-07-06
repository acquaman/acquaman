/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMDetectorViewSupport.h"

#include "ui/AMDetectorView.h"

AMDetectorViewObjectInfo::AMDetectorViewObjectInfo(AMDetectorView *prototypeView, AMDetector *prototypeDetector){
	initWithMetaObject(prototypeView->metaObject(), prototypeDetector->getMetaObject());
}

AMDetectorViewObjectInfo::AMDetectorViewObjectInfo(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject){
	if(inheritsDetectorView(classMetaObject) && inheritsDetectorInfo(supportsMetaObject) )
		initWithMetaObject(classMetaObject, supportsMetaObject);
	else
		viewMetaObject = 0;
}

void AMDetectorViewObjectInfo::initWithMetaObject(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject) {
	viewMetaObject = classMetaObject;
	detectorMetaObject = supportsMetaObject;
	viewClassName = viewMetaObject->className();
	detectorClassName = detectorMetaObject->className();
}

bool AMDetectorViewObjectInfo::inheritsDetectorView(const QMetaObject *metaObject) const{
	// is this a subclass of AMDetectorView? (Or an AMDetectorView itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsDetectorView;
	do {
		inheritsDetectorView = (superClass->className() == QString("AMDetectorView"));
	}
	while( (superClass=superClass->superClass()) && inheritsDetectorView == false );
	return inheritsDetectorView;
}

bool AMDetectorViewObjectInfo::inheritsBriefView(const QMetaObject *metaObject) const{
	// is this a subclass of AMBriefDetectorView? (Or an AMBriefDetectorView itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsBriefView;
	do {
		inheritsBriefView = (superClass->className() == QString("AMBriefDetectorView"));
	}
	while( (superClass=superClass->superClass()) && inheritsBriefView == false );
	return inheritsBriefView;
}

bool AMDetectorViewObjectInfo::inheritsDetailedView(const QMetaObject *metaObject) const{
	// is this a subclass of AMBriefDetectorView? (Or an AMBriefDetectorView itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsDetailedView;
	do {
		inheritsDetailedView = (superClass->className() == QString("AMDetailedDetectorView"));
	}
	while( (superClass=superClass->superClass()) && inheritsDetailedView == false );
	return inheritsDetailedView;
}


bool AMDetectorViewObjectInfo::inheritsDetectorInfo(const QMetaObject *metaObject) const{
	// is this a subclass of AMDetectorInfo? (Or an AMDetectorInfo itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsDetectorInfo;
	do {
		inheritsDetectorInfo = (superClass->className() == QString("AMDetectorInfo"));
	}
	while( (superClass=superClass->superClass()) && inheritsDetectorInfo == false );
	return inheritsDetectorInfo;
}

namespace AMDetectorViewSupport{
	QHash<QString, AMDetectorViewObjectInfo> registeredClasses_;

	const QHash<QString, AMDetectorViewObjectInfo>* registeredClasses() {
		return &registeredClasses_;
	}

	const QList<AMDetectorViewObjectInfo> supportedClasses(AMDetector *detector){
		QHash<QString, AMDetectorViewObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		QList<AMDetectorViewObjectInfo> rv;
		if(detector){
			while (i != registeredClasses_.constEnd()) {
				if(i.value().detectorClassName == detector->getMetaObject()->className())
					rv.append(i.value());
				++i;
			}
		}
		return rv;
	}

	const QList<AMDetectorViewObjectInfo> supportedBriefViews(AMDetector *detector){
		QHash<QString, AMDetectorViewObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		QList<AMDetectorViewObjectInfo> rv;
		if(detector){
			while (i != registeredClasses_.constEnd()) {
				if( (i.value().detectorClassName == detector->getMetaObject()->className()) && ( i.value().isBriefView() ) )
					rv.append(i.value());
				++i;
			}
		}
		return rv;
	}

	const QList<AMDetectorViewObjectInfo> supportedDetailedViews(AMDetector *detector){
		QHash<QString, AMDetectorViewObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		QList<AMDetectorViewObjectInfo> rv;
		if(detector){
			while (i != registeredClasses_.constEnd()) {
				if( (i.value().detectorClassName == detector->getMetaObject()->className()) && ( i.value().isDetailedView() ) )
					rv.append(i.value());
				++i;
			}
		}
		return rv;
	}

	AMDetectorView* createDetectorView(AMDetector *detector, bool configureOnly){
		if(!detector)
			return 0;
		//qDebug() << "Trying to create view for detector named " << detector->detectorName();
		QList<AMDetectorViewObjectInfo> supported = supportedClasses(detector);
		if(supported.count() > 0){
			AMDetectorView *dv = qobject_cast<AMDetectorView*>(supported.first().viewMetaObject->newInstance());
			if(dv && dv->setDetector(detector, configureOnly))
				return dv;
		}
		return 0;
	}

	AMDetectorView* createBriefDetectorView(AMDetector *detector, bool configureOnly){
		if(!detector)
			return 0;
		//qDebug() << "Trying to create view for detector named " << detector->detectorName();
		QList<AMDetectorViewObjectInfo> supported = supportedBriefViews(detector);
		if(supported.count() > 0){
			AMDetectorView *dv = qobject_cast<AMDetectorView*>(supported.first().viewMetaObject->newInstance());
			if(dv && dv->setDetector(detector, configureOnly))
				return dv;
		}
		return 0;
	}

	AMDetectorView* createDetailedDetectorView(AMDetector *detector, bool configureOnly){
		if(!detector)
			return 0;
		//qDebug() << "Trying to create view for detector named " << detector->detectorName();
		QList<AMDetectorViewObjectInfo> supported = supportedDetailedViews(detector);
		if(supported.count() > 0){
			AMDetectorView *dv = qobject_cast<AMDetectorView*>(supported.first().viewMetaObject->newInstance());
			if(dv && dv->setDetector(detector, configureOnly))
				return dv;
		}
		return 0;
	}
}

