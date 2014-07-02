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


#include "AMDoingDarkCurrentCorrectionAction.h"

AMDoingDarkCurrentCorrectionAction::AMDoingDarkCurrentCorrectionAction(AMDoingDarkCurrentCorrectionActionInfo* info, QObject *parent) :
    AMAction3(info, parent)
{
}

AMDoingDarkCurrentCorrectionAction::AMDoingDarkCurrentCorrectionAction(const AMDoingDarkCurrentCorrectionAction &other) :
    AMAction3(other)
{
}

AMDoingDarkCurrentCorrectionAction::~AMDoingDarkCurrentCorrectionAction(){}

AMAction3* AMDoingDarkCurrentCorrectionAction::createCopy() const {
    return new AMDoingDarkCurrentCorrectionAction(*this);
}

bool AMDoingDarkCurrentCorrectionAction::canPause() const {
    return false;
}

bool AMDoingDarkCurrentCorrectionAction::canSkip() const {
    return false;
}

bool AMDoingDarkCurrentCorrectionAction::hasChildren() const {
    return false;
}

int AMDoingDarkCurrentCorrectionAction::numberOfChildren() const {
    return 0;
}

void AMDoingDarkCurrentCorrectionAction::goFail() {
    setFailed();
}

void AMDoingDarkCurrentCorrectionAction::goSucceed() {
    setSucceeded();
}

void AMDoingDarkCurrentCorrectionAction::startImplementation() {
    AMDetectorDwellTimeSource *dwellTimeSource = doingDarkCurrentCorrectionInfo()->dwellTimeSource();
    int dwellTime = doingDarkCurrentCorrectionInfo()->dwellTime();

    setStarted();
    dwellTimeSource->requestSetDarkCurrentCorrectionTime(dwellTime);
    goSucceed();
}

void AMDoingDarkCurrentCorrectionAction::pauseImplementation() {

}

void AMDoingDarkCurrentCorrectionAction::resumeImplementation() {

}

void AMDoingDarkCurrentCorrectionAction::cancelImplementation() {
    setCancelled();
}

void AMDoingDarkCurrentCorrectionAction::skipImplementation(const QString &command) {
    Q_UNUSED(command)
}

const AMDoingDarkCurrentCorrectionActionInfo* AMDoingDarkCurrentCorrectionAction::doingDarkCurrentCorrectionInfo() const {
    return qobject_cast<const AMDoingDarkCurrentCorrectionActionInfo*>(info());
}

AMDoingDarkCurrentCorrectionActionInfo* AMDoingDarkCurrentCorrectionAction::doingDarkCurrentCorrectionInfo() {
    return qobject_cast<AMDoingDarkCurrentCorrectionActionInfo*>(info());
}
