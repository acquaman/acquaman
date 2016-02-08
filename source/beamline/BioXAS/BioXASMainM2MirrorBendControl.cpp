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

double BioXASMainM2MirrorBendControl::calculateUpstreamBendRadius(double upstreamForce) const
{
	double result = 870.191 + 18502.5/upstreamForce - 264.828 * log(upstreamForce);
	return result;
}

double BioXASMainM2MirrorBendControl::calculateDownstreamBendRadius(double downstreamForce) const
{
	double result = 474.389 + 20700.9/downstreamForce - 156.535 * log(downstreamForce);
	return result;
}

double BioXASMainM2MirrorBendControl::calculateUpstreamBenderValue(double bendRadius) const
{
	double result = 13.2496 + 17048.4/bendRadius - 1.47537 * log(bendRadius);
	return result;
}

double BioXASMainM2MirrorBendControl::calculateDownstreamBenderValue(double bendRadius) const
{
	double result = 13.3754 + 17509.5/bendRadius - 1.4731 * log(bendRadius);
	return result;
}
