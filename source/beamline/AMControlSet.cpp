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


#include "AMControlSet.h"

 AMControlSet::~AMControlSet(){}
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
	bool connected = true;

	for(int x = 0, num = count(); x < num && connected; x++) {
		AMControl *control = at(x);
		if ( !(control && control->isConnected()) )
			connected = false;
	}

	return connected;
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

AMControl* AMControlSet::controlNamed(const QString& controlName) const {
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

	if(wasConnected_ == true && !isConnected()){
		wasConnected_ = false;
		emit connected(false);
	}

	if(!wasConnected_ && isConnected()){
		wasConnected_ = true;
		emit connected(true);
	}

	return true;
}

AMControlInfoList AMControlSet::toInfoList() const {
	AMControlInfoList rv;

	int numControls = count();
	for(int i=0; i<numControls; i++)
		rv.append(at(i)->toInfo());

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

int AMControlSet::setFromInfoList(const AMControlInfoList& info){
	AMControl *tmpCtrl;
	int controlsSet = 0;
	for(int x = 0; x < info.count(); x++){
		tmpCtrl = controlNamed(info.at(x).name());
		if(tmpCtrl){
			controlsSet++;
			tmpCtrl->move(info.at(x).value());
		}
	}
	return controlsSet;
}

void AMControlSet::onConnected(bool ctrlConnected){
	AMControl *tmpCtrl = 0; //NULL
	if((tmpCtrl = qobject_cast<AMControl*>(QObject::sender())))
		emit controlConnectedChanged(ctrlConnected, tmpCtrl);
	if(wasConnected_ == true && !ctrlConnected){
		wasConnected_ = false;
		emit connected(false);
	}

	if(!wasConnected_ && isConnected()){
		wasConnected_ = true;
		emit connected(true);
	}
}

void AMControlSet::onConnectionsTimedOut(){
	if(!wasConnected_){
		emit connected(false);
		emit controlSetTimedOut();
	}
}

void AMControlSet::onControlValueChanged(){
	emit controlSetValuesChanged();
}

