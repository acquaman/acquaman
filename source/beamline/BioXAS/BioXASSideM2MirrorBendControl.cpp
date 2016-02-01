#include "BioXASSideM2MirrorBendControl.h"

BioXASSideM2MirrorBendControl::BioXASSideM2MirrorBendControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorBendControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setMinimumValue( calculateBendRadius(20, 20) );
	setMaximumValue( calculateBendRadius(0, 0) );
	setTolerance(10);

	// Current settings.

	updateStates();
}

BioXASSideM2MirrorBendControl::~BioXASSideM2MirrorBendControl()
{

}

double BioXASSideM2MirrorBendControl::calculateUpstreamBendRadius(double upstreamForce) const
{
	double result = 893.733 + 21620.4/upstreamForce - 268.868 * log(upstreamForce);
	return result;
}

double BioXASSideM2MirrorBendControl::calculateDownstreamBendRadius(double downstreamForce) const
{
	double result = -2987.93 + 38851.9/downstreamForce + 722.397 * log(downstreamForce);
	return result;
}

double BioXASSideM2MirrorBendControl::calculateUpstreamBenderValue(double bendRadius) const
{
	double result = 20.8111 + 17925/bendRadius - 2.30933 * log(bendRadius);
	return result;
}

double BioXASSideM2MirrorBendControl::calculateDownstreamBenderValue(double bendRadius) const
{
	double result = 0.232579 + 21051.1/bendRadius + 0.167868 * log(bendRadius);
	return result;
}
