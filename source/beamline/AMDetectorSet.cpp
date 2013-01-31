#include "AMDetectorSet.h"

#include <QTimer>
#include "dataman/info/AMDetectorInfoSet.h"
#include "util/AMErrorMonitor.h"

AMDetectorSet::AMDetectorSet(QObject *parent) :
	QObject(parent), AMOrderedSet<QString, AMDetector*>(false)
{
	wasConnected_ = false;
	QTimer::singleShot(AMDETECTORSET_DETECTOR_TIMEOUT_MS, this, SLOT(onTimedOut()));
}

QString AMDetectorSet::name() const{
	return name_;
}

AMDetectorInfoSet AMDetectorSet::toInfoSet() const{
	AMDetectorInfoSet retVal;

	int numDetectors = count();
	for(int x = 0; x < numDetectors; x++)
		retVal.append(at(x)->toInfo(), at(x)->toInfo().name());

	return retVal;
}

bool AMDetectorSet::isConnnected() const{
	int detectorCount = count();
	for(int x = 0; x < detectorCount; x++)
		if(!at(x)->isConnected())
			return false;
	return true;
}

QStringList AMDetectorSet::unconnectedDetectors() const{
	int detectorCount = count();
	QStringList retVal;
	for(int x = 0; x < detectorCount; x++)
		if(!at(x)->isConnected())
			retVal.append(at(x)->name());
	return retVal;
}

int AMDetectorSet::indexOf(AMDetector *detector) const{
	return indexOfValue(detector);
}

int AMDetectorSet::indexOf(const QString &detectorName) const{
	return indexOfKey(detectorName);
}

AMDetector* AMDetectorSet::detectorNamed(const QString &detectorName){
	int index = indexOfKey(detectorName);
	if(index < 0)
		return 0;
	return at(index);
}

AMDetector* AMDetectorSet::detectorAt(int index){
	if(index < 0 || index >= count())
		return 0;
	return at(index);
}

bool AMDetectorSet::addDetector(AMDetector *detector){
	if(!detector)
		return false;

	if( append(detector, detector->name()) ){
		connect(detector, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));
		onConnectedChanged(detector->isConnected());
		return true;
	}
	return false;
}

bool AMDetectorSet::removeDetector(AMDetector *detector){
	int index = indexOfValue(detector);
	if(index < 0)
		return false;

	disconnect(detector, 0, this, 0);
	remove(index);

	if(wasConnected_ == true && !isConnnected()){
		wasConnected_ = false;
		emit connected(false);
	}
	if(!wasConnected_ && isConnnected()){
		wasConnected_ = true;
		emit connected(true);
	}

	return true;
}

bool AMDetectorSet::validInfoSet(const AMDetectorInfoSet &infoSet){
	AMDetector *tmpDetector;
	for(int x = 0; x < infoSet.count(); x++){
		tmpDetector = detectorNamed(infoSet.at(x).name());
		if(!tmpDetector)
			return false;
	}
	return true;
}

void AMDetectorSet::setFromInfoSet(const AMDetectorInfoSet &infoSet){

}

void AMDetectorSet::setName(const QString &name){
	name_ = name;
}

void AMDetectorSet::onConnectedChanged(bool detectorConnected){
	AMDetector *tmpDetector = 0; //NULL
	if( (tmpDetector = qobject_cast<AMDetector*>(QObject::sender())) )
		emit detectorConnectedChanged(detectorConnected, tmpDetector);
	if(wasConnected_ == true && !detectorConnected){
		wasConnected_ = false;
		emit connected(false);
	}
	if(!wasConnected_ && isConnnected()){
		wasConnected_ = true;
		emit connected(true);
	}
}

void AMDetectorSet::onTimedOut(){
	if(!wasConnected_){
		emit connected(false);
		emit detectorSetTimedOut();
	}
}


AMDetectorGroup::AMDetectorGroup(const QString &name, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	lastAllAreConnected_ = false;

	connectedSet_ = new AMDetectorSet(this);
	connectedSet_->setName("ConnectedFor"+name);
	unconnectedSet_ = new AMDetectorSet(this);
	unconnectedSet_->setName("UnconnectedFor"+name);

	connect(connectedSet_, SIGNAL(detectorConnectedChanged(bool,AMDetector*)), this, SLOT(onConnectedSetDetectorConnectedChanged(bool,AMDetector*)));
	connect(unconnectedSet_, SIGNAL(detectorConnectedChanged(bool,AMDetector*)), this, SLOT(onUnconnectedSetDetectorConnectedChanged(bool,AMDetector*)));
}

QString AMDetectorGroup::name() const{
	return name_;
}

bool AMDetectorGroup::allAreConnected() const{
	return (connectedSet_->isConnnected() && (connectedSet_->count() > 0) && (unconnectedSet_ == 0));
}

bool AMDetectorGroup::contains(AMDetector *detector) const{
	int connectedIndex = connectedSet_->indexOfValue(detector);
	int unconnectedIndex = unconnectedSet_->indexOfValue(detector);
	if(connectedIndex < 0 && unconnectedIndex < 0)
		return false;
	return true;
}

int AMDetectorGroup::count() const{
	return connectedSet_->count() + unconnectedSet_->count();
}

AMDetector* AMDetectorGroup::detectorByName(const QString &name){
	if(!connectedSet_->contains(name) || !unconnectedSet_->contains(name))
		return 0; //NULL

	if(connectedSet_->contains(name))
		return connectedSet_->detectorNamed(name);
	return unconnectedSet_->detectorNamed(name);
}

AMDetectorSet* AMDetectorGroup::connectedDetectors(){
	return connectedSet_;
}

AMDetectorSet* AMDetectorGroup::unconnectedDetectors(){
	return unconnectedSet_;
}

bool AMDetectorGroup::addDetector(AMDetector *detector){
	if(!detector)
		return false;

	bool retVal;
	if(detector->isConnected())
		retVal = connectedSet_->addDetector(detector);
	else
		retVal = unconnectedSet_->addDetector(detector);

	checkAllAreConnected();
	return retVal;
}

bool AMDetectorGroup::removeDetector(AMDetector *detector){
	if(!detector)
		return false;

	bool retVal;
	if(detector->isConnected())
		retVal = connectedSet_->removeDetector(detector);
	else
		retVal = unconnectedSet_->removeDetector(detector);

	checkAllAreConnected();
	return retVal;
}

void AMDetectorGroup::setName(const QString &name){
	name_ = name;
}

void AMDetectorGroup::onConnectedSetDetectorConnectedChanged(bool detectorConnected, AMDetector *detector){
	if(detectorConnected){
		AMErrorMon::debug(this, AMDETECTORGROUP_UNEXPECTED_CONNECTED_CHANGED, QString("That's unexpected, the connected set should never signal connected=true. Detector Group Name: %1. Detector name: %2").arg(name()).arg(detector->name()) );
		return;
	}

	connectedSet_->removeDetector(detector);
	unconnectedSet_->addDetector(detector);
	emit detectorBecameUnconnected(detector);
	if(lastAllAreConnected_ != allAreConnected()){
		checkAllAreConnected();
		emit allAreConnectedChanged(allAreConnected());
	}
}

void AMDetectorGroup::onUnconnectedSetDetectorConnectedChanged(bool detectorConnected, AMDetector *detector){
	if(!detectorConnected){
		AMErrorMon::debug(this, AMDETECTORGROUP_UNEXPECTED_UNCONNECTED_CHANGED, QString("That's unexpected, the unconnected set should never signal connected=false. Detector Group Name: %1. Detector name: %2").arg(name()).arg(detector->name()) );
		return;
	}

	unconnectedSet_->removeDetector(detector);
	connectedSet_->addDetector(detector);
	emit detectorBecameConnected(detector);
	if(lastAllAreConnected_ != allAreConnected()){
		checkAllAreConnected();
		emit allAreConnectedChanged(allAreConnected());
	}
}

void AMDetectorGroup::checkAllAreConnected(){
	lastAllAreConnected_ = allAreConnected();
}

AMDetectorSelector::AMDetectorSelector(AMDetectorGroup *detectorGroup, QObject *parent) :
	QObject(parent)
{
	detectorGroup_ = detectorGroup;

	if(detectorGroup_){
		AMDetectorSet *connectedSet = detectorGroup_->connectedDetectors();
		for(int x = 0; x < connectedSet->count(); x++){
			selectedDetectors_.insert(connectedSet->at(x)->name(), false);
			defaultDetectors_.insert(connectedSet->at(x)->name(), false);
		}

		AMDetectorSet *unconnectedSet = detectorGroup_->unconnectedDetectors();
		for(int x = 0; x < unconnectedSet->count(); x++){
			selectedDetectors_.insert(unconnectedSet->at(x)->name(), false);
			defaultDetectors_.insert(unconnectedSet->at(x)->name(), false);
		}

		connect(detectorGroup_, SIGNAL(detectorBecameConnected(AMDetector*)), this, SIGNAL(detectorBecameConnected(AMDetector*)));
		connect(detectorGroup_, SIGNAL(detectorBecameUnconnected(AMDetector*)), this, SIGNAL(detectorBecameUnconnected(AMDetector*)));
		connect(detectorGroup_, SIGNAL(allAreConnectedChanged(bool)), this, SIGNAL(allAreConnectedChanged(bool)));
	}
}

bool AMDetectorSelector::detectorIsSelectedByName(const QString &name) const{
	if(!detectorGroup_ || !selectedDetectors_.contains(name))
		return false;
	return selectedDetectors_.value(name);
}

bool AMDetectorSelector::detectorIsSelected(AMDetector *detector) const{
	if(!detectorGroup_ || !detectorGroup_->contains(detector))
		return false;

	return detectorIsSelectedByName(detector->name());
}

bool AMDetectorSelector::detectorIsDefaultByName(const QString &name) const{
	if(!detectorGroup_ || !defaultDetectors_.contains(name))
		return false;
	return defaultDetectors_.value(name);
}

bool AMDetectorSelector::detectorIsDefault(AMDetector *detector) const{
	if(!detectorGroup_ || !detectorGroup_->contains(detector))
		return false;

	return detectorIsDefaultByName(detector->name());
}

AMDetectorGroup* AMDetectorSelector::detectorGroup(){
	return detectorGroup_;
}

int AMDetectorSelector::count() const{
	return detectorGroup_->count();
}

void AMDetectorSelector::setDetectorSelectedByName(const QString &name, bool isSelected){
	if(!detectorGroup_ || !selectedDetectors_.contains(name))
		return;

	if(selectedDetectors_.value(name) != isSelected){
		selectedDetectors_[name] = isSelected;
		AMDetector *detector = detectorGroup_->detectorByName(name);
		emit selectedChanged(detector);
	}
}

void AMDetectorSelector::setDetectorSelected(AMDetector *detector, bool isSelected){
	if(!detectorGroup_ || !detectorGroup_->contains(detector))
		return;

	if(selectedDetectors_.value(detector->name()) != isSelected){
		selectedDetectors_[detector->name()] = isSelected;
		emit selectedChanged(detector);
	}
}

void AMDetectorSelector::setDetectorDefaultByName(const QString &name, bool isDefault){
	if(!detectorGroup_ || !defaultDetectors_.contains(name))
		return;

	if(defaultDetectors_.value(name) != isDefault){
		defaultDetectors_[name] = isDefault;
		AMDetector *detector = detectorGroup_->detectorByName(name);
		emit defaultChanged(detector);
	}
}

void AMDetectorSelector::setDetectorDefault(AMDetector *detector, bool isDefault){
	if(!detectorGroup_ || !detectorGroup_->contains(detector))
		return;

	if(defaultDetectors_.value(detector->name()) != isDefault){
		defaultDetectors_[detector->name()] = isDefault;
		emit defaultChanged(detector);
	}
}
