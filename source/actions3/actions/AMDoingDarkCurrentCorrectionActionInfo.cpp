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


#include "AMDoingDarkCurrentCorrectionActionInfo.h"

AMDoingDarkCurrentCorrectionActionInfo::AMDoingDarkCurrentCorrectionActionInfo(AMDetectorDwellTimeSource *newDwellTimeSource, int newTime, QObject *parent):
    AMActionInfo3("Doing Dark Current Correction", "Doing Dark Current Correction", ":/system-run.png", parent)
{
    dwellTimeSource_ = newDwellTimeSource;
    dwellTime_ = newTime;
}

AMDoingDarkCurrentCorrectionActionInfo::AMDoingDarkCurrentCorrectionActionInfo(const AMDoingDarkCurrentCorrectionActionInfo &other) :
    AMActionInfo3(other)
{
    dwellTimeSource_ = other.dwellTimeSource();
    dwellTime_ = other.dwellTime();
}

AMDoingDarkCurrentCorrectionActionInfo::~AMDoingDarkCurrentCorrectionActionInfo() {}

AMActionInfo3* AMDoingDarkCurrentCorrectionActionInfo::createCopy() const {
    return new AMDoingDarkCurrentCorrectionActionInfo(*this);
}

QString AMDoingDarkCurrentCorrectionActionInfo::typeDescription() const {
    return "Emit signal with updated dark current correction dwell time.";
}

AMDetectorDwellTimeSource* AMDoingDarkCurrentCorrectionActionInfo::dwellTimeSource() const
{
    return dwellTimeSource_;
}

double AMDoingDarkCurrentCorrectionActionInfo::dwellTime() const
{
    return dwellTime_;
}

void AMDoingDarkCurrentCorrectionActionInfo::setDwellTimeSource(AMDetectorDwellTimeSource *newDwellTimeSource)
{
    if (newDwellTimeSource != dwellTimeSource_)
        dwellTimeSource_ = newDwellTimeSource;
}

void AMDoingDarkCurrentCorrectionActionInfo::setDwellTime(double newTime)
{
    if (newTime > 0 && newTime != dwellTime_)
        dwellTime_ = newTime;
}
