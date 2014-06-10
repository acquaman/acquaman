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


#include "AMOldDetector.h"

 AMDetectorSignalSource::~AMDetectorSignalSource(){}
AMDetectorSignalSource::AMDetectorSignalSource(AMOldDetector *parent)
	: QObject() {
	detector_ = parent;
}

void AMDetectorSignalSource::emitConnected(bool isConnected) {
	emit connected(isConnected);
	emit availabilityChagned(detector(), isConnected);
}

void AMDetectorSignalSource::emitInfoChanged() {
	emit infoChanged();
}

void AMDetectorSignalSource::emitReadingsChanged(){
	emit readingsChanged();
}

void AMDetectorSignalSource::emitSettingsChanged() {
	emit settingsChanged();
}

void AMDetectorSignalSource::emitDeleted() {
	emit deleted(detector_);
}



AMOldDetector::AMOldDetector(const QString &name, AMOldDetector::ReadMethod readMethod)
	: name_(name)
{
	readMethod_ = readMethod;
	connected_ = false;
	signalSource_ = new AMDetectorSignalSource(this);
}

AMOldDetector::~AMOldDetector() {
	signalSource_->emitDeleted();
	delete signalSource_;
	signalSource_ = 0;
}

AMDetectorSignalSource* AMOldDetector::signalSource() const {
	return signalSource_;
}

bool AMOldDetector::isConnected() const{
	return connected_;
}

const QMetaObject* AMOldDetector::getMetaObject() {
	return 0;
}

AMOldDetector::ReadMethod AMOldDetector::readMethod() const{
	return readMethod_;
}

QString AMOldDetector::dacqName() const{
	return "";
}

QStringList AMOldDetector::dacqBegin() const{
	return QStringList();
}

QStringList AMOldDetector::dacqMove() const{
	return QStringList();
}

QStringList AMOldDetector::dacqDwell() const{
	return QStringList();
}

QStringList AMOldDetector::dacqFinish() const{
	return QStringList();
}

double AMOldDetector::reading() const{
	return -1;
}

QString AMOldDetector::detectorName() const {
	return name_;
}

void AMOldDetector::setConnected(bool isConnected){
	connected_ = isConnected;
	emitConnected(isConnected);
}

void AMOldDetector::setTimedOut(){
	connected_ = false;
	emitConnected(false);
}

void AMOldDetector::setReadMethod(AMOldDetector::ReadMethod readMethod){
	readMethod_ = readMethod;
}


void AMOldDetector::emitConnected(bool isConnected) {
	signalSource_->emitConnected(isConnected);
}

void AMOldDetector::emitInfoChanged() {
	signalSource_->emitInfoChanged();
}

void AMOldDetector::emitReadingsChanged(){
	signalSource_->emitReadingsChanged();
}

void AMOldDetector::emitSettingsChanged() {
	signalSource_->emitSettingsChanged();
}
