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


#include "AMDetectorTriggerActionInfo.h"

 AMDetectorTriggerActionInfo::~AMDetectorTriggerActionInfo(){}
AMDetectorTriggerActionInfo::AMDetectorTriggerActionInfo(const AMDetectorInfo &detectorInfo, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMActionInfo3("Detector Trigger", "Detector Trigger", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	readMode_ = readMode;
	continousWindowSeconds_ = -1;

	QString description = QString("Trigger %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorTriggerActionInfo::AMDetectorTriggerActionInfo(const AMDetectorTriggerActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
	readMode_ = other.readMode();
	continousWindowSeconds_ = other.continousWindowSeconds();
}

AMActionInfo3 *AMDetectorTriggerActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorTriggerActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}
