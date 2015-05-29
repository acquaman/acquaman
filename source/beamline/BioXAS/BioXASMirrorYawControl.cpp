#include "BioXASMirrorYawControl.h"
#include "actions3/AMActionSupport.h"

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

bool BioXASMirrorYawControl::validValue(double value) const
{
	Q_UNUSED(value)
	return true;
}

bool BioXASMirrorYawControl::validSetpoint(double value) const
{
	Q_UNUSED(value)
	return true;
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

double BioXASMirrorYawControl::calculateYawPosition(double yaw, double upstreamLength, double downstreamLength)
{
	double result = (downstreamLength - upstreamLength) * tan(yaw * M_PI/180);
	return result;
}

double BioXASMirrorYawControl::calculateYaw(double upstreamLength, double downstreamLength, double yawPosition)
{
	double result = atan(yawPosition / (downstreamLength - upstreamLength)) * 180/M_PI;
	return result;
}
