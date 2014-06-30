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


#ifndef AMDOINGDARKCURRENTCORRECTIONACTION_H
#define AMDOINGDARKCURRENTCORRECTIONACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMDoingDarkCurrentCorrectionActionInfo.h"

class AMDoingDarkCurrentCorrectionAction : public AMAction3
{
    Q_OBJECT

public:
    explicit AMDoingDarkCurrentCorrectionAction(AMDoingDarkCurrentCorrectionActionInfo *info, QObject *parent = 0);
    AMDoingDarkCurrentCorrectionAction(const AMDoingDarkCurrentCorrectionAction &other);
    virtual ~AMDoingDarkCurrentCorrectionAction();

    virtual AMAction3* createCopy() const;

    virtual bool canPause() const;
    virtual bool canSkip() const;
    virtual bool hasChildren() const;
    virtual int numberOfChildren() const;

protected slots:
    void goFail();
    void goSucceed();

protected:
    virtual void startImplementation();
    virtual void pauseImplementation();
    virtual void resumeImplementation();
    virtual void cancelImplementation();
    virtual void skipImplementation(const QString &command);

    const AMDoingDarkCurrentCorrectionActionInfo* doingDarkCurrentCorrectionInfo() const;
    AMDoingDarkCurrentCorrectionActionInfo* doingDarkCurrentCorrectionInfo();

};

#endif // AMDOINGDARKCURRENTCORRECTIONACTION_H
