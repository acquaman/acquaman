#include "BioXASMirrorYawControl.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorYawControl::BioXASMirrorYawControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Yaw");

	// Current settings.

	updateStates();
}

BioXASMirrorYawControl::~BioXASMirrorYawControl()
{

}

bool BioXASMirrorYawControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = yaw_->canMeasure();
	}

	return result;
}

bool BioXASMirrorYawControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = yaw_->canMove();
	}

	return result;
}

bool BioXASMirrorYawControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = yaw_->canStop();
	}

	return result;
}

void BioXASMirrorYawControl::updateConnected()
{
	bool isConnected = ( yaw_ && yaw_->isConnected() );

	setConnected( isConnected && validLengths() );
}

void BioXASMirrorYawControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateYaw(upstreamLength_, downstreamLength_, yaw_->value()) );
	}
}

void BioXASMirrorYawControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving(yaw_->isMoving());
	}
}

AMAction3* BioXASMirrorYawControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		double yawDestination = calculateYawPosition(setpoint, upstreamLength_, downstreamLength_);
		AMAction3 *move = AMActionSupport::buildControlMoveAction(yaw_, yawDestination);

		result = move;
	}

	return result;
}


