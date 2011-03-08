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


#include "AMDetector.h"

AMDetectorSignalSource::AMDetectorSignalSource(AMDetector *parent)
	: QObject() {
	detector_ = parent;
}

void AMDetectorSignalSource::emitConnected(bool isConnected) {
	emit connected(isConnected);
}

void AMDetectorSignalSource::emitInfoChanged() {
	emit infoChanged();
}

void AMDetectorSignalSource::emitValuesChanged() {
	emit valuesChanged();
}

void AMDetectorSignalSource::emitDeleted() {
	emit deleted(detector_);
}



AMDetector::AMDetector(const QString& name)
	: name_(name)
{
	connected_ = false;
	signalSource_ = new AMDetectorSignalSource(this);
}

AMDetector::~AMDetector() {
	signalSource_->emitDeleted();
	delete signalSource_;
	signalSource_ = 0;
}

AMDetectorSignalSource* AMDetector::signalSource() const {
	return signalSource_;
}

bool AMDetector::isConnected(){
	return connected_;
}

const QMetaObject* AMDetector::getMetaObject() {
	return 0;
}

QString AMDetector::detectorName() const {
	return name_;
}

/*
QString AMDetector::detectorDescription() const {
	return description_;
}

void AMDetector::setDetectorDescription(const QString& description) {
	description_ = description;
	emitInfoChanged();
}
*/

void AMDetector::setConnected(bool isConnected){
	connected_ = isConnected;
	emitConnected(isConnected);
}

void AMDetector::emitConnected(bool isConnected) {
	signalSource_->emitConnected(isConnected);
}

void AMDetector::emitInfoChanged() {
	signalSource_->emitInfoChanged();
}

void AMDetector::emitValuesChanged() {
	signalSource_->emitValuesChanged();
}
