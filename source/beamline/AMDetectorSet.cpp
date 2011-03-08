#include "AMDetectorSet.h"

AMDetectorSet::AMDetectorSet(QObject *parent) :
	QObject(parent), AMOrderedSet<QString, AMDetector*>(false)
{
	connect(AMOrderedSet<QString, AMDetector*>::signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(detectorAdded(int)));
	wasConnected_ = false;
	QTimer::singleShot(AMDETECTORSET_CONTROL_TIMEOUT_MS, this, SLOT(onConnectionsTimedOut()));
}

QString AMDetectorSet::name() const {
	return name_;
}

bool AMDetectorSet::isConnected() const {
	/*
	int num = count();
	for(int x = 0; x < num; x++)
		if(!at(x)->isConnected())
			return false;
	*/
	return true;
}

QStringList AMDetectorSet::unconnected() const {
	/*
	int num = count();
	QStringList retVal;
	for(int x = 0; x < num; x++)
		if(!at(x)->isConnected())
			retVal.append(at(x)->name());
	return retVal;
	*/
}

AMDetectorInfoSet AMDetectorSet::toInfoSet() const {
	AMDetectorInfoSet rv;

	int numDetectors = count();
	for(int i=0; i<numDetectors; i++) {
		AMDetector *c = at(i);
		//rv.append( AMControlInfo(c->name(), c->value(), c->minimumValue(), c->maximumValue(), c->units()) );
	}

	return rv;
}

int AMDetectorSet::indexOf(AMDetector* detector) {
	return indexOfValue(detector);
}

int AMDetectorSet::indexOf(const QString& detectorName) {
	return indexOfKey(detectorName);
}

AMDetector* AMDetectorSet::detectorNamed(const QString& detectorName) {
	int index = indexOfKey(detectorName);
	if(index < 0)
		return 0;

	return at(index);
}

bool AMDetectorSet::addDetector(AMDetector* newDetector) {
	if(!newDetector)
		return false;

	if( append(newDetector, newDetector->detectorName()) ) {
		connect(newDetector->signalSource(), SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
		connect(newDetector->signalSource(), SIGNAL(valuesChanged()), this, SLOT(onDetectorValueChanged()));
		return true;
	}
	return false;
}

bool AMDetectorSet::removeDetector(AMDetector* detector) {
	int index = indexOfValue(detector);
	if(index < 0)
		return false;

	// Check this one for a qobject_cast or the signal source or both
	//disconnect(detector, 0, this, 0);
	remove(index);
	return true;
}

bool AMDetectorSet::validInfoSet(const AMDetectorInfoSet &info){
	/*
	/// \todo alternate orderings or subsets of the entire list
	AMControl *tmpCtrl;
	for(int x = 0; x < info.count(); x++){
		tmpCtrl = controlNamed(info.at(x).name());
		if(!tmpCtrl)
			return false;
	}
	*/
	return true;
}

void AMDetectorSet::setFromInfoSet(const AMDetectorInfoSet& info){
	/*
	AMControl *tmpCtrl;
	for(int x = 0; x < info.count(); x++){
		tmpCtrl = controlNamed(info.at(x).name());
		if(tmpCtrl)
			tmpCtrl->move(info.at(x).value());
		/// \todo error checking on else
	}
	*/
}

void AMDetectorSet::onConnected(bool ctrlConnected){
	/*
	AMControl *tmpCtrl = 0; //NULL
	if(tmpCtrl = qobject_cast<AMControl*>(QObject::sender()))
		emit controlConnectedChanged(ctrlConnected, tmpCtrl);
	if(wasConnected_ == true && !ctrlConnected){
		wasConnected_ = false;
		emit connected(false);
	}

	if(isConnected() && !wasConnected_){
		wasConnected_ = true;
		emit connected(true);
	}
	*/
}

void AMDetectorSet::onConnectionsTimedOut(){
	if(!wasConnected_)
		emit connected(false);
}

void AMDetectorSet::onDetectorValueChanged(){
	/*
	emit controlSetValuesChanged(toInfoList());
	*/
}
