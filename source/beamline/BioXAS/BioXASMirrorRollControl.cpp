#include "BioXASMirrorRollControl.h"

BioXASMirrorRollControl::BioXASMirrorRollControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorPseudoMotorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Roll");

	// Current settings.

	updateConnected();
	updateValue();
	updateIsMoving();
}

BioXASMirrorRollControl::~BioXASMirrorRollControl()
{

}

void BioXASMirrorRollControl::updateValue()
{
	if (isConnected()) {
		setValue(0);
	}
}

AMAction3* BioXASMirrorRollControl::createMoveAction(double setpoint)
{
	return 0;
}

double BioXASMirrorRollControl::calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	return 0;
}


