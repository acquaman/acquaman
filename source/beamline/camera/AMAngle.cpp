#include "AMAngle.h"

#include <QDebug>
#include <math.h>

AMAngle::AMAngle(QObject *parent) :
	QObject(parent)
{
	setState(DEG);
	setAngle(0);
}

AMAngle::AMAngle(double value, AMAngle::angleState state, QObject *parent) :
	QObject(parent)
{
	setState(state);
	setAngle(value);
}

double AMAngle::radians()
{
	if(state() == RAD)
	{
		return angle();
	}
	else
	{
		if(state() == DEG)
		{
			setState(RAD);
			setAngle(degToRad(angle()));
			return angle();
		}
		else
		{
			qDebug()<<"AMAngle::radians - unknown state";
			return -1;
		}
	}
}

double AMAngle::degrees()
{
	if(state() == DEG)
	{
		return angle();
	}
	else
	{
		if(state() == RAD)
		{
			setState(DEG);
			setAngle(radToDeg(angle()));
			return angle();
		}
		else
		{
			qDebug()<<"AMAngle::degrees - unknown state";
			return -1;
		}
	}
}

/// converts radians to degrees
double AMAngle::radToDeg(double angle)
{
	return 180.0*angle/M_PI;
}

/// converts degrees to radians
double AMAngle::degToRad(double angle)
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

AMAngle::angleState AMAngle::state()
{
	return state_;
}

double AMAngle::angle()
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
