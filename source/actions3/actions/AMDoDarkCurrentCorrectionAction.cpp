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


#include "AMDoDarkCurrentCorrectionAction.h"
#include <QDebug>

AMDoDarkCurrentCorrectionAction::AMDoDarkCurrentCorrectionAction(AMDoDarkCurrentCorrectionActionInfo* info, QObject *parent) :
    AMAction3(info, parent)
{
}

AMDoDarkCurrentCorrectionAction::AMDoDarkCurrentCorrectionAction(const AMDoDarkCurrentCorrectionAction &other) :
    AMAction3(other)
{
}

AMDoDarkCurrentCorrectionAction::~AMDoDarkCurrentCorrectionAction() {}

AMAction3* AMDoDarkCurrentCorrectionAction::createCopy() const {
    return new AMDoDarkCurrentCorrectionAction(*this);
}

bool AMDoDarkCurrentCorrectionAction::canPause() const {
    return false;
}

bool AMDoDarkCurrentCorrectionAction::canSkip() const {
    return false;
}

bool AMDoDarkCurrentCorrectionAction::hasChildren() const {
    return false;
}

int AMDoDarkCurrentCorrectionAction::numberOfChildren() const {
    return 0;
}

void AMDoDarkCurrentCorrectionAction::onNewDarkCurrentMeasurementState(CLSSIS3820Scaler::DarkCurrentCorrectionState newState) {
    if (newState == CLSSIS3820Scaler::SUCCEEDED)
        setSucceeded();
    else if (newState == CLSSIS3820Scaler::FAILED)
        setFailed();
}

void AMDoDarkCurrentCorrectionAction::startImplementation() {
    qDebug() << "AMDoDarkCurrentCorrectionAction::startImplementation is executing.";

    CLSSIS3820Scaler *scaler = doDarkCurrentCorrectionInfo()->scaler();
    connect( scaler, SIGNAL(newDarkCurrentMeasurementState(CLSSIS3820Scaler::DarkCurrentCorrectionState)), this, SLOT(onNewDarkCurrentMeasurementState(CLSSIS3820Scaler::DarkCurrentCorrectionState)) );
    double dwellTime = doDarkCurrentCorrectionInfo()->dwellTime();

    setStarted();
    scaler->doDarkCurrentCorrection(dwellTime);
}

void AMDoDarkCurrentCorrectionAction::pauseImplementation() {

}

void AMDoDarkCurrentCorrectionAction::resumeImplementation() {

}

void AMDoDarkCurrentCorrectionAction::cancelImplementation() {
    setCancelled();
}

void AMDoDarkCurrentCorrectionAction::skipImplementation(const QString &command) {
    Q_UNUSED(command)
}

const AMDoDarkCurrentCorrectionActionInfo* AMDoDarkCurrentCorrectionAction::doDarkCurrentCorrectionInfo() const {
    return qobject_cast<const AMDoDarkCurrentCorrectionActionInfo*>(info());
}

AMDoDarkCurrentCorrectionActionInfo* AMDoDarkCurrentCorrectionAction::doDarkCurrentCorrectionInfo() {
    return qobject_cast<AMDoDarkCurrentCorrectionActionInfo*>(info());
}
