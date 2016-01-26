#include "BioXASMainM1MirrorBendControl.h"

BioXASMainM1MirrorBendControl::BioXASMainM1MirrorBendControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorBendControl(name, units, parent, description)
{
	/// Initialize inherited variables.

	setMinimumValue( 6000 );
	setMaximumValue( 20000 );
	setTolerance(50);

	/// Current settings.

	updateStates();
}

BioXASMainM1MirrorBendControl::~BioXASMainM1MirrorBendControl()
{

}

double BioXASMainM1MirrorBendControl::calculateUpstreamBendRadius(double upstreamForce) const
{
	double result = 14911.5 - 1210.49/upstreamForce - 4025.05 * log(upstreamForce);
	return result;
}

double BioXASMainM1MirrorBendControl::calculateDownstreamBendRadius(double downstreamForce) const
{
	double result = 16056.7 - 2216.81/downstreamForce - 4452.2 * log(downstreamForce);
	return result;
}

double BioXASMainM1MirrorBendControl::calculateUpstreamBenderValue(double bendRadius) const
{
	double result = -10.9632 + 88056.8/bendRadius + 0.57913 * log(bendRadius);
	return result;
}

double BioXASMainM1MirrorBendControl::calculateDownstreamBenderValue(double bendRadius) const
{
	double result = 4.94817 + 76131.6/bendRadius - 0.992059 * log(bendRadius);
	return result;
}
