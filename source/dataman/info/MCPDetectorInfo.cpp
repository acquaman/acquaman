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


#include "MCPDetectorInfo.h"

MCPDetectorInfo::MCPDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMDetectorInfo(name, description, parent)
{
	hvSetpoint_ = double(0.0);
	hvSetpointRangeMin_ = double(0.0);
	hvSetpointRangeMax_ = double(1400.0);
	units_ = "counts";
}

MCPDetectorInfo::MCPDetectorInfo(const MCPDetectorInfo &original) :
		AMDetectorInfo(original)
{
//	retreiveAndSetProperties(original);
	this->operator =(original);
}

AMDetectorInfo* MCPDetectorInfo::toNewInfo() const{
	return new MCPDetectorInfo(*this);
}

MCPDetectorInfo& MCPDetectorInfo::operator =(const MCPDetectorInfo& other){
	if(this != &other){
//		retreiveAndSetProperties(other);
		AMDetectorInfo::operator =(other);
		setHVSetpoint(other.hvSetpoint());
		setHVSetpointRangeMin(other.hvSetpointRangeMin());
		setHVSetpointRangeMax(other.hvSetpointRangeMax());
		setUnits(other.units());
	}
	return *this;
}

double MCPDetectorInfo::hvSetpoint() const
{
	return hvSetpoint_;
}

double MCPDetectorInfo::hvSetpointRangeMin() const
{
	return hvSetpointRangeMin_;
}

double MCPDetectorInfo::hvSetpointRangeMax() const
{
	return hvSetpointRangeMax_;
}

QPair<double, double> MCPDetectorInfo::hvSetpointRange() const
{
	return QPair<double, double>(hvSetpointRangeMin_, hvSetpointRangeMax_);
}

QDebug MCPDetectorInfo::qDebugPrint(QDebug &d) const{
	d << hvSetpoint() << "[hvSetpoint]";
	return d;
}

bool MCPDetectorInfo::hasDetails() const
{
	return true;
}

void MCPDetectorInfo::setHVSetpoint(double hvSetpoint) {
	hvSetpoint_ = hvSetpoint;
	setModified(true);
}

void MCPDetectorInfo::setHVSetpointRangeMin(double min) {
	hvSetpointRangeMin_ = min;
	setModified(true);
}

void MCPDetectorInfo::setHVSetpointRangeMax(double max) {
	hvSetpointRangeMax_ = max;
	setModified(true);
}

void MCPDetectorInfo::setHVSetpointRange(QPair<double, double> range){
	hvSetpointRangeMin_ = range.first;
	hvSetpointRangeMax_ = range.second;
	setModified(true);
}
