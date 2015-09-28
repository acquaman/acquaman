#ifndef AMTRAPEZOIDVELOCITYPROFILE_H
#define AMTRAPEZOIDVELOCITYPROFILE_H

/*!
 * \brief A class which represents the velocity profile of a some motion which
 * follows a trapezoidal structure ie. it begins at some base velocity, accelerating
 * at the initialAcceleration towards some target velocity; when it reached the
 * point at which it must decelerate to return to the base velocity by the end
 * of the motion changing velocity at the rate defined by returnAcceleration it
 * will begin its deceleration.
 * NOTE 1: If the time of the motion is not sufficient to accelerate up to the
 * target velocity, the profile will resemble a triangle, rather than a trapezoid.
 * NOTE 2: The velocity profile might describe a motion in which the target velocity
 * is less than the base velocity. In this case the initial acceleration must be
 * <0 and the return acceleration >0.
 */
class AMTrapezoidVelocityProfile
{
public:
    /*!
     * Creates a velocity profile whose base velocity, inital acceleration, target
     * velocity and return acceleration are all zero. This will not be considered
     * a valid velocity profile.
     */
    AMTrapezoidVelocityProfile();

    /*!
     * Creates a velocity profile which will follow a motion according to the
     * provided details.
     * \param baseVelocity ~ The initial velocity prior to the motion commencing.
     * \param initialAcceleration ~ The acceleration which moves from the base
     * velocity to the target velocity.
     * \param targetVelocity ~ The velocity which will be accelerated towards during
     * the motion.
     * \param returnAcceleration ~ The acceleration which returns to the base
     * velocity by the end of the time period over which the motion will take
     * place.
     */
    AMTrapezoidVelocityProfile(double baseVelocity,
                               double initialAcceleration,
                               double targetVelocity,
                               double returnAcceleration);

    /*!
     * The initial velocity prior to the motion commencing.
     */
    double baseVelocity() const;

    /*!
     * The acceleration which moves from the base velocity to the target velocity.
     */
    double initialAcceleration() const;

    /*!
     * The velocity which will be accelerated towards during the motion.
     */
    double targetVelocity() const;

    /*!
     * The acceleration which returns to the base velocity by the end of the time
     * period over which the motion will take place.
     */
    double returnAcceleration() const;

    /*!
     * Whether the motion profile is valid. Ensures that:
     *   - If the base velocity is less than the target velocity, the initial acceleration
     *     must be >0 and the return acceleration must be <0.
     *   - If the base velocity is greater than the target velocity, the initial
     *     acceleration must be <0 and the return accleration must be >0.
     */
    bool isValid() const;
protected:
    double baseVelocity_;
    double initialAcceleration_;
    double targetVelocity_;
    double returnAcceleration_;


};

#endif // AMTRAPEZOIDVELOCITYPROFILE_H
