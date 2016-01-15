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


#include "AMDetectorDwellTimeActionInfo.h"

 AMDetectorDwellTimeActionInfo::~AMDetectorDwellTimeActionInfo(){}
AMDetectorDwellTimeActionInfo::AMDetectorDwellTimeActionInfo(const AMDetectorInfo &detectorInfo, double dwellSeconds, QObject *parent) :
	AMActionInfo3("Detector Dwell", "Detector Dwell", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	dwellSeconds_ = dwellSeconds;

	QString description = QString("Dwell Time %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorDwellTimeActionInfo::AMDetectorDwellTimeActionInfo(const AMDetectorDwellTimeActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
	dwellSeconds_ = other.dwellSeconds();
}

AMActionInfo3 *AMDetectorDwellTimeActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorDwellTimeActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMDetectorDwellTimeActionInfo::setDwellSeconds(double dwellSeconds)
{
	 dwellSeconds_ = dwellSeconds;
	 setModified(true);
}
