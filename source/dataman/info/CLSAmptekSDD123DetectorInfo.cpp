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


#include "CLSAmptekSDD123DetectorInfo.h"

 CLSAmptekSDD123DetectorInfo::~CLSAmptekSDD123DetectorInfo(){}
CLSAmptekSDD123DetectorInfo::CLSAmptekSDD123DetectorInfo(const QString &name, const QString &description, QObject *parent) :
	AMBasicXRFDetectorInfo(name, description, parent)
{
	setUnits("Counts");
}

CLSAmptekSDD123DetectorInfo::CLSAmptekSDD123DetectorInfo(const CLSAmptekSDD123DetectorInfo &original) :
	AMBasicXRFDetectorInfo(original)
{
	this->operator =(original);
}

AMOldDetectorInfo* CLSAmptekSDD123DetectorInfo::toNewInfo() const{
	return new CLSAmptekSDD123DetectorInfo(*this);
}

CLSAmptekSDD123DetectorInfo& CLSAmptekSDD123DetectorInfo::operator =(const CLSAmptekSDD123DetectorInfo &other)
{
	if(this != &other){
		AMBasicXRFDetectorInfo::operator =(other);
		setUnits(other.units());
		setDetectorTemperature(other.detectorTemperature());
	}
	return *this;
}

double CLSAmptekSDD123DetectorInfo::detectorTemperature() const
{
	return detectorTemperature_;
}

void CLSAmptekSDD123DetectorInfo::setDetectorTemperature(double detectorTemperature)
{
	if(detectorTemperature_ != detectorTemperature){
		detectorTemperature_ = detectorTemperature;
		setModified(true);
	}
}
