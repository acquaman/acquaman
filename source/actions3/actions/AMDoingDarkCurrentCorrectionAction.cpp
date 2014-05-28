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
