/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMDetectorSet.h"

AMDetectorSet::AMDetectorSet(QObject *parent) :
	QObject(parent), AMOrderedSet<QString, QPair<AMDetector*, bool> >(false)
{
	connect(AMOrderedSet<QString, QPair<AMDetector*, bool> >::signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(detectorAdded(int)));
	connect(AMOrderedSet<QString, QPair<AMDetector*, bool> >::signalSource(), SIGNAL(itemRemoved(int)), this, SIGNAL(detectorRemoved(int)));
	wasConnected_ = false;
	QTimer::singleShot(AMDETECTORSET_CONTROL_TIMEOUT_MS, this, SLOT(onConnectionsTimedOut()));
}

QString AMDetectorSet::name() const {
	return name_;
}

bool AMDetectorSet::isConnected() const {
	int num = count();
	for(int x = 0; x < num; x++)
		if(!at(x).first->isConnected())
			return false;
	return true;
}

QStringList AMDetectorSet::unconnected() const {
	int num = count();
	QStringList retVal;
	for(int x = 0; x < num; x++)
		if(!at(x).first->isConnected())
			retVal.append(at(x).first->detectorName());
	return retVal;
}

AMDetectorInfoSet AMDetectorSet::toInfoSet() const {
	AMDetectorInfoSet rv;

	int numDetectors = count();
	for(int i=0; i<numDetectors; i++) {
		AMDetector *d = at(i).first;
		rv.addDetectorInfo( d->toInfo(), isDefaultAt(i) );
	}

	return rv;
}

int AMDetectorSet::indexOf(AMDetector* detector) const{
	int indexWFalse = indexOfValue(QPair<AMDetector*, bool>(detector, false));
	int indexWTrue = indexOfValue(QPair<AMDetector*, bool>(detector, true));
	return std::max(indexWFalse, indexWTrue);
}

int AMDetectorSet::indexOf(const QString& detectorName) const{
	return indexOfKey(detectorName);
}

AMDetector* AMDetectorSet::detectorNamed(const QString& detectorName) {
	int index = indexOfKey(detectorName);
	if(index < 0)
		return 0; //NULL

	return at(index).first;
}

AMDetector* AMDetectorSet::detectorAt(int index) {
	if(index < 0 || index >= count())
		return 0; //NULL
	return at(index).first;
}

bool AMDetectorSet::isDefaultNamed(const QString &detectorName) const{
	int index = indexOf(detectorName);
	if(index > 0)
		return at(index).second;
	return false;
}

bool AMDetectorSet::isDefaultDetector(AMDetector *detector) const{
	int index = indexOf(detector);
	if(index > 0)
		return at(index).second;
	return false;
}

bool AMDetectorSet::isDefaultAt(int index) const{
	if(index < 0 || index >= count())
		return false;
	return at(index).second;
}

bool AMDetectorSet::addDetector(AMDetector* newDetector, bool isDefault) {
	if(!newDetector)
		return false;

	if( append(QPair<AMDetector*, bool>(newDetector, isDefault), newDetector->detectorName()) ) {
		qDebug() << "Adding detector as " << newDetector->detectorName();
		connect(newDetector->signalSource(), SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
		connect(newDetector->signalSource(), SIGNAL(readingsChanged()), this, SIGNAL(detectorSetReadingsChanged()));
		connect(newDetector->signalSource(), SIGNAL(settingsChanged()), this, SIGNAL(detectorSetSettingsChanged()));

		onConnected(newDetector->isConnected());

		return true;
	}
	return false;
}

bool AMDetectorSet::removeDetector(AMDetector* detector) {
	int index = indexOf(detector);
	if(index < 0)
		return false;

	// Check this one for a qobject_cast or the signal source or both
	disconnect(detector->signalSource(), 0, this, 0);
	qDebug() << "Removing at index " << index << " as " << detector->detectorName();
	remove(index);
	if(count() > 0)
		onConnected(detectorAt(0)->isConnected());

	return true;
}

bool AMDetectorSet::validInfoSet(const AMDetectorInfoSet &info){
	/// \todo alternate orderings or subsets of the entire list
	AMDetector *tmpDtctr;
	for(int x = 0; x < info.count(); x++){
		tmpDtctr = detectorNamed(info.detectorInfoAt(x)->name());
		if(!tmpDtctr)
			return false;
	}
	return true;
}

void AMDetectorSet::setFromInfoSet(const AMDetectorInfoSet& info){
	AMDetector *tmpDtctr;
	for(int x = 0; x < info.count(); x++){
		tmpDtctr = detectorNamed(info.detectorInfoAt(x)->name());
		if(tmpDtctr)
			tmpDtctr->setFromInfo(info.at(x).first);
		/// \todo error checking on else
	}
}

void AMDetectorSet::onConnected(bool dtctrConnected){
	/* NTBA March 14, 2011 David Chevrier
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
