#include "acquaman.h"

#include "dataman/AMDbObject.h"

namespace AM {
	/// Register acquaman-defined types with the QMetaType system. Call on application startup before using these types.
	void registerTypes() {
		qRegisterMetaType<AMIntList>();
		qRegisterMetaType<AMDoubleList>();
		qRegisterMetaType<AMDbObjectList>();
		qRegisterMetaType<AMDbObject*>();
	}
	// test section
	/*
	AMIntList b;
	b << 3 << 6 << 8;

	AMDoubleList dl;
	dl << 3.0 << 6.0 << 8.0;

	QVariant vb = QVariant::fromValue(b);
	qDebug() << "type of AMIntList:" << vb.type() << "userType" << vb.userType() << "QMetaType type():" << QMetaType::type("AMIntList") << "and" << qMetaTypeId<AMIntList>();
	qDebug() << "Can convert? " << vb.canConvert<AMIntList>() << vb.canConvert<AMDoubleList>();

	QVariant vdl = QVariant::fromValue(dl);
	qDebug() << "type of AMDoubleList:" << vdl.type() << "userType" << vdl.userType() << "QMetaType type():" << QMetaType::type("AMDoubleList") << "and" << qMetaTypeId<AMDoubleList>();

	qDebug() << "Can convert? " << vdl.canConvert<AMDoubleList>() << vdl.canConvert<AMIntList>();
	*/
	/////////////
}
