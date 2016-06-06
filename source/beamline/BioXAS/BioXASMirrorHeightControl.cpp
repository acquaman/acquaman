#include "BioXASMirrorHeightControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorHeightControl::BioXASMirrorHeightControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Height");

	// Current settings.

	updateStates();
}

BioXASMirrorHeightControl::~BioXASMirrorHeightControl()
{

}

bool BioXASMirrorHeightControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboardMotor_->canMeasure() && upstreamOutboardMotor_->canMeasure() && downstreamMotor_->canMeasure() );
	}

	return result;
}

bool BioXASMirrorHeightControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboardMotor_->canMove() && upstreamOutboardMotor_->canMove() && downstreamMotor_->canMove() );
	}

	return result;
}

bool BioXASMirrorHeightControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboardMotor_->canStop() && upstreamOutboardMotor_->canStop() && downstreamMotor_->canStop() );
	}

	return result;
}

void BioXASMirrorHeightControl::updateConnected()
{
	bool isConnected = (
				upstreamInboardMotor_ && upstreamInboardMotor_->isConnected() &&
				upstreamOutboardMotor_ && upstreamOutboardMotor_->isConnected() &&
				downstreamMotor_ && downstreamMotor_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASMirrorHeightControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateHeight(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), upstreamInboardMotor_->zPosition(), upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), upstreamOutboardMotor_->zPosition(), downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), downstreamMotor_->zPosition()) );
	}
}

void BioXASMirrorHeightControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( upstreamInboardMotor_->isMoving() || upstreamOutboardMotor_->isMoving() || downstreamMotor_->isMoving() );
	}
}

AMAction3* BioXASMirrorHeightControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

		double roll = calculateRoll(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), upstreamInboardMotor_->zPositionSetpoint(), upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), upstreamOutboardMotor_->zPositionSetpoint(), downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), downstreamMotor_->zPositionSetpoint());
		double pitch = calculatePitch(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), upstreamInboardMotor_->zPositionSetpoint(), upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), upstreamOutboardMotor_->zPositionSetpoint(), downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), downstreamMotor_->zPositionSetpoint());

		double upstreamInboardDestination = calculateUpstreamInboardPosition(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), pitch, roll, setpoint);
		move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamInboardMotor_, upstreamInboardDestination));

		double upstreamOutboardDestination = calculateUpstreamOutboardPosition(upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), pitch, roll, setpoint);
		move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamOutboardMotor_, upstreamOutboardDestination));

		double downstreamDestination = calculateDownstreamPosition(downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), pitch, roll, setpoint);
		move->addSubAction(AMActionSupport::buildControlMoveAction(downstreamMotor_, downstreamDestination));

		result = move;
	}

	return result;
}
