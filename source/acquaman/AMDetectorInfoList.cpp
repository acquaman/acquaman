#include "AMDetectorInfoList.h"

AMDetectorInfoSet::AMDetectorInfoSet(QObject *parent)
	: AMDbObject(parent), AMOrderedList<AMDetectorInfo>()
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));
}

AMDetectorInfoSet::AMDetectorInfoSet(const AMDetectorInfoSet& other)
	: AMDbObject(other), AMOrderedList<AMDetectorInfo>(other)
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));
}

AMDetectorInfoSet::AMDetectorInfoSet(AMDatabase* db, int id)
	: AMDbObject(), AMOrderedList<AMDetectorInfo>() {

	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SLOT(onDetectorValuesChanged(int)));

	loadFromDb(db, id);
}

AMDetectorInfoSet& AMDetectorInfoSet::operator=(const AMDetectorInfoSet& other) {
	// always: check for self-assignment
	if(this != &other) {
		AMOrderedList<AMDetectorInfo>::operator=(other);
		AMDbObject::operator=(other);

		// if the items in other have been modified, but the signal hasn't been emitted yet, other.modified_ will not be set to true yet. We know that things have changed, so ours should be true.
		if(other.signalSource()->delayedItemChangedScheduled())
			setModified(true);
	}

	return *this;
}

QString AMDetectorInfoSet::description(){
	return description_;
}

// Returns a list of pointers to the AMControlInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
AMDbObjectList AMDetectorInfoSet::dbReadDetectorInfos() {
	AMDbObjectList rv;
	for(int i=0; i<count(); i++)
		rv << &((*this)[i]);
	return rv;
}

// Called by the database system on loadFromDb() to give us our new set of AMControlInfo objects. We copy these ones into our internal list and then delete them.
void AMDetectorInfoSet::dbLoadDetectorInfos(const AMDbObjectList& newDetectorInfos) {
	clear();	// get rid of our existing

	for(int i=0; i<newDetectorInfos.count(); i++) {
		AMDetectorInfo* newDetectorInfo = qobject_cast<AMDetectorInfo*>(newDetectorInfos.at(i));
		if(newDetectorInfo) {
			append(*newDetectorInfo);	// note: makes a copy of object pointed to by newControlInfo, and stores in our internal list.
		}

		if(newDetectorInfos.at(i))
			delete newDetectorInfos.at(i);	// we're copying these; don't need to keep these ones around. Our responsibility to delete.
	}
}

void AMDetectorInfoSet::setDescription(const QString &description){
	description_ = description;
	setModified(true);
}




AMOldDetectorInfoSet::AMOldDetectorInfoSet(QObject *parent) :
	QObject(parent)
{
}

//AMDetectorInfoSet::AMDetectorInfoSet(AMDetectorInfoSet *copy, QObject *parent) :
//		QObject(parent)
//{
//
//}

int AMOldDetectorInfoSet::indexOf(const QString &name){
	for(int x = 0; x < detectors_.count(); x++)
		if(name == detectors_.at(x)->name())
			return x;
	return -1;
}

AMDetectorInfo* AMOldDetectorInfoSet::detectorByName(const QString &name){
	int index = indexOf(name);
	if(index != -1)
		return detectorAt(index);
	else
		return NULL;
}

bool AMOldDetectorInfoSet::isDefaultByName(const QString &name){
	int index = indexOf(name);
	if(index != -1)
		return isDefaultAt(index);
	else
		return false;
}

/// Returns false if the AMControl to be added is already in the list; otherwise adds the control and returns true.
bool AMOldDetectorInfoSet::addDetector(AMDetectorInfo* detector, bool defaultDetector) {
	if(detectors_.contains(detector))
		return false;
	detectors_.append(detector);
	defaultDetectors_.append(defaultDetector);
	return true;
}

/// Returns false if the AMControl to be removed is not present; otherwise removes the control and returns true.
bool AMOldDetectorInfoSet::removeDetector(AMDetectorInfo* detector) {
	return detectors_.removeOne(detector);
}
