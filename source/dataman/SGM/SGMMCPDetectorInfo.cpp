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


#include "SGMMCPDetectorInfo.h"

 SGMMCPDetectorInfo::~SGMMCPDetectorInfo(){}
SGMMCPDetectorInfo::SGMMCPDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMOldDetectorInfo(name, description, parent)
{
	hvSetpoint_ = double(0.0);
	hvSetpointRangeMin_ = double(0.0);
	hvSetpointRangeMax_ = double(1400.0);
	units_ = "counts";
}

SGMMCPDetectorInfo::SGMMCPDetectorInfo(const SGMMCPDetectorInfo &original) :
		AMOldDetectorInfo(original)
{
	this->operator =(original);
}

AMOldDetectorInfo* SGMMCPDetectorInfo::toNewInfo() const{
	return new SGMMCPDetectorInfo(*this);
}

SGMMCPDetectorInfo& SGMMCPDetectorInfo::operator =(const SGMMCPDetectorInfo& other){
	if(this != &other){
		AMOldDetectorInfo::operator =(other);
		setHVSetpoint(other.hvSetpoint());
		setHVSetpointRangeMin(other.hvSetpointRangeMin());
		setHVSetpointRangeMax(other.hvSetpointRangeMax());
		setUnits(other.units());
	}
	return *this;
}

double SGMMCPDetectorInfo::hvSetpoint() const
{
	return hvSetpoint_;
}

double SGMMCPDetectorInfo::hvSetpointRangeMin() const
{
	return hvSetpointRangeMin_;
}

double SGMMCPDetectorInfo::hvSetpointRangeMax() const
{
	return hvSetpointRangeMax_;
}

AMRange SGMMCPDetectorInfo::hvSetpointRange() const
{
	return AMRange(hvSetpointRangeMin_, hvSetpointRangeMax_);
}

QDebug SGMMCPDetectorInfo::qDebugPrint(QDebug &d) const{
	d << name() << hvSetpoint() << "[hvSetpoint]";
	return d;
}

bool SGMMCPDetectorInfo::hasDetails() const
{
	return true;
}

void SGMMCPDetectorInfo::setHVSetpoint(double hvSetpoint) {
	hvSetpoint_ = hvSetpoint;
	setModified(true);
}

void SGMMCPDetectorInfo::setHVSetpointRangeMin(double min) {
	hvSetpointRangeMin_ = min;
	setModified(true);
}

void SGMMCPDetectorInfo::setHVSetpointRangeMax(double max) {
	hvSetpointRangeMax_ = max;
	setModified(true);
}

void SGMMCPDetectorInfo::setHVSetpointRange(const AMRange &range){
	hvSetpointRangeMin_ = range.minimum();
	hvSetpointRangeMax_ = range.maximum();
	setModified(true);
}
