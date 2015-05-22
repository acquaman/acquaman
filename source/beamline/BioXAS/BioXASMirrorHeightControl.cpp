#include "BioXASMirrorHeightControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorHeightControl::BioXASMirrorHeightControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorPseudoMotorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Height");

	// Current settings.

	updateConnected();
	updateValue();
	updateIsMoving();
}

BioXASMirrorHeightControl::~BioXASMirrorHeightControl()
{

}

void BioXASMirrorHeightControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateHeight(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPosition(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPosition()) );
	}
}

AMAction3* BioXASMirrorHeightControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3("Move pitch", "Move pitch"), AMListAction3::Sequential);

		for (int i = 0; i < 5; i++) {
			AMListAction3 *move = new AMListAction3(new AMListActionInfo3(QString("Move pitch attempt %1").arg(i), QString("Move pitch attempt %1").arg(i)), AMListAction3::Sequential);

			double upstreamInboardDestination = calculateUpstreamInboardZ(setpoint, upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPosition(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPosition());
			move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamInboard_, upstreamInboardDestination));

			double upstreamOutboardDestination = calculateUpstreamOutboardZ(setpoint, upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPosition());
			move->addSubAction(AMActionSupport::buildControlMoveAction(upstreamOutboard_, upstreamOutboardDestination));

			double downstreamDestination = calculateDownstreamZ(setpoint, upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPosition(), downstream_->xPosition(), downstream_->yPosition());
			move->addSubAction(AMActionSupport::buildControlMoveAction(downstream_, downstreamDestination));

			moveAction->addSubAction(move);
		}

		result = moveAction;
	}

	return result;
}

double BioXASMirrorHeightControl::calculateUpstreamInboardZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = (downstreamX - upstreamInboardX) * (downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY) * tan(pitch * M_PI/180) + (downstreamZ - upstreamOutboardZ) * (downstreamY - upstreamInboardY);
	double denom = downstreamY - upstreamOutboardY;
	double result = downstreamZ - numerator / denom;

	return result;
}

double BioXASMirrorHeightControl::calculateUpstreamOutboardZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = (downstreamZ - upstreamInboardZ) * (downstreamY - upstreamOutboardY) - ((downstreamX - upstreamInboardX) * (downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY)) * tan(pitch * M_PI/180);
	double denom = downstreamY - upstreamInboardY;
	double result = downstreamZ - numerator/denom;

	return result;
}

double BioXASMirrorHeightControl::calculateDownstreamZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY)
{
	double numerator = ((upstreamInboardZ * downstreamY) - (upstreamInboardZ * upstreamOutboardY) - (upstreamOutboardZ * downstreamY) + (upstreamOutboardZ * upstreamInboardY) + ((downstreamX - upstreamInboardX) * (downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY)) * tan(pitch * M_PI/180));
	double denom = upstreamInboardY - upstreamOutboardY;
	double result = numerator / denom;

	return result;
}

double BioXASMirrorHeightControl::calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = ((downstreamZ - upstreamInboardZ)*(downstreamY - upstreamOutboardY) - (downstreamZ - upstreamOutboardZ)*(downstreamY - upstreamInboardY));
	double denom = ((downstreamX - upstreamInboardX)*(downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX)*(downstreamY - upstreamInboardY));
	double pitch = atan( numerator / denom ) * 180/M_PI;

	return pitch;
}
