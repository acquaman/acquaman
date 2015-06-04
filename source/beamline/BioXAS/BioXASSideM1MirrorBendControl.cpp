#include "BioXASSideM1MirrorBendControl.h"

BioXASSideM1MirrorBendControl::BioXASSideM1MirrorBendControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorBendControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setMinimumValue( calculateBendRadius(12, 12) );
	setMaximumValue( calculateBendRadius(0.0001, 0.0001) );
	setTolerance(50);

	// Current settings.

	updateStates();
}

BioXASSideM1MirrorBendControl::~BioXASSideM1MirrorBendControl()
{

}

double BioXASSideM1MirrorBendControl::calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue)
{
	double radius1 = -18654.3 + 144742/upstreamBenderValue + 5413.6 * log(upstreamBenderValue);
	double radius2 = -28996.3 + 152538/downstreamBenderValue + 9534.08 * log(downstreamBenderValue);

	double radius = (radius1 + radius2) / 2.0;

	return radius;
}

double BioXASSideM1MirrorBendControl::calculateUpstreamBenderValue(double bendRadius)
{
	double result = 9.60504 + 62480.3/bendRadius - 0.797077 * log(bendRadius);
	return result;
}

double BioXASSideM1MirrorBendControl::calculateDownstreamBenderValue(double bendRadius)
{
	double result = -4.93766 + 87074.7/bendRadius + 0.491897 * log(bendRadius);
	return result;
}
