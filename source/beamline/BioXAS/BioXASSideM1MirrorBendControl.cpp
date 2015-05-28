#include "BioXASSideM1MirrorBendControl.h"

BioXASSideM1MirrorBendControl::BioXASSideM1MirrorBendControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent, const QString &description) :
	BioXASMirrorBendControl(name, units, upstreamLength, downstreamLength, parent, description)
{

}

BioXASSideM1MirrorBendControl::~BioXASSideM1MirrorBendControl()
{

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

double BioXASSideM1MirrorBendControl::calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue)
{
	return 0;
}
