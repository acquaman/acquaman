#include "BioXASMirrorHeightControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMirrorHeightControl::BioXASMirrorHeightControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	BioXASMirrorPseudoMotorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	setContextKnownDescription("Height");

	// Current settings.

	updateStates();
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

double BioXASMirrorHeightControl::calculateUpstreamInboardZ(double height, double upstreamInboardX, double upstreamInboardY, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double term1 = ((downstreamZ - upstreamOutboardZ) *(downstreamY - upstreamInboardY));
	double term2 = ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term3 = (downstreamX * (downstreamY - upstreamOutboardY) + downstreamY * (upstreamOutboardX - downstreamX)) / (upstreamOutboardY - downstreamY);
	double term4 = (height - ((downstreamZ * upstreamOutboardY) - (upstreamOutboardZ * downstreamY)) / (upstreamOutboardY - downstreamY));
	double term5 = (upstreamOutboardY - downstreamY);

	double result = (term1 + (term2/term3) * (term4)) / (term5);

	return result;
}

double BioXASMirrorHeightControl::calculateUpstreamOutboardZ(double height, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double downstreamX, double downstreamY, double downstreamZ)
{
	double term1 = height / (downstreamX * (downstreamY - upstreamOutboardY) + downstreamY * (upstreamOutboardX - downstreamX)) / (upstreamOutboardY - downstreamY);
	double term2 = ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term3 = (upstreamInboardZ - downstreamZ) * (upstreamOutboardY - downstreamY);
	double term4 = downstreamZ * upstreamOutboardY * ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term5 = downstreamZ * downstreamY * upstreamInboardY;
	double term6 = downstreamY - upstreamInboardY - downstreamY * ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));

	double result = ((term1 * term2) - term3 - term4 + term5) / term6;
	return result;
}

double BioXASMirrorHeightControl::calculateDownstreamZ(double height, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY)
{
	double term1 = height / (downstreamX * (downstreamY - upstreamOutboardY) + downstreamY * (upstreamOutboardX - downstreamX)) / (upstreamOutboardY - downstreamY);
	double term2 = (upstreamOutboardY - downstreamY) * ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term3 = upstreamOutboardZ * downstreamY * ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term4 = upstreamInboardZ * pow(upstreamOutboardY - downstreamY, 2);
	double term5 = upstreamOutboardZ * (downstreamY - upstreamInboardY) * (upstreamOutboardY - downstreamY);
	double term6 = upstreamOutboardY * ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY)) - pow(upstreamOutboardY - downstreamY, 2) - (downstreamY - upstreamInboardY) * (upstreamOutboardY - downstreamY);

	double result = (term1 * term2 + term3 - term4 - term5) / term6;

	return result;
}

double BioXASMirrorHeightControl::calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double term1 = ((downstreamZ * upstreamOutboardY) - (upstreamOutboardZ * downstreamY)) / (upstreamOutboardY - downstreamY);
	double term2Num = ((upstreamInboardZ - downstreamZ) * (upstreamOutboardY - downstreamY)) - ((downstreamZ - upstreamOutboardZ) * (downstreamY - upstreamInboardY));
	double term2Denom = ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term3 = (downstreamX * (downstreamY - upstreamOutboardY) + downstreamY * (upstreamOutboardX - downstreamX)) / (upstreamOutboardY - downstreamY);

	double height = term1 + (term2Num/term2Denom) * term3;

	return height;
}
