/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMSingleControlDetector.h"

AMSingleControlDetector::AMSingleControlDetector(const QString &name, AMControl *control, AMDetector::ReadMethod readMethod, QObject *parent) :
		AMDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	control_ = control;
	connect(control_, SIGNAL(connected(bool)), this, SLOT(onControlConnected(bool)));
	connect(control_, SIGNAL(valueChanged(double)), AMDetector::signalSource(), SIGNAL(readingsChanged()));
	onControlConnected(control_->isConnected());
}

AMSingleControlDetector::~AMSingleControlDetector(){
	control_ = NULL;
}

const QMetaObject* AMSingleControlDetector::getMetaObject() {
	return metaObject();
}

double AMSingleControlDetector::reading() const{
	if(isConnected())
		return control_->value();
	else
		return -1;
}

AMDetectorInfo* AMSingleControlDetector::toInfo() const{
	return new AMDetectorInfo(*this);
}

bool AMSingleControlDetector::setControls(AMDetectorInfo *detectorSettings){
	Q_UNUSED(detectorSettings)
	return false;
}

AMControl* AMSingleControlDetector::control() {
	return control_;
}

bool AMSingleControlDetector::setFromInfo(const AMDetectorInfo *info){
	Q_UNUSED(info)
	return false;
}

QString AMSingleControlDetector::description() const{
	return AMDetectorInfo::description();
}

void AMSingleControlDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

void AMSingleControlDetector::onControlConnected(bool connected){
	setConnected(connected);
}

///////////////////////////////////////////////////

AMSingleReadOnlyControlDetector::AMSingleReadOnlyControlDetector(const QString &name, const QString &baseName, AMDetector::ReadMethod readMethod, QObject *parent) :
		AMDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	control_ = new AMReadOnlyPVControl(name, baseName, this);
	connect(control_, SIGNAL(connected(bool)), this, SLOT(onControlConnected(bool)));
	connect(control_, SIGNAL(valueChanged(double)), AMDetector::signalSource(), SIGNAL(readingsChanged()));
}

AMSingleReadOnlyControlDetector::~AMSingleReadOnlyControlDetector(){
}

const QMetaObject* AMSingleReadOnlyControlDetector::getMetaObject() {
	return metaObject();
}

QString AMSingleReadOnlyControlDetector::dacqName() const{
	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(control_);
	if(isConnected() && tmpControl)
		return tmpControl->readPVName();
	else
		return "";
}

double AMSingleReadOnlyControlDetector::reading() const{
	if(isConnected())
		return control_->value();
	else
		return -1;
}

AMDetectorInfo* AMSingleReadOnlyControlDetector::toInfo() const{
	return new AMDetectorInfo(*this);
}

bool AMSingleReadOnlyControlDetector::setControls(AMDetectorInfo *detectorSettings){
	Q_UNUSED(detectorSettings)
	return false;
}

AMControl* AMSingleReadOnlyControlDetector::control() {
	return control_;
}

bool AMSingleReadOnlyControlDetector::setFromInfo(const AMDetectorInfo *info){
	Q_UNUSED(info)
	return false;
}

QString AMSingleReadOnlyControlDetector::description() const{
	return AMDetectorInfo::description();
}

void AMSingleReadOnlyControlDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
	control_->setDescription(description);
}

void AMSingleReadOnlyControlDetector::onControlConnected(bool connected){
	setConnected(connected);
}
