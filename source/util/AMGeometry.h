#ifndef AMGEOMETRY
#define AMGEOMETRY

#endif // AMGEOMETRY
#include <math.h>

/*!
 * Namespace containing helper functions for performing geometry calculations
 */
namespace AMGeometry {

	/*!
	 * \brief Converts radian values to degrees
	 * \param radians ~ The radians value to convert
	 * \return The provided radians value converted to degrees
	 */
	double radiansToDegrees(double radians)
	{
		return 180.0*radians/M_PI;
	}

	/*!
	 * \brief Converts degree values to radians
	 * \param degrees ~ The degree value converted to radians
	 * \return The provided degree value converted to radians
	 */
	double degreesToRadians(double degrees)
	{
		return M_PI*degrees/180.0;
	}
}
