#include "BioXASMirrorBendControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorBendControl::BioXASMirrorBendControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, upstreamLength, downstreamLength, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Bend");

	// Initialize member variables.

	upstreamBender_ = 0;
	downstreamBender_ = 0;
}

BioXASMirrorBendControl::~BioXASMirrorBendControl()
{

}

bool BioXASMirrorBendControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = upstreamBender_->canMeasure() && downstreamBender_->canMeasure();
	}

	return result;
}

bool BioXASMirrorBendControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = upstreamBender_->canMove() && downstreamBender_->canMove();
	}

	return result;
}

bool BioXASMirrorBendControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = upstreamBender_->canStop() && downstreamBender_->canStop();
	}

	return result;
}

bool BioXASMirrorBendControl::validValue(double value) const
{
	bool isValid = false;

	if (value >= minimumValue() && value <= maximumValue())
		isValid = true;

	return isValid;
}

bool BioXASMirrorBendControl::validSetpoint(double value) const
{
	return validValue(value);
}

void BioXASMirrorBendControl::setUpstreamBenderControl(AMControl *newControl)
{
	if (upstreamBender_ != newControl) {

		if (upstreamBender_)
			removeChildControl(upstreamBender_);

		upstreamBender_ = newControl;

		if (upstreamBender_)
			addChildControl(upstreamBender_);

		emit upstreamBenderChanged(upstreamBender_);
	}
}

void BioXASMirrorBendControl::setDownstreamBenderControl(AMControl *newControl)
{
	if (downstreamBender_ != newControl) {

		if (downstreamBender_)
			removeChildControl(downstreamBender_);

		downstreamBender_ = newControl;

		if (downstreamBender_)
			addChildControl(downstreamBender_);

		emit downstreamBenderChanged(downstreamBender_);
	}
}

void BioXASMirrorBendControl::updateConnected()
{
	bool isConnected = (
				upstreamBender_ && upstreamBender_->isConnected() &&
				downstreamBender_ && downstreamBender_->isConnected()
				);

	setConnected( isConnected );
}

void BioXASMirrorBendControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateBendRadius(upstreamBender_->value(), downstreamBender_->value()) );
	}
}

void BioXASMirrorBendControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( upstreamBender_->isMoving() || downstreamBender_->isMoving() );
	}
}

AMAction3* BioXASMirrorBendControl::createMoveActionIteration(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

		double upstreamBendDestination = calculateUpstreamBenderValue(setpoint);
		AMAction3 *moveUpstream = AMActionSupport::buildControlMoveAction(upstreamBender_, upstreamBendDestination);
		move->addSubAction(moveUpstream);

		double downstreamBendDestination = calculateDownstreamBenderValue(setpoint);
		AMAction3 *moveDownstream = AMActionSupport::buildControlMoveAction(downstreamBender_, downstreamBendDestination);
		move->addSubAction(moveDownstream);

		result = move;
	}

	return result;
}

