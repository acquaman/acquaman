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
	double radius1 = calculateUpstreamBendRadius(upstreamBenderValue);
	double radius2 = calculateDownstreamBendRadius(downstreamBenderValue);

	double radius = (radius1 + radius2) / 2.0;

	return radius;
}

double BioXASSideM1MirrorBendControl::calculateUpstreamBendRadius(double upstreamForce)
{
	double result = -5060.53 + 65308.6/upstreamForce + 1809.02 * log(upstreamForce);
	return result;
}

double BioXASSideM1MirrorBendControl::calculateDownstreamBendRadius(double downstreamForce)
{
	double result = -7730.63 + 65377.9/downstreamForce + 2807.23 * log(downstreamForce);
	return result;
}

double BioXASSideM1MirrorBendControl::calculateUpstreamBenderValue(double bendRadius)
{
	double result = -6.96243 + 63691.6/bendRadius + 0.660993 * log(bendRadius);
	return result;
}

double BioXASSideM1MirrorBendControl::calculateDownstreamBenderValue(double bendRadius)
{
	double result = -12.2374 + 64509.8/bendRadius + 1.16554 * log(bendRadius);
	return result;
}
