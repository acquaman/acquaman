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


#include "AMDetectorSet.h"

#include <QTimer>
#include "dataman/info/AMDetectorInfoSet.h"
#include "util/AMErrorMonitor.h"

 AMDetectorSet::~AMDetectorSet(){}
AMDetectorSet::AMDetectorSet(QObject *parent) :
	QObject(parent), AMOrderedSet<QString, AMDetector*>(false)
{
	wasConnected_ = false;
	allHaveResponded_ = false;
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

QList<const AMDetector*> AMDetectorSet::timedOutDetectors() const{
	QList<const AMDetector*> retVal;
	for(int x = 0, size = count(); x < size; x++){
		if(at(x)->isTimedOut())
			retVal.append(at(x));
	}
	return retVal;
}

QList<const AMDetector*> AMDetectorSet::unrespondedDetectors() const{
	QList<const AMDetector*> retVal;
	for(int x = 0, size = count(); x < size; x++){
		if(!at(x)->isConnected() && !at(x)->isTimedOut())
			retVal.append(at(x));
	}
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
		connect(detector, SIGNAL(timedOut()), this, SLOT(onTimedOut()));
		onConnectedChanged(detector->isConnected());

		emit detectorAdded(count() - 1);

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

	emit detectorRemoved(index);

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

/*
void AMDetectorSet::setFromInfoSet(const AMDetectorInfoSet &infoSet){
	for(int x = 0; x < infoSet.count(); x++)
		if(detectorNamed(infoSet.at(x).name()))
			detectorNamed(infoSet.at(x).name());
}
*/

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

	if(!allHaveResponded_ && unrespondedDetectors().count() == 0){
		emit allDetectorsResponded();
		allHaveResponded_ = true;
	}
}

void AMDetectorSet::onTimedOut(){
	if(!wasConnected_){
		emit connected(false);
		emit detectorSetTimedOut();
	}
}
