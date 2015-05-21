#include "BioXASMirrorRollControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASMirrorRollControl::BioXASMirrorRollControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorPseudoMotorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Roll");

	// Current settings.

	updateConnected();
	updateValue();
	updateIsMoving();
}

BioXASMirrorRollControl::~BioXASMirrorRollControl()
{

}

void BioXASMirrorRollControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateRoll(upstreamInboard_->xPosition(), upstreamInboard_->yPosition(), upstreamInboard_->zPosition(), upstreamOutboard_->xPosition(), upstreamOutboard_->yPosition(), upstreamOutboard_->zPosition(), downstream_->xPosition(), downstream_->yPosition(), downstream_->zPosition()) );
	}
}

AMAction3* BioXASMirrorRollControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3("Move roll", "Move roll"), AMListAction3::Sequential);

		for (int i = 0; i < 5; i++) {
			AMListAction3 *move = new AMListAction3(new AMListActionInfo3(QString("Move roll attempt %1").arg(i), QString("Move roll attempt %1").arg(i)), AMListAction3::Sequential);

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

double BioXASMirrorRollControl::calculateUpstreamInboardZ(double roll, double upstreamInboardX, double upstreamInboardY, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = ((upstreamInboardX * upstreamOutboardZ) * (upstreamInboardX - downstreamX)) - ((upstreamInboardX * downstreamZ) * (upstreamInboardX - upstreamOutboardX)) - ((upstreamInboardX * (upstreamInboardX - downstreamX) * (upstreamOutboardY - upstreamInboardY)) + (upstreamInboardX * (upstreamInboardX - upstreamOutboardX) * (upstreamInboardY - downstreamY))) * tan(roll * M_PI/180);
	double denom = (upstreamOutboardX * (upstreamInboardX - downstreamX)) + (downstreamX * (upstreamInboardX - upstreamOutboardX));
	double result = numerator / denom;

	return result;
}

double BioXASMirrorRollControl::calculateUpstreamOutboardZ(double roll, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double downstreamX, double downstreamY, double downstreamZ)
{
	double term1Num = ((upstreamInboardX * (upstreamInboardX - downstreamX) * (upstreamOutboardY - upstreamInboardY)) + (upstreamInboardX * (upstreamInboardX - upstreamOutboardX) * (upstreamInboardY - downstreamY))) * tan(roll * M_PI/180) - (((downstreamX * upstreamInboardZ) - (upstreamInboardX * downstreamZ)) * (upstreamInboardX - upstreamOutboardX));
	double term1Denom = upstreamInboardX * (upstreamInboardX - downstreamX);
	double term2Num = (upstreamOutboardX * upstreamInboardZ);
	double term2Denom = upstreamInboardX;
	double result = term1Num / term1Denom + term2Num / term2Denom;

	return result;
}

double BioXASMirrorRollControl::calculateDownstreamZ(double roll, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY)
{
	double term1Num = (downstreamX * upstreamInboardZ);
	double term1Denom = (upstreamInboardX);
	double term2Num = ((upstreamInboardX * (upstreamInboardX - downstreamX) * (upstreamOutboardY - upstreamInboardY)) + (upstreamInboardX * (upstreamInboardX - upstreamOutboardX) * (upstreamInboardY - downstreamY))) * tan(roll * M_PI/180) - (((upstreamInboardX * upstreamOutboardZ) - (upstreamOutboardX * upstreamInboardZ)) * (upstreamInboardX - downstreamX));
	double term2Denom = (upstreamInboardX - upstreamOutboardX) * upstreamInboardX;
	double result = term1Num / term1Denom - term2Num / term2Denom;

	return result;
}

double BioXASMirrorRollControl::calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = (((upstreamInboardX * upstreamOutboardZ) - (upstreamOutboardX * upstreamInboardZ)) * (upstreamInboardX - downstreamX)) + (((downstreamX * upstreamInboardZ) - (upstreamInboardX * downstreamZ)) * (upstreamInboardX - upstreamOutboardX));
	double denom = (upstreamInboardX * (upstreamInboardX - downstreamX) * (upstreamOutboardY - upstreamInboardY)) + (upstreamInboardX + (upstreamInboardX - upstreamOutboardX) * (upstreamInboardY - downstreamY));
	double result = atan(numerator/denom) * 180/M_PI;

	return result;
}


