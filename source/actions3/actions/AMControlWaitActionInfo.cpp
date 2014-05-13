#include "AMControlWaitActionInfo.h"

AMControlWaitActionInfo::AMControlWaitActionInfo(const AMControlInfo &setpoint, double timeout, AMControlWaitActionInfo::MatchType matchType, QObject *parent) :
    AMActionInfo3("Wait for control", "Wait for control", QString(), parent)
{
    controlInfo_ = setpoint;
    timeout_ = timeout;
    matchType_ = matchType;

}

AMControlWaitActionInfo::~AMControlWaitActionInfo(){}

AMControlWaitActionInfo::AMControlWaitActionInfo(const AMControlWaitActionInfo &other) :
    AMActionInfo3(other)
{
    controlInfo_.setValuesFrom(*(other.controlInfo()));
    timeout_ = other.timeout();
    matchType_ = other.matchType();

}

double AMControlWaitActionInfo::timeout() const
{
    return timeout_;
}

AMControlWaitActionInfo::MatchType AMControlWaitActionInfo::matchType() const
{
    return matchType_;
}

void AMControlWaitActionInfo::setSetpoint(double setpoint)
{
    controlInfo_.setValue(setpoint);
    setModified(true);
    updateDescriptionText();
}

void AMControlWaitActionInfo::setControlInfo(const AMControlInfo &controlInfo)
{
    controlInfo_.setValuesFrom(controlInfo);
    setModified(true);
    updateDescriptionText();
}

void AMControlWaitActionInfo::setTimeout(double newTimeout)
{
    if (newTimeout <= 0)
        return;

    if (newTimeout == timeout_)
        return;

    timeout_ = newTimeout;
    setModified(true);
    updateDescriptionText();
}

void AMControlWaitActionInfo::setMatchType(AMControlWaitActionInfo::MatchType newMatchType)
{
    if (newMatchType == matchType_)
        return;

    matchType_ = newMatchType;
    setModified(true);
    updateDescriptionText();
}

int AMControlWaitActionInfo::matchTypeAsInt() const
{
    return int(matchType());
}

void AMControlWaitActionInfo::setMatchTypeAsInt(int newMatchType)
{
    setMatchType(AMControlWaitActionInfo::MatchType(newMatchType));
}

void AMControlWaitActionInfo::updateDescriptionText()
{
    QString d;

    switch(matchType_) {
    case AMControlWaitActionInfo::MatchEqual:
        d = QString("Waiting for %1 to become equal to %2.").arg(controlInfo_.name()).arg(controlInfo_.value());
        break;
    case AMControlWaitActionInfo::MatchGreaterThan:
        d = QString("Waiting for %1 to become greater than %2.").arg(controlInfo_.name()).arg(controlInfo_.value());
        break;
    case AMControlWaitActionInfo::MatchLessThan:
        d = QString("Waiting for %1 to become less than %2.").arg(controlInfo_.name()).arg(controlInfo_.value());
        break;
    case AMControlWaitActionInfo::MatchInvalid:
        d = QString("This control wait is invalid.");
        break;
    }

    setShortDescription(d);
    setLongDescription(d);
}
