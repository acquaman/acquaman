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


#ifndef AMOLDDETECTORVIEWSUPPORT_H
#define AMOLDDETECTORVIEWSUPPORT_H

#include "beamline/AMOldDetector.h"

class AMOldDetectorView;

class AMOldDetectorViewObjectInfo{
public:
	/// The default constructor creates an invalid AMDbObject.
 	virtual ~AMOldDetectorViewObjectInfo();
	AMOldDetectorViewObjectInfo() {
		viewMetaObject = 0;
	}

	/// fill the className
	AMOldDetectorViewObjectInfo(AMOldDetectorView *prototypeView, AMOldDetector *prototypeDetector);

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMOldDetectorViewObjectInfo(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject);


	/// Indicates this AMDbObjectInfo represents a valid object.
	bool isValid() const { return (viewMetaObject != 0); }

	bool isBriefView() const {
		if(isValid())
			return inheritsOldBriefView(viewMetaObject);
		return false;
	}

	bool isDetailedView() const{
		if(isValid())
			return inheritsOldDetailedView(viewMetaObject);
		return false;
	}

	QString viewClassName;	///< the class name (C++ type name) of the view objects
	QString detectorClassName; ///< the class name (C++ type name) of the detector objects
	const QMetaObject* viewMetaObject;	///< QMetaObject pointer with the complete class meta-information. The remaining values can be determined by parsing the metaObject's classInfo parameters, but are stored here for efficiency.
	const QMetaObject* detectorMetaObject; ///< QMetaObject pointer with complete class meta-information for the detector this view supports

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject);

	/// checks to make sure a QMetaObject inherits AMOldDetectorView
	bool inheritsOldDetectorView(const QMetaObject *metaObject) const;

	/// checks to make sure a QMetaObject inherits AMBriefDetectorView
	bool inheritsOldBriefView(const QMetaObject *metaObject) const;

	/// checks to make sure a QMetaObject inherits AMDetailedDetectorView
	bool inheritsOldDetailedView(const QMetaObject *metaObject) const;

	/// checks to make sure a QMetaObject inherits AMOldDetectorInfo
	bool inheritsOldDetectorInfo(const QMetaObject *metaObject) const;
};

namespace AMOldDetectorViewSupport{
	extern QHash<QString, AMOldDetectorViewObjectInfo> registeredClasses_;

	// Registers the pairing of a view class (AMOldDetectorView descendant) and a detector class (AMOldDetectorInfo descendent)
	// Class T1 needs to inherit AMOldDetectorView and Class T2 needs to inherit AMOldDetectorInfo
	template <class Ta, class Tb>
			bool registerClass() {
		// create the meta object for the view
		const QMetaObject *viewMo = &(Ta::staticMetaObject);
		// create the meta object for the detector
		const QMetaObject *detectorMo = &(Tb::staticMetaObject);

		// is this a subclass of AMOldDetectorView? (Or an AMOldDetectorView itself?)
		const QMetaObject* viewSuperClass = viewMo;
		bool inheritsOldDetectorView;
		do {
			inheritsOldDetectorView = (viewSuperClass->className() == QString("AMOldDetectorView"));
		}
		while( (viewSuperClass=viewSuperClass->superClass()) && inheritsOldDetectorView == false );
		if(!inheritsOldDetectorView)
			return false;	// can't register a non AMOldDetectorView subclass.

		// is this a subclass of AMOldDetectorInfo? (Or an AMOldDetectorInfo itself?)
		const QMetaObject* detectorSuperClass = detectorMo;
		bool inheritsOldDetectorInfo;
		do {
			inheritsOldDetectorInfo = (detectorSuperClass->className() == QString("AMOldDetectorInfo"));
		}
		while( (detectorSuperClass=detectorSuperClass->superClass()) && inheritsOldDetectorInfo == false );
		if(!inheritsOldDetectorInfo)
			return false;	// can't register a non AMOldDetectorInfo subclass.

		// is it already registered? return true.
		QString className(viewMo->className());
		if(registeredClasses_.contains(className)) {
			//AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("Detector View Support: The class '%1' has already been registered with detector view. Skipping duplicate registration.").arg(className)));
			return true;
		}

		AMOldDetectorViewObjectInfo newInfo(viewMo, detectorMo);

		registeredClasses_.insert(className, newInfo);
		return true;
	}

	/// returns a const pointer to the hash of registered AMDetectorViewObjectInfo classes
	const QHash<QString, AMOldDetectorViewObjectInfo>* registeredClasses();

	/// returns a const pointer to a list of AMDetectorViewObjectInfo classes supporting the AMDetector
	const QList<AMOldDetectorViewObjectInfo> supportedClasses(AMOldDetector *detector);

	const QList<AMOldDetectorViewObjectInfo> supportedBriefViews(AMOldDetector *detector);

	const QList<AMOldDetectorViewObjectInfo> supportedDetailedViews(AMOldDetector *detector);

	/// Useful for gui generation, this creates the first view available from the supportedClasses for the AMDetector. You can use qobject_cast<>() or type() to find out the detailed type of the new object.  Returns 0 if no object found.
	/*! Ownership of the newly-created object becomes the responsibility of the caller. */
	AMOldDetectorView* createDetectorView(AMOldDetector *detector, bool configureOnly = false);

	AMOldDetectorView* createBriefDetectorView(AMOldDetector *detector, bool configureOnly = false);

	AMOldDetectorView* createDetailedDetectorView(AMOldDetector *detector, bool configureOnly = false);
}


#endif // AMDETECTORVIEWSUPPORT_H
