/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
