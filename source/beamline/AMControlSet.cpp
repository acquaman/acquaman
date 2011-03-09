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
	QObject(parent), AMOrderedSet<QString, AMControl*>(false)
{
	wasConnected_ = false;
	QTimer::singleShot(AMCONTROLSET_CONTROL_TIMEOUT_MS, this, SLOT(onConnectionsTimedOut()));
}

QString AMControlSet::name() const {
	return name_;
}

bool AMControlSet::isConnected() const {
	int num = count();
	for(int x = 0; x < num; x++)
		if(!at(x)->isConnected())
			return false;
	return true;
}

QStringList AMControlSet::unconnected() const {
	int num = count();
	QStringList retVal;
	for(int x = 0; x < num; x++)
		if(!at(x)->isConnected())
			retVal.append(at(x)->name());
	return retVal;
}

int AMControlSet::indexOf(AMControl* control) {
	return indexOfValue(control);
}

int AMControlSet::indexOf(const QString& controlName) {
	return indexOfKey(controlName);
}

AMControl* AMControlSet::controlNamed(const QString& controlName) {
	int index = indexOfKey(controlName);
	if(index < 0)
		return 0;

	return at(index);
}

bool AMControlSet::addControl(AMControl* newControl) {
	if(!newControl)
		return false;

	if( append(newControl, newControl->name()) ) {
		connect(newControl, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
		connect(newControl, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged()));
		onConnected(newControl->isConnected());
		if(newControl->isConnected())
			onControlValueChanged();
		return true;
	}
	return false;
}

bool AMControlSet::removeControl(AMControl* control) {
	int index = indexOfValue(control);
	if(index < 0)
		return false;

	disconnect(control, 0, this, 0);
	remove(index);
	return true;
}

AMControlInfoList AMControlSet::toInfoList() const {
	AMControlInfoList rv;

	int numControls = count();
	for(int i=0; i<numControls; i++) {
		AMControl* c = at(i);
		rv.append( AMControlInfo(c->name(), c->value(), c->minimumValue(), c->maximumValue(), c->units()) );
	}

	return rv;
}

bool AMControlSet::validInfoList(const AMControlInfoList &info){
	/// \todo alternate orderings or subsets of the entire list
	AMControl *tmpCtrl;
	for(int x = 0; x < info.count(); x++){
		tmpCtrl = controlNamed(info.at(x).name());
		if(!tmpCtrl)
			return false;
	}
	return true;
}

void AMControlSet::setFromInfoList(const AMControlInfoList& info){
	AMControl *tmpCtrl;
	for(int x = 0; x < info.count(); x++){
		tmpCtrl = controlNamed(info.at(x).name());
		if(tmpCtrl)
			tmpCtrl->move(info.at(x).value());
		/// \todo error checking on else
	}
}

void AMControlSet::onConnected(bool ctrlConnected){
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
}

void AMControlSet::onConnectionsTimedOut(){
	if(!wasConnected_)
		emit connected(false);
}

void AMControlSet::onControlValueChanged(){
	emit controlSetValuesChanged(toInfoList());
}

