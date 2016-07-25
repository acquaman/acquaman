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


#include "AMControlInfoList.h"
#include <algorithm>
#include <cmath>

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

bool AMControlInfoList::operator ==(const AMControlInfoList &other) const {
	if(count() != other.count())
		return false;
	AMControlInfo tmpOther, tmpMine;
	for(int x = count()-1; x >= 0; x--){
		tmpOther = other.at(x);
		tmpMine = at(x);
		if( (tmpOther.name() != tmpMine.name()) || ( fabs(tmpOther.value() - tmpMine.value()) > std::min(tmpOther.tolerance(), tmpMine.tolerance()) ) )
			return false;
	}
	return true;
}

bool AMControlInfoList::operator !=(const AMControlInfoList &other) const{
	return !operator ==(other);
}

bool AMControlInfoList::compareWithinTolerance(const AMControlInfoList &other, QList<double> tolerances) const {
	if( (count() != other.count()) || (count() != tolerances.count()) )
		return false;
	AMControlInfo tmpOther, tmpMine;
	double tmpTolerance;
	for(int x = count()-1; x >= 0; x--){
		tmpOther = other.at(x);
		tmpMine = at(x);
		tmpTolerance = tolerances.at(x);
		if( (tmpOther.name() != tmpMine.name()) || ( fabs(tmpOther.value() - tmpMine.value()) > tmpTolerance ) )
			return false;
	}
	return true;
}

QDebug operator<<(QDebug d, const AMControlInfoList& cil){
	d << "(";
	for(int x = 0; x < cil.count(); x++)
		d << "[" << cil.at(x).name() << "=" << cil.at(x).value() << "]";
	d << ")";
	return d;
}

// Returns a list of pointers to the AMControlInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
AMDbObjectList AMControlInfoList::dbReadControlInfos() {
	AMDbObjectList rv;
	for(int i=0; i<count(); i++)
		rv << &((*this)[i]);
	return rv;
}

// Called by the database system on loadFromDb() to give us our new set of AMControlInfo objects. We copy these ones into our internal list and then delete them.
void AMControlInfoList::dbLoadControlInfos(const AMDbObjectList& newControlInfos) {
	clear();	// get rid of our existing

	for(int i=0; i<newControlInfos.count(); i++) {
		AMControlInfo* newControlInfo = qobject_cast<AMControlInfo*>(newControlInfos.at(i));
		if(newControlInfo) {
			append(*newControlInfo);	// note: makes a copy of object pointed to by newControlInfo, and stores in our internal list.
		}

		newControlInfos.at(i)->deleteLater();	// we're copying these. Don't need to keep these ones around. They're our responsibility to delete.
	}
}

void AMControlInfoList::setValuesFrom(const AMControlInfoList &other)
{
	int otherCount = other.count();

	// remove any extras we have.
	/// \todo \bug Watch out... these are being orphaned in the database.
	while(count() > otherCount)
		remove(count()-1);

	// loop through and modify the ones we have in common. In the case where us and other have the same size, this will be all we do.  No new rows will be needed in the database.
	for(int i=0; i<count(); i++) {
		(*this)[i].setValuesFrom(other.at(i));
	}

	// add any extras
	for(int i=count(); i<otherCount; i++) {
		AMControlInfo newCI;
		newCI.setValuesFrom(other.at(i));
		append(newCI);
	}

	setModified(true);
}

bool AMControlInfoList::hasControl(const QString &controlName) const
{
	return (indexOf(controlName) != -1);
}

int AMControlInfoList::indexOf(const QString &controlName) const
{
	for(int i=count()-1; i>= 0; i--) {
		if(at(i).name() == controlName)
			return i;
	}
	return -1;
}




