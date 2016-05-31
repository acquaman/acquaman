#include "BioXASMirrorYawControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASMirrorYawControl::BioXASMirrorYawControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Yaw");
	setTolerance(0.0001);

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
		result = (
					yawMotor_->canMeasure() &&
					lateralMotor_->canMeasure()
					);
	}

	return result;
}

bool BioXASMirrorYawControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = (
					yawMotor_->canMove() &&
					lateralMotor_->canMove()
					);
	}

	return result;
}

bool BioXASMirrorYawControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = (
					yawMotor_->canStop() &&
					lateralMotor_->canStop()
					);
	}

	return result;
}

void BioXASMirrorYawControl::updateConnected()
{
	bool isConnected = (
				yawMotor_ && yawMotor_->isConnected() &&
				lateralMotor_ && lateralMotor_->isConnected()
				);

	setConnected( isConnected && validLengths() );
}

void BioXASMirrorYawControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateYaw(upstreamLength_, downstreamLength_, yawMotor_->value()) );
	}
}

void BioXASMirrorYawControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving(yawMotor_->isMoving() || lateralMotor_->isMoving());
	}
}

AMAction3* BioXASMirrorYawControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {

		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

		double lateral = calculateLateral(upstreamLength_, downstreamLength_, lateralMotor_->value(), yawMotor_->value());

		double yawDestination = calculateYawPosition(setpoint, upstreamLength_, downstreamLength_);
		move->addSubAction(AMActionSupport::buildControlMoveAction(yawMotor_, yawDestination));

		// The lateral control depends on the yaw and so will appear to move as a consequence of yaw motion. We want to try and correct for this with the following action.

		double lateralDestination = calculateLateralPosition(lateral, upstreamLength_, downstreamLength_, setpoint);
		move->addSubAction(AMActionSupport::buildControlMoveAction(lateralMotor_, lateralDestination));

		result = move;
	}

	return result;
}


