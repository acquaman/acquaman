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


#include "AMDetectorSelector.h"

#include "beamline/AMDetectorGroup.h"
#include "beamline/AMDetectorSet.h"

 AMDetectorSelector::~AMDetectorSelector(){}
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

		connect(detectorGroup_, SIGNAL(detectorRemovedFromGroup(AMDetector*)), this, SLOT(onDetectorRemovedFromGroup(AMDetector*)));
		connect(detectorGroup_, SIGNAL(detectorAddedToGroup(AMDetector*)), this, SLOT(onDetectorAddedToGroup(AMDetector*)));
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

AMDetectorInfoSet AMDetectorSelector::selectedDetectorInfos() const{
	AMDetectorInfoSet retVal;

	AMDetector *detector = 0;
	QList<AMDetector*> foundInPreferentialOrdering;
	for(int x = 0; x < preferentialOrdering_.count(); x++){
		detector = detectorGroup_->detectorByName(preferentialOrdering_.at(x));
		if(detector && detectorIsSelected(detector)){
			retVal.addDetectorInfo(detector->toInfo());
			foundInPreferentialOrdering.append(detector);
		}
	}

	QMap<QString, bool>::const_iterator i = selectedDetectors_.constBegin();
	while (i != selectedDetectors_.constEnd()) {
		detector = detectorGroup_->detectorByName(i.key());
		if(i.value() && !foundInPreferentialOrdering.contains(detector))
			retVal.addDetectorInfo(detector->toInfo());
		++i;
	}
	return retVal;
}

AMDetectorInfoSet AMDetectorSelector::unselectedDetectorInfos() const{
	AMDetectorInfoSet retVal;

	AMDetector *detector = 0;
	QList<AMDetector*> foundInPreferentialOrdering;
	for(int x = 0; x < preferentialOrdering_.count(); x++){
		detector = detectorGroup_->detectorByName(preferentialOrdering_.at(x));
		if(detector && !detectorIsSelected(detector)){
			retVal.addDetectorInfo(detector->toInfo());
			foundInPreferentialOrdering.append(detector);
		}
	}

	for(int x = 0, size = detectorGroup_->connectedDetectors()->count(); x < size; x++){
		detector = detectorGroup_->connectedDetectors()->detectorAt(x);
		if(!detectorIsSelected(detector) && !foundInPreferentialOrdering.contains(detector))
			retVal.addDetectorInfo(detector->toInfo());
	}

	for(int x = 0, size = detectorGroup_->unconnectedDetectors()->count(); x < size; x++){
		detector = detectorGroup_->unconnectedDetectors()->detectorAt(x);
		if(!detectorIsSelected(detector) && !foundInPreferentialOrdering.contains(detector))
			retVal.addDetectorInfo(detector->toInfo());
	}

	return retVal;
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

QStringList AMDetectorSelector::preferentialOrdering() const{
	return preferentialOrdering_;
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

void AMDetectorSelector::setDefaultsSelected(){
	if(!detectorGroup_)
		return;

	QMap<QString, bool>::const_iterator i = defaultDetectors_.constBegin();
	while (i != defaultDetectors_.constEnd()) {
		if(i.value())
			setDetectorSelectedByName(i.key(), true);
		++i;
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

void AMDetectorSelector::setPreferentialOrdering(const QStringList &preferentialOrdering){
	preferentialOrdering_ = preferentialOrdering;
}

void AMDetectorSelector::onDetectorRemovedFromGroup(AMDetector *detector){
	selectedDetectors_.remove(detector->name());
	defaultDetectors_.remove(detector->name());
	preferentialOrdering_.removeAll(detector->name());
}

void AMDetectorSelector::onDetectorAddedToGroup(AMDetector *detector){
	selectedDetectors_.insert(detector->name(), false);
	defaultDetectors_.insert(detector->name(), false);
	preferentialOrdering_.append(detector->name());
}
