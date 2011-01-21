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


#include "AMControlInfoList.h"

AMControlInfo::AMControlInfo(const QString& name, double value, double minimum, double maximum, const QString& units, QObject* parent)
	: AMDbObject(parent)
{
	setName(name);
	value_ = value;
	minimum_ = minimum;
	maximum_ = maximum;
	units_ = units;
}

AMControlInfo::AMControlInfo(AMDatabase *db, int id)
	: AMDbObject() {

	setName("Invalid Control Info");
	value_ = 0;
	minimum_ = 0;
	maximum_ = 0;

	loadFromDb(db, id);
}

AMControlInfoList::AMControlInfoList(QObject *parent)
	: AMDbObject(parent), AMOrderedList<AMControlInfo>()
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onControlAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onControlRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onControlValuesChanged(int)));
}

AMControlInfoList::AMControlInfoList(const AMControlInfoList& other)
	: AMDbObject(other), AMOrderedList<AMControlInfo>(other)
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onControlAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onControlRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onControlValuesChanged(int)));
}

AMControlInfoList::AMControlInfoList(AMDatabase* db, int id)
	: AMDbObject(), AMOrderedList<AMControlInfo>() {

	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onControlAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onControlRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onControlValuesChanged(int)));

	loadFromDb(db, id);
}

AMControlInfoList& AMControlInfoList::operator=(const AMControlInfoList& other) {
	// always: check for self-assignment
	if(this != &other) {
		AMOrderedList<AMControlInfo>::operator=(other);
		AMDbObject::operator=(other);

		// if the items in other have been modified, but the signal hasn't been emitted yet, other.modified_ will not be set to true yet. We know that things have changed, so ours should be true.
		if(other.signalSource()->delayedItemChangedScheduled())
			setModified(true);
	}

	return *this;
}


//QStringList AMControlInfoList::dbReadControlNames() const {
//	QStringList rv;
//	for(int i=0; i<count(); i++)
//		rv << at(i).name();
//	return rv;
//}
//AMDoubleList AMControlInfoList::dbReadControlValues() const {
//	AMDoubleList rv;
//	for(int i=0; i<count(); i++)
//		rv << at(i).value();
//	return rv;
//}
//AMDoubleList AMControlInfoList::dbReadControlMinimums() const {
//	AMDoubleList rv;
//	for(int i=0; i<count(); i++)
//		rv << at(i).minimum();
//	return rv;
//}
//AMDoubleList AMControlInfoList::dbReadControlMaximums() const {
//	AMDoubleList rv;
//	for(int i=0; i<count(); i++)
//		rv << at(i).maximum();
//	return rv;
//}
//QStringList AMControlInfoList::dbReadControlUnits() const {
//	QStringList rv;
//	for(int i=0; i<count(); i++)
//		rv << at(i).units();
//	return rv;
//}

//void AMControlInfoList::dbLoadControlNames(const QStringList& newNames) {
//	for(int i=0; i<newNames.count(); i++) {
//		if(i<count())
//			(*this)[i].setName(newNames.at(i));
//		else
//			append( AMControlInfo(newNames.at(i),0,0,0,QString()) );
//	}
//}

//void AMControlInfoList::dbLoadControlValues(const AMDoubleList& newValues) {
//	for(int i=0; i<newValues.count(); i++) {
//		// dbLoadControlNames() will be called first, so we know that we have enough controls to store this here.
//		(*this)[i].setValue(newValues.at(i));
//	}
//}

//void AMControlInfoList::dbLoadControlMinimums(const AMDoubleList& newMinimums) {
//	for(int i=0; i<newMinimums.count(); i++) {
//		// dbLoadControlNames() will be called first, so we know that we have enough controls to store this here.
//		(*this)[i].setMinimum(newMinimums.at(i));
//	}
//}

//void AMControlInfoList::dbLoadControlMaximums(const AMDoubleList& newMaximums) {
//	for(int i=0; i<newMaximums.count(); i++) {
//		// dbLoadControlNames() will be called first, so we know that we have enough controls to store this here.
//		(*this)[i].setMaximum(newMaximums.at(i));
//	}
//}

//void AMControlInfoList::dbLoadControlUnits(const QStringList& newUnits) {
//	for(int i=0; i<newUnits.count(); i++) {
//		// dbLoadControlNames() will be called first, so we know that we have enough controls to store this here.
//		(*this)[i].setUnits(newUnits.at(i));
//	}
//}
