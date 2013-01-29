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
	connectedSet_ = new AMDetectorSet(this);
	unconnectedSet_ = new AMDetectorSet(this);

	connect(connectedSet_, SIGNAL(connected(bool)), this, SLOT(onConnectedSetConnectedChanged(bool)));
	connect(unconnectedSet_, SIGNAL(connected(bool)), this, SLOT(onUnconnectedSetConnectedChanged(bool)));
}

QString AMDetectorGroup::name() const{
	return name_;
}

bool AMDetectorGroup::allAreConnected() const{
	return (connectedSet_->isConnnected() && (connectedSet_->count() > 0) && (unconnectedSet_ == 0));
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

	if(detector->isConnected())
		return connectedSet_->addDetector(detector);
	else
		return unconnectedSet_->addDetector(detector);
}

bool AMDetectorGroup::removeDetector(AMDetector *detector){
	if(!detector)
		return false;

	if(detector->isConnected())
		return connectedSet_->removeDetector(detector);
	else
		return unconnectedSet_->removeDetector(detector);
}

void AMDetectorGroup::setName(const QString &name){
	name_ = name;
}

void AMDetectorGroup::onConnectedSetConnectedChanged(bool detectorConnected){
	if(detectorConnected)
		AMErrorMon::debug(this, AMDETECTORGROUP_UNEXPECTED_CONNECTED_CHANGED, QString("That's unexpected, the connected set should never signal connected=true. Detector Group Name: %1.").arg(name()) );

	AMDetector *switchDetector;
	for(int x = connectedSet_->count()-1; x >= 0; x--){
		if(!connectedSet_->at(x)->isConnected()){
			switchDetector = connectedSet_->at(x);
			connectedSet_->removeDetector(switchDetector);
			unconnectedSet_->addDetector(switchDetector);
		}
	}
}

void AMDetectorGroup::onUnconnectedSetConnectedChanged(bool detectorConnected){
	if(!detectorConnected)
		AMErrorMon::debug(this, AMDETECTORGROUP_UNEXPECTED_UNCONNECTED_CHANGED, QString("That's unexpected, the unconnected set should never signal connected=false. Detector Group Name: %1.").arg(name()) );

	AMDetector *switchDetector;
	for(int x = unconnectedSet_->count()-1; x >= 0; x--){
		if(!unconnectedSet_->at(x)->isConnected()){
			switchDetector = unconnectedSet_->at(x);
			unconnectedSet_->removeDetector(switchDetector);
			connectedSet_->addDetector(switchDetector);
		}
	}
}
