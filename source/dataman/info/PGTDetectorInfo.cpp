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


#include "PGTDetectorInfo.h"

PGTDetectorInfo::PGTDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMSpectralOutputDetectorInfo(name, description, 1024, "energy", QStringList(), parent)
{
	hvSetpoint_ = double(0.0);
	hvSetpointRangeMin_ = double(0.0);
	hvSetpointRangeMax_ = double(180.0);
}

PGTDetectorInfo::PGTDetectorInfo(const PGTDetectorInfo &original) :
		AMSpectralOutputDetectorInfo(original)
{
//	retreiveAndSetProperties(original);
	this->operator =(original);
}

AMDetectorInfo* PGTDetectorInfo::toNewInfo() const{
	return new PGTDetectorInfo(*this);
}

PGTDetectorInfo& PGTDetectorInfo::operator =(const PGTDetectorInfo &other){
	if(this != &other){
//		retreiveAndSetProperties(other);
		AMSpectralOutputDetectorInfo::operator =(other);
		setHVSetpoint(other.hvSetpoint());
		setHVSetpointRangeMin(other.hvSetpointRangeMin());
		setHVSetpointRangeMax(other.hvSetpointRangeMax());
	}
	return *this;
}


double PGTDetectorInfo::hvSetpoint() const {
	return hvSetpoint_;
}

double PGTDetectorInfo::hvSetpointRangeMin() const {
	return hvSetpointRangeMin_;
}

double PGTDetectorInfo::hvSetpointRangeMax() const {
	return hvSetpointRangeMax_;
}

QPair<double, double> PGTDetectorInfo::hvSetpointRange() const {
	return QPair<double, double>(hvSetpointRangeMin_, hvSetpointRangeMax_);
}

QDebug PGTDetectorInfo::qDebugPrint(QDebug &d) const{
	d << hvSetpoint() << "[hvSetpoint]"
			<< integrationMode() << "[integrationMode]"
			<< integrationTime() << "[integrationTime]";
	return d;
}

bool PGTDetectorInfo::hasDetails() const {
	return true;
}

void PGTDetectorInfo::setHVSetpoint(double hvSetpoint) {
	hvSetpoint_ = hvSetpoint;
	setModified(true);
}

void PGTDetectorInfo::setHVSetpointRangeMin(double min) {
	hvSetpointRangeMin_ = min;
	setModified(true);
}

void PGTDetectorInfo::setHVSetpointRangeMax(double max) {
	hvSetpointRangeMax_ = max;
	setModified(true);
}

void PGTDetectorInfo::setHVSetpointRange(QPair<double, double> range){
	hvSetpointRangeMin_ = range.first;
	hvSetpointRangeMax_ = range.second;
	setModified(true);
}
