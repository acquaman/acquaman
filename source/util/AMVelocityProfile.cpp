#include "AMVelocityProfile.h"

AMVelocityProfile::AMVelocityProfile()
{

}

double AMVelocityProfile::velocityAt(double time) const
{
    if(isValidTime(time) && isValid()) {

        return velocityFromTimeCalculation(time);
    } else {

        return baseValue();
    }

}

double AMVelocityProfile::accelerationAt(double time) const
{
    if(isValidTime(time) && isValid()) {
        return accelerationFromTimeCalculation(time);
    } else {

        return baseValue();
    }
}

QVector<double> AMVelocityProfile::velocityMap(double hertz, double startTime, double endTime) const
{
    if(hertz > 0 && endTime > startTime) {

        int numberOfEntries = int((endTime - startTime) / hertz);


        QVector<double> returnVelocities(numberOfEntries + 1);
        returnVelocities.clear();

        double currentTime = startTime;
        while (currentTime <= endTime) {

            returnVelocities.append(velocityAt(currentTime));

            currentTime += hertz;
        }

        return returnVelocities;
    } else {

        return QVector<double>();
    }
}

QVector<double> AMVelocityProfile::accelerationMap(double hertz, double startTime, double endTime) const
{
    if(hertz > 0 && endTime > startTime) {

        int numberOfEntries = int((endTime - startTime) / hertz);

        QVector<double> returnAccelerations(numberOfEntries + 1);
        returnAccelerations.clear();

        double currentTime = startTime;
        while (currentTime <= endTime) {

            returnAccelerations.append(accelerationAt(currentTime));

            currentTime += hertz;
        }

        return returnAccelerations;
    } else {

        return QVector<double>();
    }
}

bool AMVelocityProfile::isValid() const
{
    return true;
}

bool AMVelocityProfile::isValidTime(double time) const
{
    return (time >= 0);
}

double AMVelocityProfile::baseValue() const
{
    return 0;
}



AMDiscreteVelocityProfile::AMDiscreteVelocityProfile(double timePeriod) :
    AMVelocityProfile()
{
    timePeriod_ = timePeriod;
}

QVector<double> AMDiscreteVelocityProfile::velocityMap(double hertz) const
{
    return AMVelocityProfile::velocityMap(hertz, 0, timePeriod_);
}

QVector<double> AMDiscreteVelocityProfile::accelerationMap(double hertz) const
{
    return AMVelocityProfile::accelerationMap(hertz, 0, timePeriod_);
}

bool AMDiscreteVelocityProfile::isValidTime(double time) const
{
    return (time >= 0 && time <= timePeriod_);
}


AMTrapezoidalVelocityProfile::AMTrapezoidalVelocityProfile(double timePeriod,
                                                           double baseVelocity,
                                                           double targetVelocity,
                                                           double initialAcceleration,
                                                           double returnAcceleration) :
    AMDiscreteVelocityProfile(timePeriod)
{
    baseVelocity_ = baseVelocity;
    targetVelocity_ = targetVelocity;
    initialAcceleration_ = initialAcceleration;
    returnAcceleration_ = returnAcceleration;
}

bool AMTrapezoidalVelocityProfile::isValid() const
{
    return (baseVelocity_ < targetVelocity_ && initialAcceleration_ > 0 && returnAcceleration_ < 0)
            ||
            (baseVelocity_ > targetVelocity_ && initialAcceleration_ < 0 && returnAcceleration_ > 0);
}

double AMTrapezoidalVelocityProfile::baseVelocity() const
{
    return baseVelocity_;
}

double AMTrapezoidalVelocityProfile::targetVelocity() const
{
    return targetVelocity_;
}

double AMTrapezoidalVelocityProfile::initialAcceleration() const
{
    return initialAcceleration_;
}

double AMTrapezoidalVelocityProfile::returnAcceleration() const
{
    return returnAcceleration_;
}

double AMTrapezoidalVelocityProfile::velocityFromTimeCalculation(double time) const
{
    // Calculate the time the theoretical peak of acceleration and deceleration
    // would meet if there was no target velocity. [Done by intercepting their point slope lines]
    double timeOfPeakVelocity = (2 * baseVelocity_ - returnAcceleration_ * timePeriod_) / (initialAcceleration_ - returnAcceleration_);

    if(time < timeOfPeakVelocity) {

        if (baseVelocity_ < targetVelocity_) {

            return qMin(targetVelocity_, baseVelocity_ + initialAcceleration_ * time);
        } else {
            return qMax(targetVelocity_, baseVelocity_ + initialAcceleration_ * time);
        }
    } else {

        // Calculate the peak velocity, and how much we've changed since we passed
        // that peak.
        double peakVelocity = initialAcceleration_ * timeOfPeakVelocity;
        double timeDecelerating = time - timeOfPeakVelocity;
        double deltaVelocity = returnAcceleration_ * timeDecelerating;

        if(baseVelocity_ < targetVelocity_) {

            return qMin(targetVelocity_, (peakVelocity + deltaVelocity));
        } else {

            return qMax(targetVelocity_, (peakVelocity + deltaVelocity));
        }

    }
}

double AMTrapezoidalVelocityProfile::accelerationFromTimeCalculation(double time) const
{
    double currentVelocity = velocityFromTimeCalculation(time);

    double timeOfPeakVelocity = (2 * baseVelocity_ - returnAcceleration_ * timePeriod_) / (initialAcceleration_ - returnAcceleration_);

    if(qAbs(currentVelocity - targetVelocity_) > 0.001) {

        if(time < timeOfPeakVelocity) {
            return initialAcceleration_;
        } else {
            return returnAcceleration_;
        }

    } else {
        return 0;
    }
}

double AMTrapezoidalVelocityProfile::baseValue() const
{
    return baseVelocity_;
}
