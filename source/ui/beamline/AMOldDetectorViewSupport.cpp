/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMOldDetectorViewSupport.h"

#include "ui/beamline/AMOldDetectorView.h"

 AMOldDetectorViewObjectInfo::~AMOldDetectorViewObjectInfo(){}
AMOldDetectorViewObjectInfo::AMOldDetectorViewObjectInfo(AMOldDetectorView *prototypeView, AMOldDetector *prototypeDetector){
	initWithMetaObject(prototypeView->metaObject(), prototypeDetector->getMetaObject());
}

AMOldDetectorViewObjectInfo::AMOldDetectorViewObjectInfo(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject){
	if(inheritsOldDetectorView(classMetaObject) && inheritsOldDetectorInfo(supportsMetaObject) )
		initWithMetaObject(classMetaObject, supportsMetaObject);
	else
		viewMetaObject = 0;
}

void AMOldDetectorViewObjectInfo::initWithMetaObject(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject) {
	viewMetaObject = classMetaObject;
	detectorMetaObject = supportsMetaObject;
	viewClassName = viewMetaObject->className();
	detectorClassName = detectorMetaObject->className();
}

bool AMOldDetectorViewObjectInfo::inheritsOldDetectorView(const QMetaObject *metaObject) const{
	// is this a subclass of AMOldDetectorView? (Or an AMOldDetectorView itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsOldDetectorView;
	do {
		inheritsOldDetectorView = (superClass->className() == QString("AMOldDetectorView"));
	}
	while( (superClass=superClass->superClass()) && inheritsOldDetectorView == false );
	return inheritsOldDetectorView;
}

bool AMOldDetectorViewObjectInfo::inheritsOldBriefView(const QMetaObject *metaObject) const{
	// is this a subclass of AMBriefOldDetectorView? (Or an AMBriefOldDetectorView itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsOldBriefView;
	do {
		inheritsOldBriefView = (superClass->className() == QString("AMBriefOldDetectorView"));
	}
	while( (superClass=superClass->superClass()) && inheritsOldBriefView == false );
	return inheritsOldBriefView;
}

bool AMOldDetectorViewObjectInfo::inheritsOldDetailedView(const QMetaObject *metaObject) const{
	// is this a subclass of AMDetailedOldDetectorView? (Or an AMDetailedOldDetectorView itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsOldDetailedView;
	do {
		inheritsOldDetailedView = (superClass->className() == QString("AMDetailedOldDetectorView"));
	}
	while( (superClass=superClass->superClass()) && inheritsOldDetailedView == false );
	return inheritsOldDetailedView;
}


bool AMOldDetectorViewObjectInfo::inheritsOldDetectorInfo(const QMetaObject *metaObject) const{
	// is this a subclass of AMOldDetectorInfo? (Or an AMOldDetectorInfo itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsOldDetectorInfo;
	do {
		inheritsOldDetectorInfo = (superClass->className() == QString("AMOldDetectorInfo"));
	}
	while( (superClass=superClass->superClass()) && inheritsOldDetectorInfo == false );
	return inheritsOldDetectorInfo;
}

namespace AMOldDetectorViewSupport{
	QHash<QString, AMOldDetectorViewObjectInfo> registeredClasses_;

	const QHash<QString, AMOldDetectorViewObjectInfo>* registeredClasses() {
		return &registeredClasses_;
	}

	const QList<AMOldDetectorViewObjectInfo> supportedClasses(AMOldDetector *detector){
		QHash<QString, AMOldDetectorViewObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		QList<AMOldDetectorViewObjectInfo> rv;
		if(detector){
			while (i != registeredClasses_.constEnd()) {
				if(i.value().detectorClassName == detector->getMetaObject()->className())
					rv.append(i.value());
				++i;
			}
		}
		return rv;
	}

	const QList<AMOldDetectorViewObjectInfo> supportedBriefViews(AMOldDetector *detector){
		QHash<QString, AMOldDetectorViewObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		QList<AMOldDetectorViewObjectInfo> rv;
		if(detector){
			while (i != registeredClasses_.constEnd()) {
				if( (i.value().detectorClassName == detector->getMetaObject()->className()) && ( i.value().isBriefView() ) )
					rv.append(i.value());
				++i;
			}
		}
		return rv;
	}

	const QList<AMOldDetectorViewObjectInfo> supportedDetailedViews(AMOldDetector *detector){
		QHash<QString, AMOldDetectorViewObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		QList<AMOldDetectorViewObjectInfo> rv;
		if(detector){
			while (i != registeredClasses_.constEnd()) {
				if( (i.value().detectorClassName == detector->getMetaObject()->className()) && ( i.value().isDetailedView() ) )
					rv.append(i.value());
				++i;
			}
		}
		return rv;
	}

	AMOldDetectorView* createDetectorView(AMOldDetector *detector, bool configureOnly){
		if(!detector)
			return 0;
		//qdebug() << "Trying to create view for detector named " << detector->detectorName();
		QList<AMOldDetectorViewObjectInfo> supported = supportedClasses(detector);
		if(supported.count() > 0){
			AMOldDetectorView *dv = qobject_cast<AMOldDetectorView*>(supported.first().viewMetaObject->newInstance());
			if(dv && dv->setDetector(detector, configureOnly))
				return dv;
		}
		return 0;
	}

	AMOldDetectorView* createBriefDetectorView(AMOldDetector *detector, bool configureOnly){
		if(!detector)
			return 0;
		//qdebug() << "Trying to create view for detector named " << detector->detectorName();
		QList<AMOldDetectorViewObjectInfo> supported = supportedBriefViews(detector);
		if(supported.count() > 0){
			AMOldDetectorView *dv = qobject_cast<AMOldDetectorView*>(supported.first().viewMetaObject->newInstance());
			if(dv && dv->setDetector(detector, configureOnly))
				return dv;
		}
		return 0;
	}

	AMOldDetectorView* createDetailedDetectorView(AMOldDetector *detector, bool configureOnly){
		if(!detector)
			return 0;
		//qdebug() << "Trying to create view for detector named " << detector->detectorName();
		QList<AMOldDetectorViewObjectInfo> supported = supportedDetailedViews(detector);
		if(supported.count() > 0){
			AMOldDetectorView *dv = qobject_cast<AMOldDetectorView*>(supported.first().viewMetaObject->newInstance());
			if(dv && dv->setDetector(detector, configureOnly))
				return dv;
		}
		return 0;
	}
}

