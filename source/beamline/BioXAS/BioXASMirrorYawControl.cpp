#include "BioXASMirrorYawControl.h"

BioXASMirrorYawControl::BioXASMirrorYawControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, upstreamLength, downstreamLength, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Yaw");

	// Initialize member variables.

	yaw_ = 0;
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

void BioXASMirrorYawControl::setYawControl(AMControl *newControl)
{
	if (yaw_ != newControl) {

		if (yaw_)
			removeChildControl(yaw_);

		yaw_ = newControl;

		if (yaw_)
			addChildControl(yaw_);
	}
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

AMAction3* BioXASMirrorYawControl::createMoveActionIteration(double setpoint)
{
	return 0;
}

double BioXASMirrorYawControl::calculateYawPosition(double yaw, double upstreamLength, double downstreamLength)
{
	return 0;
}

double BioXASMirrorYawControl::calculateYaw(double upstreamLength, double downstreamLength, double yawPosition)
{
	return 0;
}
