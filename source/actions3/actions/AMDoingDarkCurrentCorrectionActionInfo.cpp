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
