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


#include "AMDetectorInfo.h"

#include <QStringList>

AMDetectorInfo::AMDetectorInfo(const QString& name, const QString& description, QObject *parent) :
		AMDbObject(parent)
{
	setName(name);
	description_ = description;
}

AMDetectorInfo::AMDetectorInfo(const AMDetectorInfo &original) :
		AMDbObject(original)
{
	//retreiveAndSetProperties(original);
	this->operator =(original);
}

AMDetectorInfo& AMDetectorInfo::operator =(const AMDetectorInfo &other)
{
	if(this != &other){
		AMDbObject::operator =(other);
		description_ = other.description();
		setUnits(other.units());
	}
	return *this;
}

AMDetectorInfo* AMDetectorInfo::toNewInfo() const{
	return new AMDetectorInfo(*this);
}

QDebug operator<<(QDebug d, const AMDetectorInfo& di){
	return di.qDebugPrint(d);
}

QDebug AMDetectorInfo::qDebugPrint(QDebug &d) const{
	d << "AMDetectorInfo default";
	return d;
}

/*
void AMDetectorInfo::retreiveAndSetProperties(const AMDetectorInfo &original){
	const QMetaObject *metaobject = original.metaObject();
	int count = metaobject->propertyCount();

	for (int i=0; i<count; ++i) {
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = original.property(name);
		bool isWritable = original.metaObject()->property((original.metaObject()->indexOfProperty(name))).isWritable();
		if(isWritable)
			this->setProperty(name, value);
	}
}
*/
