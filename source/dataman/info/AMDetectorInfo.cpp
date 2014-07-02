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


#include "AMDetectorInfo.h"

 AMDetectorInfo::~AMDetectorInfo(){}
AMDetectorInfo::AMDetectorInfo(const QString &name, const QString &description, const QString &units, double acquisitionTime, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMDbObject(parent)
{
	setName(name);
	description_ = description;
	units_ = units;
	acquisitionTime_ = acquisitionTime;
	readMode_ = readMode;
}

void AMDetectorInfo::setValuesFrom(const AMDetectorInfo& other) {
	description_ = other.description();
	units_ = other.units();
	acquisitionTime_ = other.acquisitionTime();
	readMode_ = other.readMode();
	setName(other.name());
	setModified(true);
}

void AMDetectorInfo::setDescription(const QString &description){
	if(description_ != description){
		description_ = description;
		setModified(true);
	}
}

void AMDetectorInfo::setUnits(const QString &units){
	if(units_ != units){
		units_ = units;
		setModified(true);
	}
}

void AMDetectorInfo::setAcquisitionTime(double acquisitionTime){
	if(acquisitionTime_ != acquisitionTime){
		acquisitionTime_ = acquisitionTime;
		setModified(true);
	}
}

void AMDetectorInfo::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	if(readMode_ != readMode){
		readMode_ = readMode;
		setModified(true);
	}
}
