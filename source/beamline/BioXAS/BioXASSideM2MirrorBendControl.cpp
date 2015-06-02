#include "BioXASSideM2MirrorBendControl.h"

BioXASSideM2MirrorBendControl::BioXASSideM2MirrorBendControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorBendControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Bend");

	setMinimumValue(BIOXASSIDEM2MIRRORBENDCONTROL_MIN_VALUE);
	setMaximumValue(BIOXASSIDEM2MIRRORBENDCONTROL_MAX_VALUE);

	// Current settings.

	updateStates();
}

BioXASSideM2MirrorBendControl::~BioXASSideM2MirrorBendControl()
{

}

double BioXASSideM2MirrorBendControl::calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue)
{
	double radius1 = 1514.68 + (15175.6 / (upstreamBenderValue + 5)) - 379.37 * log(upstreamBenderValue + 5);
	double radius2 = -13440.2 + (144404 / (downstreamBenderValue + 15)) + 2863.27 * log(downstreamBenderValue + 15);

	double radius = (radius1 + radius2) / 2.0;

	return radius;
}

double BioXASSideM2MirrorBendControl::calculateUpstreamBenderValue(double bendRadius)
{
	double result = 14.9095 + 19090.8/bendRadius - 2.40315 * log(bendRadius);
	return result;
}

double BioXASSideM2MirrorBendControl::calculateDownstreamBenderValue(double bendRadius)
{
	double result = 21.5529 + 17259.2/bendRadius - 3.11236 * log(bendRadius);
	return result;
}
