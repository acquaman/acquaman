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


#include "AMDetectorSetAsDarkCurrentCorrectionActionInfo.h"


AMDetectorSetAsDarkCurrentCorrectionActionInfo::AMDetectorSetAsDarkCurrentCorrectionActionInfo(const AMDetectorInfo &detectorInfo, QObject *parent) :
   AMActionInfo3("Detector Set As Dark Current Correction", "Detector Set As Dark Current Correction", ":/system-run.png", parent)
{
   detectorInfo_.setValuesFrom(detectorInfo);

   QString description = QString("Set As Dark Current Correction %1").arg(detectorInfo_.description());
   setShortDescription(description);
   setLongDescription(description);
}

AMDetectorSetAsDarkCurrentCorrectionActionInfo::~AMDetectorSetAsDarkCurrentCorrectionActionInfo(){}

AMDetectorSetAsDarkCurrentCorrectionActionInfo::AMDetectorSetAsDarkCurrentCorrectionActionInfo(const AMDetectorSetAsDarkCurrentCorrectionActionInfo &other) :
   AMActionInfo3(other)
{
   detectorInfo_.setValuesFrom(*(other.detectorInfo()));
}

AMActionInfo3 *AMDetectorSetAsDarkCurrentCorrectionActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorSetAsDarkCurrentCorrectionActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}
