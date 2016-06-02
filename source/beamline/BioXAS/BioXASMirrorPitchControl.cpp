#include "BioXASMirrorPitchControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorPitchControl::BioXASMirrorPitchControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Pitch");

	// Current settings.

	updateStates();
}

BioXASMirrorPitchControl::~BioXASMirrorPitchControl()
{

}

bool BioXASMirrorPitchControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboard_->canMeasure() && upstreamOutboard_->canMeasure() && downstream_->canMeasure() );
	}

	return result;
}

bool BioXASMirrorPitchControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboard_->canMove() && upstreamOutboard_->canMove() && downstream_->canMove() );
	}

	return result;
}

bool BioXASMirrorPitchControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboard_->canStop() && upstreamOutboard_->canStop() && downstream_->canStop() );
	}

	return result;
}

void BioXASMirrorPitchControl::updateConnected()
{
	bool isConnected = (
				upstreamInboard_ && upstreamInboard_->isConnected() &&
				upstreamOutboard_ && upstreamOutboard_->isConnected() &&
				downstream_ && downstream_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASMirrorPitchControl::updateValue()
{
	if (isConnected()) {
		setValue( calculatePitch(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPosition(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPosition()) );
	}
}

void BioXASMirrorPitchControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( upstreamInboard_->isMoving() || upstreamOutboard_->isMoving() || downstream_->isMoving() );
	}
}

AMAction3* BioXASMirrorPitchControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

		double roll = calculateRoll(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPositionSetpoint(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPositionSetpoint(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPositionSetpoint());
		double height = calculateHeight(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPositionSetpoint(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPositionSetpoint(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPositionSetpoint());

		double upstreamInboardDestination = calculateUpstreamInboardPosition(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), setpoint, roll, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamInboard_, upstreamInboardDestination));

		double upstreamOutboardDestination = calculateUpstreamOutboardPosition(upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), setpoint, roll, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamOutboard_, upstreamOutboardDestination));

		double downstreamDestination = calculateDownstreamPosition(downstream_->xPosition(), downstream_->yPosition(), setpoint, roll, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(downstream_, downstreamDestination));

		result = move;
	}

	return result;
}
