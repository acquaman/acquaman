#include "BioXASMirrorPitchControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include <math.h>

BioXASMirrorPitchControl::BioXASMirrorPitchControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	AMPseudoMotorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = -1000000;
	maximumValue_ = 1000000;

	setAllowsMovesWhileMoving(false);
	setTolerance(0.05);
	setContextKnownDescription("Pitch");

	// Initialize member variables.

	upstreamInboard_ = 0;
	upstreamOutboard_ = 0;
	downstream_ = 0;

	// Current settings.

	updateConnected();
	updateValue();
	updateIsMoving();
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

	if (isConnected())
		result = ( upstreamInboard_->canMove() && upstreamOutboard_->canMove() && downstream_->canMove() );

	return result;
}

bool BioXASMirrorPitchControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamInboard_->canStop() && upstreamOutboard_->canStop() && downstream_->canStop() );

	return result;
}

bool BioXASMirrorPitchControl::validValue(double value) const
{
	bool result = false;

	if (value < maximumValue() && value > minimumValue()) {
		result = true;
	}

	return true;
}

bool BioXASMirrorPitchControl::validSetpoint(double value) const
{
	return validValue(value);
}

void BioXASMirrorPitchControl::setUpstreamInboardControl(BioXASMirrorMotor *newControl)
{
	if (upstreamInboard_ != newControl) {

		if (upstreamInboard_)
			removeChildControl(upstreamInboard_);

		upstreamInboard_ = newControl;

		if (upstreamInboard_)
			addChildControl(upstreamInboard_);

		updateStates();
	}
}

void BioXASMirrorPitchControl::setUpstreamOutboardControl(BioXASMirrorMotor *newControl)
{
	if (upstreamOutboard_ != newControl) {

		if (upstreamOutboard_)
			removeChildControl(upstreamOutboard_);

		upstreamOutboard_ = newControl;

		if (upstreamOutboard_)
			addChildControl(upstreamOutboard_);

		updateStates();
	}
}

void BioXASMirrorPitchControl::setDownstreamControl(BioXASMirrorMotor *newControl)
{
	if (downstream_ != newControl) {

		if (downstream_)
			removeChildControl(downstream_);

		downstream_ = newControl;

		if (downstream_)
			addChildControl(downstream_);

		updateStates();
	}
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

void BioXASMirrorPitchControl::updateIsMoving()
{
	if (isConnected()) {
		bool isMoving = (
					upstreamInboard_->isMoving() ||
					upstreamOutboard_->isMoving() ||
					downstream_->isMoving()
					);

		setIsMoving(isMoving);
	}
}

AMAction3* BioXASMirrorPitchControl::createMoveAction(double setpoint)
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

double BioXASMirrorPitchControl::calculateUpstreamInboardZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = (downstreamX - upstreamInboardX) * (downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY) * tan(pitch) + (downstreamZ - upstreamOutboardZ) * (downstreamY - upstreamInboardY);
	double denom = downstreamY - upstreamOutboardY;
	double result = downstreamZ - numerator / denom;

	return result;
}

double BioXASMirrorPitchControl::calculateUpstreamOutboardZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = (downstreamZ - upstreamInboardZ) * (downstreamY - upstreamOutboardY) - ((downstreamX - upstreamInboardX) * (downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY)) * tan(pitch);
	double denom = downstreamY - upstreamInboardY;
	double result = downstreamZ - numerator/denom;

	return result;
}

double BioXASMirrorPitchControl::calculateDownstreamZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY)
{
	double numerator = ((upstreamInboardZ * downstreamY) - (upstreamInboardZ * upstreamOutboardY) - (upstreamOutboardZ * downstreamY) + (upstreamOutboardZ * upstreamInboardY) + ((downstreamX - upstreamInboardX) * (downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY)) * tan(pitch));
	double denom = upstreamInboardY - upstreamOutboardY;
	double result = numerator / denom;

	return result;
}

double BioXASMirrorPitchControl::calculatePitch(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = ((downstreamZ - upstreamInboardZ)*(downstreamY - upstreamOutboardY) - (downstreamZ - upstreamOutboardZ)*(downstreamY - upstreamInboardY));
	double denom = ((downstreamX - upstreamInboardX)*(downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX)*(downstreamY - upstreamInboardY));
	double pitch = atan( numerator / denom ) * 180/M_PI;

	return pitch;
}
