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


#ifndef AMDODARKCURRENTCORRECTIONACTIONINFO_H
#define AMDODARKCURRENTCORRECTIONACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

class AMDoDarkCurrentCorrectionActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
    explicit AMDoDarkCurrentCorrectionActionInfo(CLSSIS3820Scaler *scaler, double dwellTime, QObject *parent = 0);
    virtual ~AMDoDarkCurrentCorrectionActionInfo();

    AMDoDarkCurrentCorrectionActionInfo(const AMDoDarkCurrentCorrectionActionInfo &other);

    virtual AMDoDarkCurrentCorrectionActionInfo* createCopy() const;

    virtual QString typeDescription() const;

    CLSSIS3820Scaler* scaler() const;
    double dwellTime() const;

protected:
    CLSSIS3820Scaler *scaler_;
    double dwellTime_;


};

#endif // AMDODARKCURRENTCORRECTIONACTIONINFO_H
