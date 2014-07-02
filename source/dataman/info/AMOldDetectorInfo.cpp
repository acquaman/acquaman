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


#include "AMOldDetectorInfo.h"

#include <QStringList>

AMOldDetectorInfo::AMOldDetectorInfo(const QString& name, const QString& description, QObject *parent) :
		AMDbObject(parent)
{
	setName(name);
	description_ = description;
}

AMOldDetectorInfo::AMOldDetectorInfo(const AMOldDetectorInfo &original) :
		AMDbObject(original)
{
	//retreiveAndSetProperties(original);
	this->operator =(original);
}

AMOldDetectorInfo& AMOldDetectorInfo::operator =(const AMOldDetectorInfo &other)
{
	if(this != &other){
		AMDbObject::operator =(other);
		description_ = other.description();
		setUnits(other.units());
	}
	return *this;
}

AMOldDetectorInfo* AMOldDetectorInfo::toNewInfo() const{
	return new AMOldDetectorInfo(*this);
}

QDebug operator<<(QDebug d, const AMOldDetectorInfo& di){
	return di.qDebugPrint(d);
}

QDebug AMOldDetectorInfo::qDebugPrint(QDebug &d) const{
	d << "AMDetectorInfo " << name();
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
