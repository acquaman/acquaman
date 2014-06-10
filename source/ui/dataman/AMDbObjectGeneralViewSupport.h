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


#ifndef AMDBOBJECTGENERALVIEWSUPPORT_H
#define AMDBOBJECTGENERALVIEWSUPPORT_H

#include <QString>
#include <QHash>

//class QMetaObject;
class AMDbObject;
class AMDbObjectGeneralView;

class AMDbObjectGeneralViewObjectInfo{
public:
	/// The default constructor creates an invalid object
 	virtual ~AMDbObjectGeneralViewObjectInfo();
	AMDbObjectGeneralViewObjectInfo(){
		objectMetaObject = 0;
		generalViewMetaObject = 0;
	}

	/// fill the className
	AMDbObjectGeneralViewObjectInfo(AMDbObject *prototypeObject, AMDbObjectGeneralView *prototypeGeneralView);

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMDbObjectGeneralViewObjectInfo(const QMetaObject *useObjectMetaObject, const QMetaObject *useGeneralViewMetaObject);


	/// Indicates this AMDbObjectGeneralViewInfo represents a valid object.
	bool isValid() const {
		return ( (objectMetaObject != 0) && (generalViewMetaObject != 0) );
	}

	QString objectClassName; ///< the class name (C++ type name) of the AMDbObject sub-type object
	QString generalViewClassName; ///< the class name (C++ type name) of the general view class associated to it
	const QMetaObject *objectMetaObject; ///< QMetaObject pointer with the complete meta-object for the object
	const QMetaObject *generalViewMetaObject; ///< QMetaObject pointer with the complete meta-object for the general view

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject *useObjectMetaObject, const QMetaObject *useGeneralViewMetaObject);

	/// checks to make sure a QMetaObject inherits AMScanConfiguration
	bool inheritsAMDbObject(const QMetaObject *metaObject) const;

	/// checks to make sure a QMetaObject inherits AMExporter
	bool inheritsAMDbObjectGeneralView(const QMetaObject *metaObject) const;
};

namespace AMDbObjectGeneralViewSupport{
	extern QHash<QString, AMDbObjectGeneralViewObjectInfo> registeredClasses_;

	template <class Ta, class Tb>
	bool registerClass(){
		// create the meta object for the object
		const QMetaObject *objectMo = &(Ta::staticMetaObject);
		// create the meta object for the general view
		const QMetaObject *generalViewMo = &(Tb::staticMetaObject);

		// is this a subclass of AMDbObject? (Or an AMDbObject itself?)
		const QMetaObject* objectSuperClass = objectMo;
		bool inheritsAMDbObject;
		do {
			inheritsAMDbObject = (objectSuperClass->className() == QString("AMDbObject"));
		}
		while( (objectSuperClass=objectSuperClass->superClass()) && inheritsAMDbObject == false );
		if(!inheritsAMDbObject)
			return false;

		// is this a subclass of AMDbObjectGeneralView? (Or an AMDbObjectGeneralView itself?)
		const QMetaObject* generalViewSuperClass = generalViewMo;
		bool inheritsAMDbObjectGeneralView;
		do {
			inheritsAMDbObjectGeneralView = (generalViewSuperClass->className() == QString("AMDbObjectGeneralView"));
		}
		while( (generalViewSuperClass=generalViewSuperClass->superClass()) && inheritsAMDbObjectGeneralView == false );
		if(!inheritsAMDbObjectGeneralView)
			return false;

		// is it already registered? return true.
		QString className(objectMo->className());
		if(registeredClasses_.contains(className)) {
			//AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("Detector View Support: The class '%1' has already been registered with detector view. Skipping duplicate registration.").arg(className)));
			return true;
		}

		AMDbObjectGeneralViewObjectInfo newInfo(objectMo, generalViewMo);

		registeredClasses_.insert(className, newInfo);
		return true;
	}

	/// returns a const pointer to the hash of registered AMDbObjectGeneralViewObjectInfo classes
	const QHash<QString, AMDbObjectGeneralViewObjectInfo>* registeredClasses();

	/// This creates the general view associated with type of AMDbObject. Returns 0 if no associated view is available.
	AMDbObjectGeneralView* createGeneralView(const AMDbObject *object);

	/// This creates the best matching view based on this AMDbObject's inheritance tree. Returns the base AMDbObjectGeneralView if no other is available.
	AMDbObjectGeneralView* closestMatchingGeneralView(const AMDbObject *object);

	/// This creates the best matching view based on information about this table stored in the database inheritance tree. Returns the base AMDbObjectGeneralView if no other is available.
	AMDbObjectGeneralView* closestMatchingGeneralView(const QString &databaseName, const QString &tableName, int dbId);
}

#endif // AMDBOBJECTGENERALVIEWSUPPORT_H
