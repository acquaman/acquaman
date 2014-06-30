/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMANGLE_H
#define AMANGLE_H

#define AMANGLE_REQUEST_RADIANS_FROM_INVALID_STATE 580001
#define AMANGLE_REQUEST_DEGREES_FROM_INVALID_STATE 580002
#define AMANGLE_REQUEST_GETDEGREES_FROM_INVALID_STATE 580003

/// AMAngle represents an angle
/// automatically converts when
/// needed and forces accesses to
/// request type (degrees, radians, ...)

class AMAngle
{
public:
	enum angleState {DEG, RAD};
	explicit AMAngle();
	AMAngle(double value, angleState state = DEG);
	AMAngle(const AMAngle& angle);

	double radians();
	double degrees();

	double getDegrees() const;

	/// conversion functions
	double radToDeg(double angle) const;
	double degToRad(double angle) const;
	

	/// set angle in radians
	void setRadians(double angle);
	/// set angle in pi radians (specify multiple of pi)
	void setPiRadians(double nAngle);
	/// set angle in degrees
	void setDegrees(double angle);
	/// set angle from another AMAngle
	void setAngle(AMAngle &angle);

protected:
	angleState  state() const;
	double angle() const;

	void setState(angleState state);
	void setAngle(double angle);


protected:

	double angle_;
	angleState state_;
	
};

#endif // AMANGLE_H
