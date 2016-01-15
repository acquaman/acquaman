#include "BioXASMainM2MirrorBendControl.h"

BioXASMainM2MirrorBendControl::BioXASMainM2MirrorBendControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorBendControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setMinimumValue( calculateBendRadius(20, 20) );
	setMaximumValue( calculateBendRadius(0.1, 0.1) );
	setTolerance(10);

	// Current settings.

	updateStates();
}

BioXASMainM2MirrorBendControl::~BioXASMainM2MirrorBendControl()
{

}

double BioXASMainM2MirrorBendControl::calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue) const
{
	double radius1 = 3638.76 - (179.413 / upstreamBenderValue) - 910.066 * log(upstreamBenderValue);
	double radius2 = 4007.53 - (245.964 / downstreamBenderValue) - 1039 * log(downstreamBenderValue);

	double radius = (radius1 + radius2) / 2.0;

	return radius;
}

double BioXASMainM2MirrorBendControl::calculateUpstreamBenderValue(double bendRadius) const
{
	double result = 14.9095 + 19090.8/bendRadius - 2.40315 * log(bendRadius);
	return result;
}

double BioXASMainM2MirrorBendControl::calculateDownstreamBenderValue(double bendRadius) const
{
	double result = 21.5529 + 17259.2/bendRadius - 3.11236 * log(bendRadius);
	return result;
}
