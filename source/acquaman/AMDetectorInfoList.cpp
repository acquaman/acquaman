#include "AMDetectorInfoList.h"

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
		//AMOrderedList<AMDetectorInfo>::operator=(other);
		for(int x = 0; x < other.count(); x++)
			addDetectorInfo(other.detectorInfoAt(x)->toNewInfo(), other.isActiveAt(x));
			//append(other.at(x)->toNewInfo());
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

#warning "DAVID, CHECK THESE OVER CAREFULLY"
// Returns a list of pointers to the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
AMDbObjectList AMDetectorInfoSet::dbReadDetectorInfos() {
	AMDbObjectList rv;
	for(int i=0; i<count(); i++)
		rv << &( *( ((*this)[i]).first) );
	return rv;
}

#warning "DAVID, CHECK THESE OVER CAREFULLY"
// Called by the database system on loadFromDb() to give us our new set of AMDetectorInfo objects. We copy these ones into our internal list and then delete them.
void AMDetectorInfoSet::dbLoadDetectorInfos(const AMDbObjectList& newDetectorInfos) {
	clear();	// get rid of our existing

	for(int i=0; i<newDetectorInfos.count(); i++) {
		AMDetectorInfo* newDetectorInfo = qobject_cast<AMDetectorInfo*>(newDetectorInfos.at(i));
		if(newDetectorInfo) {
			addDetectorInfo(newDetectorInfo);
			//append(newDetectorInfo);	// note: makes a copy of object pointed to by newControlInfo, and stores in our internal list.
		}

		if(newDetectorInfos.at(i))
			delete newDetectorInfos.at(i);	// we're copying these; don't need to keep these ones around. Our responsibility to delete.
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

const AMDetectorInfo* const AMDetectorInfoSet::detectorInfoNamed(const QString &detectorName) const{
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

const AMDetectorInfo* const AMDetectorInfoSet::detectorInfoAt(int index) const{
	if(index < 0 || index >= count())
		return 0; //NULL
	return at(index).first;
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
