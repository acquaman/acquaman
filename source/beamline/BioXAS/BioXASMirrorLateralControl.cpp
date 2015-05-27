#include "BioXASMirrorLateralControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorLateralControl::BioXASMirrorLateralControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
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

	upstreamLength_ = -1;
	downstreamLength_ = -1;
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
		result = ( (upstreamLength_ != -1) && (downstreamLength_ != -1) && stripeSelect_->canMeasure() && yaw_->canMeasure() );
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

void BioXASMirrorLateralControl::setUpstreamLength(double newLength)
{
	if (upstreamLength_ != newLength) {
		upstreamLength_ = newLength;
		emit upstreamLengthChanged(upstreamLength_);
	}
}

void BioXASMirrorLateralControl::setDownstreamLength(double newLength)
{
	if (downstreamLength_ != newLength) {
		downstreamLength_ = newLength;
		emit downstreamLengthChanged(downstreamLength_);
	}
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

	setConnected(isConnected);
}

void BioXASMirrorLateralControl::updateValue()
{
	if (isConnected() && upstreamLength_ != -1 && downstreamLength_ != -1) {
		setValue( calculateLateralDisplacement(upstreamLength_, downstreamLength_, stripeSelect_->value(), yaw_->value()) );
	}
}

void BioXASMirrorLateralControl::updateMoving()
{
	if (isConnected() && upstreamLength_ != -1 && downstreamLength_ != -1) {
		setIsMoving( stripeSelect_->isMoving() || yaw_->isMoving() );
	}
}

AMAction3* BioXASMirrorLateralControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected() && upstreamLength_ != -1 && downstreamLength_ != -1) {
		AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3("Move laterally", "Move laterally"), AMListAction3::Sequential);

		for (int i = 0; i < 5; i++) {
			AMListAction3 *move = new AMListAction3(new AMListActionInfo3(QString("Move laterally attempt %1").arg(i), QString("Move laterally attempt %1").arg(i)), AMListAction3::Sequential);

			double lateralDestination = calculateLateralPosition(setpoint, upstreamLength_, downstreamLength_, yaw_->value());
			move->addSubAction(AMActionSupport::buildControlMoveAction(stripeSelect_, lateralDestination));

			double yawDestination = calculateYawPosition(setpoint, upstreamLength_, downstreamLength_, stripeSelect_->value());
			move->addSubAction(AMActionSupport::buildControlMoveAction(yaw_, yawDestination));

			moveAction->addSubAction(move);
		}

		result = moveAction;
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
