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


#include "AMDoDarkCurrentCorrectionActionInfo.h"

AMDoDarkCurrentCorrectionActionInfo::AMDoDarkCurrentCorrectionActionInfo(CLSSIS3820Scaler *scaler, double dwellTime, QObject *parent) :
	AMActionInfo3("Perform dark current correction.", "Perform dark current correction.", ":/system-run.png", parent)
{
	scaler_ = scaler;
	dwellTime_ = dwellTime;
}

AMDoDarkCurrentCorrectionActionInfo::AMDoDarkCurrentCorrectionActionInfo(const AMDoDarkCurrentCorrectionActionInfo &other) :
	AMActionInfo3(other)
{
	scaler_ = other.scaler_;
	dwellTime_ = other.dwellTime_;
}

AMDoDarkCurrentCorrectionActionInfo::~AMDoDarkCurrentCorrectionActionInfo(){}

AMActionInfo3* AMDoDarkCurrentCorrectionActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDoDarkCurrentCorrectionActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

QString AMDoDarkCurrentCorrectionActionInfo::typeDescription() const {
	return "Perform dark current correction.";
}

CLSSIS3820Scaler* AMDoDarkCurrentCorrectionActionInfo::scaler() const {
	return scaler_;
}

double AMDoDarkCurrentCorrectionActionInfo::dwellTime() const {
	return dwellTime_;
}
