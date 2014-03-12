#include "AMDetectorInfoSet.h"

 AMDetectorInfoSet::~AMDetectorInfoSet(){}
AMDetectorInfoSet::AMDetectorInfoSet(QObject *parent) :
	AMDbObject(parent), AMOrderedSet<QString, AMDetectorInfo>(false)
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorInfoAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorInfoRemoved(int)));
}

AMDetectorInfoSet::AMDetectorInfoSet(AMDatabase *db, int id) :
	AMDbObject(), AMOrderedSet<QString, AMDetectorInfo>(false)
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorInfoAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorInfoRemoved(int)));

	loadFromDb(db, id);
}

AMDetectorInfoSet::AMDetectorInfoSet(const AMDetectorInfoSet &other) :
	AMDbObject(other), AMOrderedSet<QString, AMDetectorInfo>(other)
{
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDetectorInfoAdded(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDetectorInfoRemoved(int)));
}

AMDetectorInfoSet& AMDetectorInfoSet::operator =(const AMDetectorInfoSet &other){
	// always: check for self-assignment
	if(this != &other) {
		AMOrderedSet<QString, AMDetectorInfo>::operator=(other);
		AMDbObject::operator=(other);

		// if the items in other have been modified, but the signal hasn't been emitted yet, other.modified_ will not be set to true yet. We know that things have changed, so ours should be true.
		if(other.signalSource()->delayedItemChangedScheduled())
			setModified(true);
	}

	return *this;
}

bool AMDetectorInfoSet::operator ==(const AMDetectorInfoSet &other){
	if(count() != other.count())
		return false;

	for(int x = 0; x < count(); x++)
		if(other.indexOf(at(x)) < 0)
			return false;

	return true;
}

bool AMDetectorInfoSet::operator !=(const AMDetectorInfoSet &other){
	return !(this->operator ==(other));
}

QString AMDetectorInfoSet::description() const{
	return description_;
}

AMDbObjectList AMDetectorInfoSet::dbReadDetectorInfos(){
	AMDbObjectList retVal;
	for(int x = 0; x < count(); x++)
		retVal << &((*this)[x]);
	return retVal;
}

void AMDetectorInfoSet::dbLoadDetectorInfos(const AMDbObjectList& newDetectorInfos){
	clear();

	for(int x = 0; x < newDetectorInfos.count(); x++){
		AMDetectorInfo *newDetectorInfo = qobject_cast<AMDetectorInfo*>(newDetectorInfos.at(x));
		if(newDetectorInfo)
			append(*newDetectorInfo, newDetectorInfo->name());

		delete newDetectorInfos.at(x);
	}
}

int AMDetectorInfoSet::indexOf(const AMDetectorInfo &detectorInfo) const{
	return indexOfKey(detectorInfo.name());
}

int AMDetectorInfoSet::indexOf(const QString& detectorName) const{
	return indexOfKey(detectorName);
}

AMDetectorInfo& AMDetectorInfoSet::detectorInfoNamed(const QString& detectorName){
	int index = indexOfKey(detectorName);
	Q_ASSERT(index >= 0);
	return (*this)[index];
}

AMDetectorInfo& AMDetectorInfoSet::detectorInfoAt(int index){
	Q_ASSERT(index >= 0 && index < count());
	return (*this)[index];
}

bool AMDetectorInfoSet::addDetectorInfo(const AMDetectorInfo &newDetectorInfo){
	return append(newDetectorInfo, newDetectorInfo.name());
}

bool AMDetectorInfoSet::removeDetector(const AMDetectorInfo &detectorInfo){
	int index = indexOfKey(detectorInfo.name());
	if(index < 0)
		return false;

	remove(index);
	return true;
}

void AMDetectorInfoSet::setDescription(const QString& description){
	if(description_ != description){
		description_ = description;
		setModified(true);
	}
}

void AMDetectorInfoSet::onDetectorInfoAdded(int index){
	setModified(true);
	emit detectorInfoAdded(index);
}

void AMDetectorInfoSet::onDetectorInfoRemoved(int index){
	setModified(true);
	emit detectorInfoRemoved(index);
}
