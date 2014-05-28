#include "AMAngle.h"

#include <math.h>

#include "util/AMErrorMonitor.h"

AMAngle::AMAngle()
{
	setState(DEG);
	setAngle(0);
}

AMAngle::AMAngle(double value, AMAngle::angleState state)
{
	setState(state);
	setAngle(value);
}

AMAngle::AMAngle(const AMAngle& angle)
{
	setDegrees(angle.getDegrees());
}

double AMAngle::radians()
{
	if(state() == RAD)
		return angle();
	else{
		if(state() == DEG){
			setState(RAD);
			setAngle(degToRad(angle()));
			return angle();
		}
		else{
			AMErrorMon::alert(0, AMANGLE_REQUEST_RADIANS_FROM_INVALID_STATE, QString("A call was made to AMAngle::radians() when the AMAngle instance was in an invalid state.") );
			return -1;
		}
	}
}

double AMAngle::degrees()
{
	if(state() == DEG)
		return angle();
	else{
		if(state() == RAD){
			setState(DEG);
			setAngle(radToDeg(angle()));
			return angle();
		}
		else{
			AMErrorMon::alert(0, AMANGLE_REQUEST_DEGREES_FROM_INVALID_STATE, QString("A call was made to AMAngle::degrees() when the AMAngle instance was in an invalid state.") );
			return -1;
		}
	}
}

double AMAngle::getDegrees() const
{
	if(state() == DEG)
		return angle();
	else{
		if(state() == RAD)
			return radToDeg(angle());
		else{
			AMErrorMon::alert(0, AMANGLE_REQUEST_GETDEGREES_FROM_INVALID_STATE, QString("A call was made to AMAngle::getDegrees() when the AMAngle instance was in an invalid state.") );
			return 0;
		}
	}
}

/// converts radians to degrees
double AMAngle::radToDeg(double angle) const
{
	return 180.0*angle/M_PI;
}

/// converts degrees to radians
double AMAngle::degToRad(double angle) const
{
	return M_PI*angle/180.0;
}

/// sets angle to angle radians
void AMAngle::setRadians(double angle)
{
	setState(RAD);
	setAngle(angle);
}

/// sets angle to nAngle*PI radians
void AMAngle::setPiRadians(double nAngle)
{
	setState(RAD);
	setAngle(nAngle*M_PI);
}

/// sets angle to angle degrees
void AMAngle::setDegrees(double angle)
{
	setState(DEG);
	setAngle(angle);
}

/// sets the angle from another AMAngle
void AMAngle::setAngle(AMAngle &angle)
{
	setDegrees(angle.degrees());// arbitarary, could do radians as well
}

AMAngle::angleState AMAngle::state() const
{
	return state_;
}

double AMAngle::angle() const
{
	return angle_;
}

void AMAngle::setState(AMAngle::angleState state)
{
	state_ = state;
}

void AMAngle::setAngle(double angle)
{
	angle_ = angle;
}
