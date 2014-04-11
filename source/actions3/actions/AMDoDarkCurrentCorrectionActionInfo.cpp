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

AMDoDarkCurrentCorrectionActionInfo* AMDoDarkCurrentCorrectionActionInfo::createCopy() const {
    return new AMDoDarkCurrentCorrectionActionInfo(*this);
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
