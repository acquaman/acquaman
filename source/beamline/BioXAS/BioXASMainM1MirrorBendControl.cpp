#include "BioXASMainM1MirrorBendControl.h"

BioXASMainM1MirrorBendControl::BioXASMainM1MirrorBendControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorBendControl(name, units, parent, description)
{
	/// Initialize inherited variables.

	setMinimumValue( calculateBendRadius(12, 12) );
	setMaximumValue( calculateBendRadius(0.0001, 0.0001) );
	setTolerance(50);

	/// Current settings.

	updateStates();
}

BioXASMainM1MirrorBendControl::~BioXASMainM1MirrorBendControl()
{

}

double BioXASMainM1MirrorBendControl::calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue) const
{
	double radius1 = -7334.49 + 110164.0/upstreamBenderValue + 1908.99 * log(upstreamBenderValue);
	double radius2 = -12614.2 + 120562.0/downstreamBenderValue + 3932.91 * log(downstreamBenderValue);

	double radius = (radius1 + radius2) / 2.0;

	return radius;
}

double BioXASMainM1MirrorBendControl::calculateUpstreamBenderValue(double bendRadius) const
{
	double result = 17.6012 + 49019.4/bendRadius - 1.53696 * log(bendRadius);
	return result;
}

double BioXASMainM1MirrorBendControl::calculateDownstreamBenderValue(double bendRadius) const
{
	double result = 3.27884 + 78691.3/bendRadius -0.265485 * log(bendRadius);
	return result;
}
