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


#include "AMDetectorAcquisitionActionInfo.h"

 AMDetectorAcquisitionActionInfo::~AMDetectorAcquisitionActionInfo(){}
AMDetectorAcquisitionActionInfo::AMDetectorAcquisitionActionInfo(const AMDetectorInfo &detectorInfo, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMActionInfo3("Detector Acquisition", "Detector Acquisition", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	readMode_ = readMode;

	QString description = QString("Acquire %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorAcquisitionActionInfo::AMDetectorAcquisitionActionInfo(const AMDetectorAcquisitionActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
	readMode_ = other.readMode();
}

AMActionInfo3 *AMDetectorAcquisitionActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorAcquisitionActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}
