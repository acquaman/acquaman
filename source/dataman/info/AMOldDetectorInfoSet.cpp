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


#include "AMOldDetectorInfoSet.h"

#include "util/AMErrorMonitor.h"

AMOldDetectorInfoSet::AMOldDetectorInfoSet(QObject *parent)
	: AMDbObject(parent), AMOrderedSet<QString, QPair<AMOldDetectorInfo*, bool> >()
{
	warningsSuppressed_ = true;

	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));
}

AMOldDetectorInfoSet::AMOldDetectorInfoSet(const AMOldDetectorInfoSet& other)
	: AMDbObject(other), AMOrderedSet<QString, QPair<AMOldDetectorInfo*, bool> >(other)
{
	warningsSuppressed_ = true;

	dbLoadWarnings_ = other.dbLoadWarnings();
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));
}

AMOldDetectorInfoSet::AMOldDetectorInfoSet(AMDatabase* db, int id)
	: AMDbObject(), AMOrderedSet<QString, QPair<AMOldDetectorInfo*, bool> >()
{
	warningsSuppressed_ = true;

	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));

	loadFromDb(db, id);
}

AMOldDetectorInfoSet& AMOldDetectorInfoSet::operator=(const AMOldDetectorInfoSet& other) {
	// always: check for self-assignment
	if(this != &other) {
		this->clear();
		for(int x = 0; x < other.count(); x++)
			addDetectorInfo(other.detectorInfoAt(x)->toNewInfo(), other.isActiveAt(x));

		dbLoadWarnings_ = other.dbLoadWarnings();

		AMDbObject::operator=(other);

		// if the items in other have been modified, but the signal hasn't been emitted yet, other.modified_ will not be set to true yet. We know that things have changed, so ours should be true.
		if(other.signalSource()->delayedItemChangedScheduled())
			setModified(true);
	}

	return *this;
}

QDebug operator<<(QDebug d, const AMOldDetectorInfoSet& dis){
	for(int x = 0; x < dis.count(); x++){
		if(dis.isActiveAt(x))
			d << "Is Active ";
		else
			d << "Is Not Active ";
		d << *(dis.detectorInfoAt(x)) << "\n";
	}
	return d;
}

QString AMOldDetectorInfoSet::description(){
	return description_;
}

bool AMOldDetectorInfoSet::warningsSuppressed() const{
	return warningsSuppressed_;
}

/* NTBA March 14, 2011 David Chevrier
   Still not sure about these
*/
// Returns a list of pointers to the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
AMDbObjectList AMOldDetectorInfoSet::dbReadDetectorInfos() {
	AMDbObjectList rv;
	for(int x = 0; x < count(); x++)
		rv << at(x).first;
	return rv;
}

/* NTBA March 14, 2011 David Chevrier
   Still not sure about these
*/
// Called by the database system on loadFromDb() to give us our new set of AMDetectorInfo objects. We copy these ones into our internal list and then delete them.
void AMOldDetectorInfoSet::dbLoadDetectorInfos(const AMDbObjectList& newDetectorInfos) {
	clear();	// get rid of our existing

	for(int i=0; i<newDetectorInfos.count(); i++) {
		AMOldDetectorInfo* newDetectorInfo = qobject_cast<AMOldDetectorInfo*>(newDetectorInfos.at(i));
		if(newDetectorInfo)
			addDetectorInfo(newDetectorInfo);
	}
}

QString AMOldDetectorInfoSet::dbReadActiveDetectorInfos() {
	QString rv = "activeDetectorInfosVersion1.0";
	for(int x = 0; x < count(); x++){
		if(isActiveAt(x))
			rv.append(",true");
		else
			rv.append(",false");
	}
	return rv;
}

void AMOldDetectorInfoSet::dbLoadActiveDetectorInfos(const QString &activeDetectorInfos){
	if(activeDetectorInfos.isEmpty() || !activeDetectorInfos.contains("activeDetectorInfosVersion1.0")){
		dbLoadWarnings_ = "This scan was run before the detector enable states were saved, if you wish to proceed you may have to edit the configuration and reselect your detectors.";
		if(!warningsSuppressed_)
			AMErrorMon::alert(this, AMDETECTORSET_NO_ENABLE_INFO_IN_DB, "The detector info active states were not saved for this AMDetectorInfoSet");
		return;
	}

	if(activeDetectorInfos.count(',') != count()){
		dbLoadWarnings_ = "Somehow there is a mismatch between the number of detectors available and the information about which were selected, if you wish to proceed you may have to edit the configuration and reselect your detectors.";
		if(!warningsSuppressed_)
			AMErrorMon::alert(this, AMDETECTORSET_ENABLE_INFO_COUNT_MISMATCH, "There was a mismatch in the counts for the saved active states and the number of detector infos for this AMDetectorInfoSet");
		return;
	}

	QStringList activeList = activeDetectorInfos.split(',');
	activeList.removeFirst(); // Get rid of the version information

	for(int x = 0; x < count(); x++)
		if(activeList.at(x) == "true")
			setActiveAt(x, true); // No need to set active at x as false ... it starts that way

	return;
}

int AMOldDetectorInfoSet::indexOf(AMOldDetectorInfo *detectorInfo) const{
	return indexOfValue(detectorInfo);
}

int AMOldDetectorInfoSet::indexOf(const QString& detectorName) const{
	return indexOfKey(detectorName);
}

AMOldDetectorInfo* AMOldDetectorInfoSet::detectorInfoNamed(const QString& detectorName){
	int index = indexOfKey(detectorName);
	if(index < 0)
		return 0; // NULL
	return at(index).first;
}

const AMOldDetectorInfo* AMOldDetectorInfoSet::detectorInfoNamed(const QString &detectorName) const{
	int index = indexOfKey(detectorName);
	if(index < 0)
		return 0; // NULL
	return at(index).first;
}

AMOldDetectorInfo* AMOldDetectorInfoSet::detectorInfoAt(int index){
	if(index < 0 || index >= count())
		return 0; //NULL
	return at(index).first;
}

AMOldDetectorInfo* AMOldDetectorInfoSet::detectorInfoAt(int index) const{
	if(index < 0 || index >= count())
		return 0; //NULL
	return at(index).first->toNewInfo();
}

bool AMOldDetectorInfoSet::isActiveNamed(const QString& detectorName) const{
	int index = indexOf(detectorName);
	if(index > 0)
		return at(index).second;
	return false;
}

bool AMOldDetectorInfoSet::isActiveDetectorInfo(AMOldDetectorInfo *detectorInfo) const{
	int index = indexOf(detectorInfo);
	if(index > 0)
		return at(index).second;
	return false;
}

bool AMOldDetectorInfoSet::isActiveAt(int index) const{
	if(index < 0 || index >= count())
		return false;
	return at(index).second;
}

bool AMOldDetectorInfoSet::addDetectorInfo(AMOldDetectorInfo *newDetectorInfo, bool isActive){
	if(!newDetectorInfo)
		return false;

	return append(QPair<AMOldDetectorInfo*, bool>(newDetectorInfo, isActive), newDetectorInfo->name());
}

bool AMOldDetectorInfoSet::removeDetector(AMOldDetectorInfo *detectorInfo){
	int index = indexOf(detectorInfo);
	if(index < 0)
		return false;

	remove(index);
	return true;
}

bool AMOldDetectorInfoSet::setActiveNamed(const QString& detectorName, bool active){
	int index = indexOf(detectorName);
	if(index < 0)
		return false;
	((*this)[index]).second = active;
	return true;
}

bool AMOldDetectorInfoSet::setActiveDetectorInfo(AMOldDetectorInfo *detectorInfo, bool active){
	int index = indexOf(detectorInfo);
	if(index < 0)
		return false;
	((*this)[index]).second = active;
	return true;
}

bool AMOldDetectorInfoSet::setActiveAt(int index, bool active){
	if(index < 0 || index > count())
		return false;
	((*this)[index]).second = active;
	return true;
}

QString AMOldDetectorInfoSet::dbLoadWarnings() const{
	return dbLoadWarnings_;
}

void AMOldDetectorInfoSet::setDescription(const QString &description){
	description_ = description;
	setModified(true);
}

void AMOldDetectorInfoSet::setWarningsSuppressed(bool warningsSuppressed){
	warningsSuppressed_ = warningsSuppressed;
}

void AMOldDetectorInfoSet::onDetectorValuesChanged(int index) {
	setModified(true);
	emit detectorValuesChanged(index);
}

void AMOldDetectorInfoSet::onDetectorAdded(int index) {
	setModified(true);
	emit detectorAdded(index);
}

void AMOldDetectorInfoSet::onDetectorRemoved(int index) {
	setModified(true);
	emit detectorRemoved(index);
}

int AMOldDetectorInfoSet::indexOfValue(const AMOldDetectorInfo *detectorInfo) const{
	int count = values_.count();
	for(int i=0; i<count; i++) {
		if(values_.at(i).first->name() == detectorInfo->name())
			return i;
	}
	return -1;
}
