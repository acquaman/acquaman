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
	int num = count();
	for(int x = 0; x < num; x++)
		if(!at(x)->isConnected())
			return false;
	return true;
}

QStringList AMDetectorSet::unconnected() const {
	int num = count();
	QStringList retVal;
	for(int x = 0; x < num; x++)
		if(!at(x)->isConnected())
			retVal.append(at(x)->detectorName());
	return retVal;
}

AMDetectorInfoSet AMDetectorSet::toInfoSet() const {
	AMDetectorInfoSet rv;

	int numDetectors = count();
	for(int i=0; i<numDetectors; i++) {
		AMDetector *d = at(i);
		rv.append( d->toInfo() );
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
		connect(newDetector->signalSource(), SIGNAL(readingsChanged()), this, SIGNAL(detectorSetReadingsChanged()));
		connect(newDetector->signalSource(), SIGNAL(settingsChanged()), this, SIGNAL(detectorSetSettingsChanged()));
		//connect(newDetector->signalSource(), SIGNAL(readingsChanged()), this, SLOT(onDetectorReadingsChanged()));
		//connect(newDetector->signalSource(), SIGNAL(settingsChanged()), this, SLOT(onDetectorSettingsChanged()));
		return true;
	}
	return false;
}

bool AMDetectorSet::removeDetector(AMDetector* detector) {
	int index = indexOfValue(detector);
	if(index < 0)
		return false;

	// Check this one for a qobject_cast or the signal source or both
	disconnect(detector->signalSource(), 0, this, 0);
	remove(index);
	return true;
}

bool AMDetectorSet::validInfoSet(const AMDetectorInfoSet &info){
	/// \todo alternate orderings or subsets of the entire list
	AMDetector *tmpDtctr;
	for(int x = 0; x < info.count(); x++){
		tmpDtctr = detectorNamed(info.at(x)->name());
		if(!tmpDtctr)
			return false;
	}
	return true;
}

void AMDetectorSet::setFromInfoSet(const AMDetectorInfoSet& info){
	AMDetector *tmpDtctr;
	for(int x = 0; x < info.count(); x++){
		tmpDtctr = detectorNamed(info.at(x)->name());
		if(tmpDtctr)
			tmpDtctr->setFromInfo(info.at(x));
		/// \todo error checking on else
	}
}

void AMDetectorSet::onConnected(bool dtctrConnected){
	/* Need to figure this one out
	AMDetector *tmpDtctr = 0; //NULL
	if(tmpDtctr = qobject_cast<AMDetectorInfo*>(QObject::sender()))
		emit detectorConnectedChanged(dtctrConnected, tmpDtctr);
	*/
	if(wasConnected_ == true && !dtctrConnected){
		wasConnected_ = false;
		emit connected(false);
	}

	if(isConnected() && !wasConnected_){
		wasConnected_ = true;
		emit connected(true);
	}
}

void AMDetectorSet::onConnectionsTimedOut(){
	if(!wasConnected_)
		emit connected(false);
}
