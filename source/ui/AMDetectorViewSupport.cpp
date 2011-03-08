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

bool AMDetectorViewObjectInfo::inheritsDetectorView(const QMetaObject *metaObject){
	// is this a subclass of AMDetectorView? (Or an AMDetectorView itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsDetectorView;
	do {
		inheritsDetectorView = (superClass->className() == QString("AMDetectorView"));
	}
	while( (superClass=superClass->superClass()) && inheritsDetectorView == false );
	return inheritsDetectorView;
}

bool AMDetectorViewObjectInfo::inheritsDetectorInfo(const QMetaObject *metaObject){
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
		while (i != registeredClasses_.constEnd()) {
			if(i.value().detectorClassName == detector->getMetaObject()->className())
				rv.append(i.value());
			++i;
		}
		return rv;
	}

	AMDetectorView* createDetectorView(AMDetector *detector){
		if(!detector)
			return 0;
		//qDebug() << "Trying to create view for detector named " << detector->detectorName();
		QList<AMDetectorViewObjectInfo> supported = supportedClasses(detector);
		if(supported.count() > 0){
			AMDetectorView *dv = qobject_cast<AMDetectorView*>(supported.first().viewMetaObject->newInstance());
			if(dv && dv->setDetector(detector))
				return dv;
		}
		return 0;
	}
}

