/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMControlSet.h"

AMControlSet::AMControlSet(QObject *parent) :
	QObject(parent)
{
	wasConnected_ = false;
	info_ = new AMControlInfoList(this);
	QTimer connectionsTimedOut;
	connectionsTimedOut.singleShot(AMCONTROLSET_CONTROL_TIMEOUT_MS, this, SLOT(onConnectionsTimedOut()));
}

int AMControlSet::indexOf(const QString &name){
	for(int x = 0; x < ctrls_.count(); x++)
		if(name == ctrls_.at(x)->name())
			return x;
	return -1;
}

AMControl* AMControlSet::controlByName(const QString &name){
	int index = indexOf(name);
	if(index != -1)
		return controlAt(index);
	else
		return NULL;
}

bool AMControlSet::isConnected(){
	for(int x = 0; x < ctrls_.count(); x++)
		if(!controlAt(x)->isConnected())
			return false;
	return true;
}

void AMControlSet::setName(const QString &name) {
	name_ = name;
	info_->setName(name);
}


/// Returns false if the AMControl to be added is already in the list; otherwise adds the control and returns true.
bool AMControlSet::addControl(AMControl* ctrl) {
	if(ctrls_.contains(ctrl))
		return false;
	ctrls_.append(ctrl);
	connect(ctrl, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	connect(ctrl, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
	info_->append(AMControlInfo(ctrl->name(), ctrl->value(), ctrl->minimumValue(), ctrl->maximumValue(), ctrl->units()));
	return true;
}

/// Returns false if the AMControl to be removed is not present; otherwise removes the control and returns true.
bool AMControlSet::removeControl(AMControl* ctrl) {
	int index = ctrls_.indexOf(ctrl);
	info_->remove(index);
	bool retVal = ctrls_.removeOne(ctrl);
	if(retVal){
		disconnect(ctrl, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
		disconnect(ctrl, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
	}
	return retVal;
}

void AMControlSet::setFromInfo(AMControlInfoList *info){
	AMControl *tmpCtrl;
	for(int x = 0; x < info->count(); x++){
		tmpCtrl = controlByName(info->at(x).name());
		if(tmpCtrl)
			tmpCtrl->move(info->at(x).value());
	}
}

void AMControlSet::onConnected(bool ctrlConnected){
	if(wasConnected_ == true && !ctrlConnected){
		wasConnected_ = false;
		emit connected(false);
	}
	AMControl *ctrl = qobject_cast<AMControl*>(QObject::sender());
	if(!ctrl || !ctrlConnected)
		return;
	int index = ctrls_.indexOf(ctrl);
	info_->replace(index, AMControlInfo(ctrl->name(), ctrl->value(), ctrl->minimumValue(), ctrl->maximumValue(), ctrl->units()));
	if(isConnected() && !wasConnected_){
		wasConnected_ = true;
		emit connected(true);
	}
}

void AMControlSet::onConnectionsTimedOut(){
	if(!wasConnected_)
		emit connected(false);
}

void AMControlSet::onValueChanged(double value){
	AMControl *ctrl = qobject_cast<AMControl*>(QObject::sender());
	if(!ctrl)
		return;
	int index = ctrls_.indexOf(ctrl);
	(*info_)[index].setValue(value);
}
