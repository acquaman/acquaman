#ifndef AMDETECTORVIEWSUPPORT_H
#define AMDETECTORVIEWSUPPORT_H

#include "beamline/AMDetector.h"

class AMDetectorView;

class AMDetectorViewObjectInfo{
public:
	/// The default constructor creates an invalid AMDbObject.
	AMDetectorViewObjectInfo() {
		viewMetaObject = 0;
	}

	/// fill the className
	AMDetectorViewObjectInfo(AMDetectorView *prototypeView, AMDetector *prototypeDetector);

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMDetectorViewObjectInfo(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject);


	/// Indicates this AMDbObjectInfo represents a valid object.
	bool isValid() const { return (viewMetaObject != 0); }

	QString viewClassName;	///< the class name (C++ type name) of the view objects
	QString detectorClassName; ///< the class name (C++ type name) of the detector objects
	const QMetaObject* viewMetaObject;	///< QMetaObject pointer with the complete class meta-information. The remaining values can be determined by parsing the metaObject's classInfo parameters, but are stored here for efficiency.
	const QMetaObject* detectorMetaObject; ///< QMetaObject pointer with complete class meta-information for the detector this view supports

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject);

	/// checks to make sure a QMetaObject inherits AMDetectorView
	bool inheritsDetectorView(const QMetaObject *metaObject);

	/// checks to make sure a QMetaObject inherits AMDetectorInfo
	bool inheritsDetectorInfo(const QMetaObject *metaObject);
};

namespace AMDetectorViewSupport{
	extern QHash<QString, AMDetectorViewObjectInfo> registeredClasses_;

	// Registers the pairing of a view class (AMDetectorView descendant) and a detector class (AMDetectorInfo descendent)
	// Class T1 needs to inherit AMDetectorView and Class T2 needs to inherit AMDetectorInfo
	template <class Ta, class Tb>
			bool registerClass() {
		// create the meta object for the view
		const QMetaObject *viewMo = &(Ta::staticMetaObject);
		// create the meta object for the detector
		const QMetaObject *detectorMo = &(Tb::staticMetaObject);

		// is this a subclass of AMDetectorView? (Or an AMDetectorView itself?)
		const QMetaObject* viewSuperClass = viewMo;
		bool inheritsDetectorView;
		do {
			inheritsDetectorView = (viewSuperClass->className() == QString("AMDetectorView"));
		}
		while( (viewSuperClass=viewSuperClass->superClass()) && inheritsDetectorView == false );
		if(!inheritsDetectorView)
			return false;	// can't register a non AMDetectorView subclass.

		// is this a subclass of AMDetectorInfo? (Or an AMDetectorInfo itself?)
		const QMetaObject* detectorSuperClass = detectorMo;
		bool inheritsDetectorInfo;
		do {
			inheritsDetectorInfo = (detectorSuperClass->className() == QString("AMDetectorInfo"));
		}
		while( (detectorSuperClass=detectorSuperClass->superClass()) && inheritsDetectorInfo == false );
		if(!inheritsDetectorInfo)
			return false;	// can't register a non AMDetectorInfo subclass.

		// is it already registered? return true.
		QString className(viewMo->className());
		if(registeredClasses_.contains(className)) {
			//AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("Detector View Support: The class '%1' has already been registered with detector view. Skipping duplicate registration.").arg(className)));
			return true;
		}

		AMDetectorViewObjectInfo newInfo(viewMo, detectorMo);

		registeredClasses_.insert(className, newInfo);
		return true;
	}

	/// returns a const pointer to the hash of registered AMDetectorViewObjectInfo classes
	const QHash<QString, AMDetectorViewObjectInfo>* registeredClasses();

	/// returns a const pointer to a list of AMDetectorViewObjectInfo classes supporting the AMDetector
	const QList<AMDetectorViewObjectInfo> supportedClasses(AMDetector *detector);

	/// Useful for gui generation, this creates the first view available from the supportedClasses for the AMDetector. You can use qobject_cast<>() or type() to find out the detailed type of the new object.  Returns 0 if no object found.
	/*! Ownership of the newly-created object becomes the responsibility of the caller. */
	AMDetectorView* createDetectorView(AMDetector *detector);
}


#endif // AMDETECTORVIEWSUPPORT_H
