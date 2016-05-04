#include "BioXASSideM1MirrorBendControl.h"

BioXASSideM1MirrorBendControl::BioXASSideM1MirrorBendControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorBendControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setMinimumValue( calculateBendRadius(10.99, 10.91) );
	setMaximumValue( calculateBendRadius(2.27, 2.14) );
	setTolerance(50);

	// Current settings.

	updateStates();
}

BioXASSideM1MirrorBendControl::~BioXASSideM1MirrorBendControl()
{

}

double BioXASSideM1MirrorBendControl::calculateUpstreamBendRadius(double upstreamForce) const
{
	double result = -18654 + 144742/upstreamForce + 5413.54 * log(upstreamForce);
	return result;
}

double BioXASSideM1MirrorBendControl::calculateDownstreamBendRadius(double downstreamForce) const
{
	double result = -28996.6 + 152538/downstreamForce + 9534.2 * log(downstreamForce);
	return result;
}

double BioXASSideM1MirrorBendControl::calculateUpstreamBenderValue(double bendRadius) const
{
	double result = -9.60508 + 62480.2/bendRadius + -0.79708 * log(bendRadius);
	return result;
}

double BioXASSideM1MirrorBendControl::calculateDownstreamBenderValue(double bendRadius) const
{
	double result = -4.93754 + 87074.5/bendRadius + 0.491887 * log(bendRadius);
	return result;
}
