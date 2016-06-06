#include "BioXASMirrorLateralControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorLateralControl::BioXASMirrorLateralControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Lateral");

	// Current settings.

	updateStates();
}

BioXASMirrorLateralControl::~BioXASMirrorLateralControl()
{

}

bool BioXASMirrorLateralControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = ( lateralMotor_->canMeasure() && yawMotor_->canMeasure() );
	}

	return result;
}

bool BioXASMirrorLateralControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = (lateralMotor_->canMove() && yawMotor_->canMove());
	}

	return result;
}

bool BioXASMirrorLateralControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = (lateralMotor_->canStop() && lateralMotor_->canStop());
	}

	return result;
}

void BioXASMirrorLateralControl::updateConnected()
{
	bool isConnected = (
				lateralMotor_ && lateralMotor_->isConnected() &&
				yawMotor_ && yawMotor_->isConnected()
				);

	setConnected(isConnected && validLengths());
}

void BioXASMirrorLateralControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateLateral(upstreamLength_, downstreamLength_, lateralMotor_->value(), yawMotor_->value()) );
	}
}

void BioXASMirrorLateralControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( lateralMotor_->isMoving() || yawMotor_->isMoving() );
	}
}

AMAction3* BioXASMirrorLateralControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {

		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

		double yaw = calculateYaw(upstreamLength_, downstreamLength_, yawMotor_->value());

		double lateralDestination = calculateLateralPosition(setpoint, upstreamLength_, downstreamLength_, yaw);
		move->addSubAction(AMActionSupport::buildControlMoveAction(lateralMotor_, lateralDestination));

		double yawDestination = calculateYawPositionFromLateral(setpoint, upstreamLength_, downstreamLength_, lateralDestination);
		move->addSubAction(AMActionSupport::buildControlMoveAction(yawMotor_, yawDestination));

		result = move;
	}

	return result;
}
