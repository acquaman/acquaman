#ifndef AMVELOCITYPROFILE_H
#define AMVELOCITYPROFILE_H

#include <QVector>
/*!
 * An abstract class which allows for the creation of different velocity profiles.
 * The velocity profiles map a time value to a velocity and acceleration.
 */
class AMVelocityProfile
{
public:
    /*!
     * Base constructor for AMVelocityProfile derived classes.
     */
    AMVelocityProfile();

    /*!
     * The velocity defined in the profile at the given time.
     * \param time ~ The time for which the velocity is to be returned.
     */
    double velocityAt(double time) const;

    /*!
     * The acceleration defined in the profile at the given time.
     * \param time ~ The time for which the acceleration is to be returned.
     */
    double accelerationAt(double time) const;

    /*!
     * A vector of velocity values starting from the given time with the given
     * frequency.
     * \param hertz ~ The number of seconds between each element of the vector.
     * \param startTime ~ Offset start time.
     * \param endTime ~ The time till which the velocity map will be filled.
     */
    QVector<double> velocityMap(double hertz, double startTime, double endTime) const;

    /*!
     * A vector of acceleration values starting from the given time with the given
     * frequency.
     * \param hertz ~ The number of seconds between each element of the vector.
     * \param startTime ~ Offset start time.
     * \param endTime ~ The time till which the acceleration map will be filled.
     */
    QVector<double> accelerationMap(double hertz, double startTime, double endTime) const;

    /*!
     * Whether or not the velocity profile can be considered valid.
     * By default returns true.
     */
    virtual bool isValid() const;

protected:

    /*!
     * Whether or not a time value is valid for this velocity profile.
     * By default returns true iff the time is >= 0
     */
    virtual bool isValidTime(double time) const;

    /*!
     * Defines the method used to calculate the velocity as a function of time.
     * In the base class this is pure virtual.
     * \param time ~ The time used to calculate the velocity.
     */
    virtual double velocityFromTimeCalculation(double time) const = 0;

    /*!
     * Defines the method used to calculate the acceleration as a function of
     * time.
     * In the base class this is pure virtual.
     * \param time ~ The time used to calculate the acceleration.
     */
    virtual double accelerationFromTimeCalculation(double time) const = 0;

    /*!
     * Defines the base value of the profile. That is the value which is returned
     * if the time value is not valid. Semantically this is taken to mean the
     * velocity before (or, in the case of discrete profiles, after) the profile
     * applies.
     * By default this returns 0.
     */
    virtual double baseValue() const;

};




/*!
 * An abstract class which allows for the creation of different velocity profiles
 * which each take place over a set period of time.
 */
class AMDiscreteVelocityProfile : public AMVelocityProfile
{
public:
    /*!
     * Base constructor for AMDiscreteVelocityProfile derived classes.
     * \param timePeriod ~ The discrete period of time over which the profile takes
     * place.
     */
    AMDiscreteVelocityProfile(double timePeriod);

    /*!
     * Convenience overload which provides the velocity map for the entire profile
     * period.
     * \param hertz ~ The number of seconds between each element of the vector.
     */
    QVector<double> velocityMap(double hertz) const;

    /*!
     * Convenience overload which provides the acceleration map for the entire
     * profile period.
     * \param hertz ~ The number of seconds between each element of the vector.
     */
    QVector<double> accelerationMap(double hertz) const;

protected:

    /*!
     * Whether the provided time is valid for the velocity profile.
     * \param time ~ The time to ensure that it falls within the discrete time
     * period
     * \return True if the provided time falls within the discrete time period,
     * false otherwise.
     */
    bool isValidTime(double time) const;


    double timePeriod_;
};



/*!
 * A class which represents a non symmetrical trapezoidal velocity profile. That
 * is a velocity profile which begins from some static base velocity, accelerates
 * at a constant rate towards some target velocity, and then decelerates at
 * some possibly different constant rate in order to return to the base velocity
 * after a time period.
 */
class AMTrapezoidalVelocityProfile : public AMDiscreteVelocityProfile
{
public:
    /*!
     * Creates an instance of a trapezoidal velocity profile.
     * \param timePeriod ~ The time period over which the velocity profile applies.
     * \param baseVelocity ~ The base (starting) velocity prior to, and after, our
     * time period.
     * \param targetVelocity ~ The target velocity to which the profile will move.
     * \param initialAcceleration ~ The acceleration used to perform the initial
     * move from base velocity to target velocity.
     * \param returnAcceleration ~ The acceleration used to return from the target
     * velocity to the base velocity.
     */
    AMTrapezoidalVelocityProfile(double timePeriod,
                                 double baseVelocity,
                                 double targetVelocity,
                                 double initialAcceleration,
                                 double returnAcceleration);

    /*!
     * Whether the parameters of the trapezoidal velocity profile are valid.
     * Ensures:
     *   - If the base is lower than the target, then the acceleration is +ve and
     *     the deceleration is -ve.
     *   - If the base is higher than the target, then the acceleration is -ve and
     *     the deceleration is +ve.
     */
    bool isValid() const;

    /*!
     * The base (starting) velocity prior to, and after, the period of motion.
     */
    double baseVelocity() const;

    /*!
     * The target velocity to which the profile will move.
     */
    double targetVelocity() const;

    /*!
     * The acceleration used to perform the initial move from base velocity to
     * target velocity.
     */
    double initialAcceleration() const;

    /*!
     * The acceleration used to return from the target velocity to the base velocity.
     */
    double returnAcceleration() const;
protected:

    /*!
     * The velocity at the provided time within the motion described by the velocity
     * profile.
     * \param time ~ The time whose velocity will be returned.
     */
    double velocityFromTimeCalculation(double time) const;

    /*!
     * The acceleration at the provided time within the motion described by the
     * velocity profile
     * \param time ~ /the time whose acceleration will be returned.
     */
    double accelerationFromTimeCalculation(double time) const;

    /*!
     * The base value for velocity when outside of the discrete period of the motion.
     */
    double baseValue() const;

    double baseVelocity_;
    double targetVelocity_;
    double initialAcceleration_;
    double returnAcceleration_;
};

#endif // AMVELOCITYPROFILE_H
