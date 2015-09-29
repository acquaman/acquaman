#include "AMTrapezoidVelocityProfile.h"

AMTrapezoidVelocityProfile::AMTrapezoidVelocityProfile()
{

    baseVelocity_ = 0;
    initialAcceleration_ = 0;
    targetVelocity_ = 0;
    returnAcceleration_ = 0;
}

AMTrapezoidVelocityProfile::AMTrapezoidVelocityProfile(double baseVelocity,
                                                       double initialAcceleration,
                                                       double targetVelocity,
                                                       double returnAcceleration)
{

    baseVelocity_ = baseVelocity;
    initialAcceleration_ = initialAcceleration;
    targetVelocity_ = targetVelocity;
    returnAcceleration_ = returnAcceleration;
}

double AMTrapezoidVelocityProfile::baseVelocity() const
{
    return baseVelocity_;
}

double AMTrapezoidVelocityProfile::initialAcceleration() const
{
    return initialAcceleration_;
}

double AMTrapezoidVelocityProfile::targetVelocity() const
{
    return targetVelocity_;
}

double AMTrapezoidVelocityProfile::returnAcceleration() const
{
    return returnAcceleration_;
}

bool AMTrapezoidVelocityProfile::isValid() const
{
    return (baseVelocity_ < targetVelocity_ && initialAcceleration_ >0 && returnAcceleration_ <0) ||
            (baseVelocity_ > targetVelocity_ && initialAcceleration_ <0 && returnAcceleration_ >0);
}

