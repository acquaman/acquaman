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


#include "CLSPGTDetectorInfo.h"

 CLSPGTDetectorInfo::~CLSPGTDetectorInfo(){}
CLSPGTDetectorInfo::CLSPGTDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMSpectralOutputDetectorInfo(name, description, 1024, "energy", QStringList(), parent)
{
	hvSetpoint_ = double(0.0);
	hvSetpointRangeMin_ = double(0.0);
	hvSetpointRangeMax_ = double(180.0);
}

CLSPGTDetectorInfo::CLSPGTDetectorInfo(const CLSPGTDetectorInfo &original) :
		AMSpectralOutputDetectorInfo(original)
{
//	retreiveAndSetProperties(original);
	this->operator =(original);
}

AMOldDetectorInfo* CLSPGTDetectorInfo::toNewInfo() const{
	return new CLSPGTDetectorInfo(*this);
}

CLSPGTDetectorInfo& CLSPGTDetectorInfo::operator =(const CLSPGTDetectorInfo &other){
	if(this != &other){
//		retreiveAndSetProperties(other);
		AMSpectralOutputDetectorInfo::operator =(other);
		setHVSetpoint(other.hvSetpoint());
		setHVSetpointRangeMin(other.hvSetpointRangeMin());
		setHVSetpointRangeMax(other.hvSetpointRangeMax());
	}
	return *this;
}


double CLSPGTDetectorInfo::hvSetpoint() const {
	return hvSetpoint_;
}

double CLSPGTDetectorInfo::hvSetpointRangeMin() const {
	return hvSetpointRangeMin_;
}

double CLSPGTDetectorInfo::hvSetpointRangeMax() const {
	return hvSetpointRangeMax_;
}

AMRange CLSPGTDetectorInfo::hvSetpointRange() const {
	return AMRange(hvSetpointRangeMin_, hvSetpointRangeMax_);
}

QDebug CLSPGTDetectorInfo::qDebugPrint(QDebug &d) const{
	d << name() << hvSetpoint() << "[hvSetpoint]"
			<< integrationMode() << "[integrationMode]"
			<< integrationTime() << "[integrationTime]";
	return d;
}

bool CLSPGTDetectorInfo::hasDetails() const {
	return true;
}

void CLSPGTDetectorInfo::setHVSetpoint(double hvSetpoint) {
	hvSetpoint_ = hvSetpoint;
	setModified(true);
}

void CLSPGTDetectorInfo::setHVSetpointRangeMin(double min) {
	hvSetpointRangeMin_ = min;
	setModified(true);
}

void CLSPGTDetectorInfo::setHVSetpointRangeMax(double max) {
	hvSetpointRangeMax_ = max;
	setModified(true);
}

void CLSPGTDetectorInfo::setHVSetpointRange(const AMRange &range){
	hvSetpointRangeMin_ = range.minimum();
	hvSetpointRangeMax_ = range.maximum();
	setModified(true);
}
