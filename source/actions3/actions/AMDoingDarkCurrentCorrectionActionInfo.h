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


#ifndef AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H
#define AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H

#include "beamline/AMDetectorTriggerSource.h"
#include "actions3/AMActionInfo3.h"

class AMDoingDarkCurrentCorrectionActionInfo : public AMActionInfo3
{
    Q_OBJECT

    Q_PROPERTY(double dwellTime READ dwellTime WRITE setDwellTime)

public:
    Q_INVOKABLE AMDoingDarkCurrentCorrectionActionInfo(AMDetectorDwellTimeSource* dwellTimeSource, int newTime, QObject *parent = 0);
    virtual ~AMDoingDarkCurrentCorrectionActionInfo();

    /// Copy Constructor
    AMDoingDarkCurrentCorrectionActionInfo(const AMDoingDarkCurrentCorrectionActionInfo &other);

    /// This function is used as a virtual copy constructor
    virtual AMActionInfo3* createCopy() const;

    /// This should describe the type of the action
    virtual QString typeDescription() const;

    AMDetectorDwellTimeSource* dwellTimeSource() const;
    double dwellTime() const;

public slots:
    void setDwellTimeSource(AMDetectorDwellTimeSource* newDwellTimeSource);
    void setDwellTime(double newTime);

protected:
    /// The detector taking a dark currents measurement.
    AMDetectorDwellTimeSource* dwellTimeSource_;
    /// The dwell time for the dark current correction presently taking place.
    double dwellTime_;
};

#endif // AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H
