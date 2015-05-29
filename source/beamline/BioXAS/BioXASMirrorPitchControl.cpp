#include "BioXASMirrorPitchControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#include <QDebug>

BioXASMirrorPitchControl::BioXASMirrorPitchControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent, const QString &description) :
	BioXASMirrorMotorControl(name, units, upstreamLength, downstreamLength, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Pitch");

	// Current settings.

	updateStates();
}

BioXASMirrorPitchControl::~BioXASMirrorPitchControl()
{

}

void BioXASMirrorPitchControl::updateValue()
{
	if (isConnected()) {
		setValue( calculatePitch(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPosition(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPosition()) );
	}
}

AMAction3* BioXASMirrorPitchControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *move = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Sequential);

		double roll = calculateRoll(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPosition(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPosition());
		double height = calculateHeight(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPosition(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPosition());

		double upstreamInboardDestination = calculateUpstreamInboardZ(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), setpoint, roll, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamInboard_, upstreamInboardDestination));

		double upstreamOutboardDestination = calculateUpstreamOutboardZ(upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), setpoint, roll, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamOutboard_, upstreamOutboardDestination));

		double downstreamDestination = calculateDownstreamZ(downstream_->xPosition(), downstream_->yPosition(), setpoint, roll, height);
		move->addSubAction(AMActionSupport::buildControlMoveAction(downstream_, downstreamDestination));

		result = move;
	}

	return result;
}
