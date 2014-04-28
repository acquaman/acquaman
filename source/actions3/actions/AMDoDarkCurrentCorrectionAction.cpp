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
