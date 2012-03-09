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


#include "AMDetectorInfoSet.h"

AMDetectorInfoSet::AMDetectorInfoSet(QObject *parent)
	: AMDbObject(parent), AMOrderedSet<QString, QPair<AMDetectorInfo*, bool> >()
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));
}

AMDetectorInfoSet::AMDetectorInfoSet(const AMDetectorInfoSet& other)
	: AMDbObject(other), AMOrderedSet<QString, QPair<AMDetectorInfo*, bool> >(other)
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));
}

AMDetectorInfoSet::AMDetectorInfoSet(AMDatabase* db, int id)
	: AMDbObject(), AMOrderedSet<QString, QPair<AMDetectorInfo*, bool> >() {

	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));

	loadFromDb(db, id);
}

AMDetectorInfoSet& AMDetectorInfoSet::operator=(const AMDetectorInfoSet& other) {
	// always: check for self-assignment
	if(this != &other) {
		this->clear();
		for(int x = 0; x < other.count(); x++)
			addDetectorInfo(other.detectorInfoAt(x)->toNewInfo(), other.isActiveAt(x));
		AMDbObject::operator=(other);

		// if the items in other have been modified, but the signal hasn't been emitted yet, other.modified_ will not be set to true yet. We know that things have changed, so ours should be true.
		if(other.signalSource()->delayedItemChangedScheduled())
			setModified(true);
	}

	return *this;
}

QDebug operator<<(QDebug d, const AMDetectorInfoSet& dis){
	for(int x = 0; x < dis.count(); x++){
		if(dis.isActiveAt(x))
			d << "Is Active ";
		else
			d << "Is Not Active ";
		d << *(dis.detectorInfoAt(x)) << "\n";
	}
	return d;
}

QString AMDetectorInfoSet::description(){
	return description_;
}

/* NTBA March 14, 2011 David Chevrier
   Still not sure about these
*/
// Returns a list of pointers to the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
AMDbObjectList AMDetectorInfoSet::dbReadDetectorInfos() {
	AMDbObjectList rv;
	for(int x = 0; x < count(); x++){
		qDebug() << "dbReadDetectorInfos as " << at(x).first->name();
		rv << at(x).first;
	}
	return rv;
}

/* NTBA March 14, 2011 David Chevrier
   Still not sure about these
*/
// Called by the database system on loadFromDb() to give us our new set of AMDetectorInfo objects. We copy these ones into our internal list and then delete them.
void AMDetectorInfoSet::dbLoadDetectorInfos(const AMDbObjectList& newDetectorInfos) {
	clear();	// get rid of our existing

	for(int i=0; i<newDetectorInfos.count(); i++) {
		AMDetectorInfo* newDetectorInfo = qobject_cast<AMDetectorInfo*>(newDetectorInfos.at(i));
		if(newDetectorInfo)
			addDetectorInfo(newDetectorInfo);
	}
}

int AMDetectorInfoSet::indexOf(AMDetectorInfo *detectorInfo) const{
	return indexOfValue(detectorInfo);
}

int AMDetectorInfoSet::indexOf(const QString& detectorName) const{
	return indexOfKey(detectorName);
}

AMDetectorInfo* AMDetectorInfoSet::detectorInfoNamed(const QString& detectorName){
	int index = indexOfKey(detectorName);
	if(index < 0)
		return 0; // NULL
	return at(index).first;
}

const AMDetectorInfo* AMDetectorInfoSet::detectorInfoNamed(const QString &detectorName) const{
	int index = indexOfKey(detectorName);
	if(index < 0)
		return 0; // NULL
	return at(index).first;
}

AMDetectorInfo* AMDetectorInfoSet::detectorInfoAt(int index){
	if(index < 0 || index >= count())
		return 0; //NULL
	return at(index).first;
}

AMDetectorInfo* AMDetectorInfoSet::detectorInfoAt(int index) const{
	if(index < 0 || index >= count())
		return 0; //NULL
	return at(index).first->toNewInfo();
}

bool AMDetectorInfoSet::isActiveNamed(const QString& detectorName) const{
	int index = indexOf(detectorName);
	if(index > 0)
		return at(index).second;
	return false;
}

bool AMDetectorInfoSet::isActiveDetectorInfo(AMDetectorInfo *detectorInfo) const{
	int index = indexOf(detectorInfo);
	if(index > 0)
		return at(index).second;
	return false;
}

bool AMDetectorInfoSet::isActiveAt(int index) const{
	if(index < 0 || index >= count())
		return false;
	return at(index).second;
}

bool AMDetectorInfoSet::addDetectorInfo(AMDetectorInfo *newDetectorInfo, bool isActive){
	if(!newDetectorInfo)
		return false;

	return append(QPair<AMDetectorInfo*, bool>(newDetectorInfo, isActive), newDetectorInfo->name());
}

bool AMDetectorInfoSet::removeDetector(AMDetectorInfo *detectorInfo){
	int index = indexOf(detectorInfo);
	if(index < 0)
		return false;

	remove(index);
	return true;
}

bool AMDetectorInfoSet::setActiveNamed(const QString& detectorName, bool active){
	int index = indexOf(detectorName);
	if(index < 0)
		return false;
	((*this)[index]).second = active;
	return true;
}

bool AMDetectorInfoSet::setActiveDetectorInfo(AMDetectorInfo *detectorInfo, bool active){
	int index = indexOf(detectorInfo);
	if(index < 0)
		return false;
	((*this)[index]).second = active;
	return true;
}

bool AMDetectorInfoSet::setActiveAt(int index, bool active){
	if(index < 0 || index > count())
		return false;
	((*this)[index]).second = active;
	return true;
}

void AMDetectorInfoSet::setDescription(const QString &description){
	description_ = description;
	setModified(true);
}

void AMDetectorInfoSet::onDetectorValuesChanged(int index) {
	setModified(true);
	emit detectorValuesChanged(index);
}

void AMDetectorInfoSet::onDetectorAdded(int index) {
	setModified(true);
	emit detectorAdded(index);
}

void AMDetectorInfoSet::onDetectorRemoved(int index) {
	setModified(true);
	emit detectorRemoved(index);
}

int AMDetectorInfoSet::indexOfValue(const AMDetectorInfo *detectorInfo) const{
	int count = values_.count();
	for(int i=0; i<count; i++) {
		if(values_.at(i).first->name() == detectorInfo->name())
			return i;
	}
	return -1;
}
