#include "BioXASMirrorRollControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASMirrorRollControl::BioXASMirrorRollControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Roll");

	// Current settings.

	updateStates();
}

BioXASMirrorRollControl::~BioXASMirrorRollControl()
{

}

bool BioXASMirrorRollControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboardMotor_->canMeasure() && upstreamOutboardMotor_->canMeasure() && downstreamMotor_->canMeasure() );
	}

	return result;
}

bool BioXASMirrorRollControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboardMotor_->canMove() && upstreamOutboardMotor_->canMove() && downstreamMotor_->canMove() );
	}

	return result;
}

bool BioXASMirrorRollControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboardMotor_->canStop() && upstreamOutboardMotor_->canStop() && downstreamMotor_->canStop() );
	}

	return result;
}

void BioXASMirrorRollControl::updateConnected()
{
	bool isConnected = (
				upstreamInboardMotor_ && upstreamInboardMotor_->isConnected() &&
				upstreamOutboardMotor_ && upstreamOutboardMotor_->isConnected() &&
				downstreamMotor_ && downstreamMotor_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASMirrorRollControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateRoll(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), upstreamInboardMotor_->zPosition(), upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), upstreamOutboardMotor_->zPosition(), downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), downstreamMotor_->zPosition()) );
	}
}

void BioXASMirrorRollControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( upstreamInboardMotor_->isMoving() || upstreamOutboardMotor_->isMoving() || downstreamMotor_->isMoving() );
	}
}

AMAction3* BioXASMirrorRollControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {

		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

		double pitch = calculatePitch(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), upstreamInboardMotor_->zPositionSetpoint(), upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), upstreamOutboardMotor_->zPositionSetpoint(), downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), downstreamMotor_->zPositionSetpoint());
		double height = calculateHeight(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), upstreamInboardMotor_->zPositionSetpoint(), upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), upstreamOutboardMotor_->zPositionSetpoint(), downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), downstreamMotor_->zPositionSetpoint());

		double upstreamInboardDestination = calculateUpstreamInboardPosition(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), pitch, setpoint, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamInboardMotor_, upstreamInboardDestination));

		double upstreamOutboardDestination = calculateUpstreamOutboardPosition(upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), pitch, setpoint, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamOutboardMotor_, upstreamOutboardDestination));

		double downstreamDestination = calculateDownstreamPosition(downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), pitch, setpoint, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(downstreamMotor_, downstreamDestination));

		result = move;
	}

	return result;
}
