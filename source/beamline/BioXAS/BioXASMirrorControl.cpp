#include "BioXASMirrorControl.h"
#include "actions3/AMListAction3.h"

BioXASMirrorControl::BioXASMirrorControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent, const QString &description) :
	AMPseudoMotorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = -1000000;
	maximumValue_ = 1000000;

	setAllowsMovesWhileMoving(false);
	setTolerance(0.05);

	// Initialize member variables.

	upstreamLength_ = 0;
	downstreamLength_ = 0;

	// Current settings.

	setUpstreamLength(upstreamLength);
	setDownstreamLength(downstreamLength);
}

BioXASMirrorControl::~BioXASMirrorControl()
{

}

bool BioXASMirrorControl::validLengths() const
{
	bool valid = ( upstreamLength_ != -1 && downstreamLength_ != -1 );
	return valid;
}

void BioXASMirrorControl::setUpstreamLength(double newValue)
{
	if (upstreamLength_ != newValue) {
		upstreamLength_ = newValue;
		emit upstreamLengthChanged(upstreamLength_);
	}
}

void BioXASMirrorControl::setDownstreamLength(double newValue)
{
	if (downstreamLength_ != newValue) {
		downstreamLength_ = newValue;
		emit downstreamLengthChanged(downstreamLength_);
	}
}
