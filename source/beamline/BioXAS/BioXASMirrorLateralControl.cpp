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
		result = ( stripeSelect_->canMeasure() && yaw_->canMeasure() );
	}

	return result;
}

bool BioXASMirrorLateralControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = (stripeSelect_->canMove() && yaw_->canMove());
	}

	return result;
}

bool BioXASMirrorLateralControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = (stripeSelect_->canStop() && stripeSelect_->canStop());
	}

	return result;
}

void BioXASMirrorLateralControl::updateConnected()
{
	bool isConnected = (
				stripeSelect_ && stripeSelect_->isConnected() &&
				yaw_ && yaw_->isConnected()
				);

	setConnected(isConnected && validLengths());
}

void BioXASMirrorLateralControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateLateral(upstreamLength_, downstreamLength_, stripeSelect_->value(), yaw_->value()) );
	}
}

void BioXASMirrorLateralControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( stripeSelect_->isMoving() || yaw_->isMoving() );
	}
}

AMAction3* BioXASMirrorLateralControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {

		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Sequential);

		double lateralDestination = calculateLateralPosition(setpoint, upstreamLength_, downstreamLength_, yaw_->value());
		move->addSubAction(AMActionSupport::buildControlMoveAction(stripeSelect_, lateralDestination));

		double yawDestination = calculateYawPosition(setpoint, upstreamLength_, downstreamLength_, stripeSelect_->value());
		move->addSubAction(AMActionSupport::buildControlMoveAction(yaw_, yawDestination));

		result = move;
	}

	return result;
}

double BioXASMirrorLateralControl::calculateYawPosition(double lateral, double upstreamLength, double downstreamLength, double lateralPosition)
{
	Q_UNUSED(downstreamLength)

	double yawControlValue = atan((lateralPosition - lateral) / upstreamLength) * 180/M_PI;
	double result = BioXASMirrorControl::calculateYawPosition(yawControlValue, upstreamLength, downstreamLength);
	return result;
}
