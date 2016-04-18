#ifndef AMTRAPEZOIDVELOCITYPROFILE_H
#define AMTRAPEZOIDVELOCITYPROFILE_H
#include <QtCore>
/*!
 * \brief A class which represents the velocity profile of a some motion which
 * follows a trapezoidal structure ie. it begins at some base velocity, accelerating
 * at the initialAcceleration towards some target velocity; when it reached the
 * point at which it must decelerate to return to the base velocity by the end
 * of the motion changing velocity at the rate defined by returnAcceleration it
 * will begin its deceleration.
 * NOTE 1: If the time of the motion is not sufficient to accelerate up to the
 * target velocity, the profile will resemble a triangle, rather than a trapezoid.
 */
class AMTrapezoidVelocityProfile
{
public:
	/*!
	 * Creates an invalid velocity profile, whose values are all zero.
	 */
	AMTrapezoidVelocityProfile();

	/*!
	 * Creates a velocity profile which will follow a motion according to the
	 * provided details.
	 * \param startPosition ~ The starting position of the motion
	 * \param endPosition ~ The target position of the motion
	 * \param acceleration ~ The acceleration used during the motion
	 * \param time ~ The time the motion should take
	 */
	AMTrapezoidVelocityProfile(double startPosition,
	                           double endPosition,
	                           double acceleration,
	                           double time);

	/*!
	  * The start position of the motion
	  */
	double startPosition() const;

	/*!
	  * The end position of the motion
	  */
	double endPosition() const;

	/*!
	  * The acceleration used to reach the target velocity
	  */
	double acceleration() const;

	/*!
	  * The time taken for the motion
	  */
	double time() const;

	/*!
	  * The target velocity to which the motion should accelerate in order to reach
	  * the endPosition at time.
	  */
	double targetVelocity() const;

	/*!
	 * Whether the motion profile is valid. Ensures that:
	 *   - If the base velocity is less than the target velocity, the initial acceleration
	 *     must be >0 and the return acceleration must be <0.
	 *   - If the base velocity is greater than the target velocity, the initial
	 *     acceleration must be <0 and the return accleration must be >0.
	 */
	bool isValid() const;

	/*!
	  * Static helper method which gives the time taken to perform a move with the
	  * specified parameters.
	  */
	static double timeForVelocity(double startPosition,
	                              double endPosition,
	                              double acceleration,
	                              double targetVelocity);

protected:
	double startPosition_;
	double endPosition_;
	double acceleration_;
	double time_;
	bool isValid_;

};

#endif // AMTRAPEZOIDVELOCITYPROFILE_H
