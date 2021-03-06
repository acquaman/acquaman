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


#include "AMDetectorGroup.h"

#include "beamline/AMDetectorSet.h"
#include "util/AMErrorMonitor.h"

 AMDetectorGroup::~AMDetectorGroup(){}
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

	connect(unconnectedSet_, SIGNAL(allDetectorsResponded()), this, SIGNAL(allDetectorsResponded()));
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
	if(!connectedSet_->contains(name) && !unconnectedSet_->contains(name))
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
	emit detectorAddedToGroup(detector);
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
	emit detectorRemovedFromGroup(detector);
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
