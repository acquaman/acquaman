#include "BioXASMirrorLateralControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorLateralControl::BioXASMirrorLateralControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, upstreamLength, downstreamLength, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Lateral");

	// Initialize member variables.

	stripeSelect_ = 0;
	yaw_ = 0;
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

void BioXASMirrorLateralControl::setStripeSelectionControl(AMControl *newControl)
{
	if (stripeSelect_ != newControl) {

		if (stripeSelect_)
			removeChildControl(stripeSelect_);

		stripeSelect_ = newControl;

		if (stripeSelect_)
			addChildControl(stripeSelect_);

		emit stripeSelectionControlChanged(stripeSelect_);
	}
}

void BioXASMirrorLateralControl::setYawControl(AMControl *newControl)
{
	if (yaw_ != newControl) {

		if (yaw_)
			removeChildControl(yaw_);

		yaw_ = newControl;

		if (yaw_)
			addChildControl(yaw_);

		emit yawControlChanged(yaw_);
	}
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
		setValue( calculateLateralDisplacement(upstreamLength_, downstreamLength_, stripeSelect_->value(), yaw_->value()) );
	}
}

void BioXASMirrorLateralControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( stripeSelect_->isMoving() || yaw_->isMoving() );
	}
}

AMAction3* BioXASMirrorLateralControl::createMoveActionIteration(double setpoint)
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

double BioXASMirrorLateralControl::calculateLateralPosition(double lateralDisplacement, double upstreamLength, double downstreamLength, double yawPosition)
{
	Q_UNUSED(downstreamLength)

	double result = lateralDisplacement + (upstreamLength * tan(yawPosition) * 180/M_PI);
	return result;
}

double BioXASMirrorLateralControl::calculateYawPosition(double lateralDisplacement, double upstreamLength, double downstreamLength, double lateralPosition)
{
	Q_UNUSED(downstreamLength)

	double lateralDifference = lateralPosition - lateralDisplacement;
	double result = atan(lateralDifference / upstreamLength) * 180/M_PI;
	return result;
}

double BioXASMirrorLateralControl::calculateLateralDisplacement(double upstreamLength, double downstreamLength, double lateralPosition, double yawPosition)
{
	Q_UNUSED(downstreamLength)

	double yawDisplacement = (-upstreamLength) * tan(yawPosition * M_PI/180);
	double result = lateralPosition + yawDisplacement;

	return result;
}
