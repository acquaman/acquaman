#include "BioXASMirrorBendControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorBendControl::BioXASMirrorBendControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, parent, description)
{
	setContextKnownDescription("Bend");
}

BioXASMirrorBendControl::~BioXASMirrorBendControl()
{

}

bool BioXASMirrorBendControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = upstreamBenderMotor_->canMeasure() && downstreamBenderMotor_->canMeasure();
	}

	return result;
}

bool BioXASMirrorBendControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = upstreamBenderMotor_->canMove() && downstreamBenderMotor_->canMove();
	}

	return result;
}

bool BioXASMirrorBendControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = upstreamBenderMotor_->canStop() && downstreamBenderMotor_->canStop();
	}

	return result;
}

void BioXASMirrorBendControl::updateConnected()
{
	bool isConnected = (
				upstreamBenderMotor_ && upstreamBenderMotor_->isConnected() &&
				downstreamBenderMotor_ && downstreamBenderMotor_->isConnected()
				);

	setConnected( isConnected );
}

void BioXASMirrorBendControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateBendRadius(upstreamBenderMotor_->value(), downstreamBenderMotor_->value()) );
	}
}

void BioXASMirrorBendControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( upstreamBenderMotor_->isMoving() || downstreamBenderMotor_->isMoving() );
	}
}

AMAction3* BioXASMirrorBendControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

		double upstreamBendDestination = calculateUpstreamBenderValue(setpoint);
		AMAction3 *moveUpstream = AMActionSupport::buildControlMoveAction(upstreamBenderMotor_, upstreamBendDestination);
		move->addSubAction(moveUpstream);

		double downstreamBendDestination = calculateDownstreamBenderValue(setpoint);
		AMAction3 *moveDownstream = AMActionSupport::buildControlMoveAction(downstreamBenderMotor_, downstreamBendDestination);
		move->addSubAction(moveDownstream);

		result = move;
	}

	return result;
}

double BioXASMirrorBendControl::calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue) const
{
	double radius1 = calculateUpstreamBendRadius(upstreamBenderValue);
	double radius2 = calculateDownstreamBendRadius(downstreamBenderValue);

	double radius = (radius1 + radius2) / 2.0;

	return radius;
}

