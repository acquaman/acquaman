#include "BioXASMirrorMotorControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include <math.h>

BioXASMirrorMotorControl::BioXASMirrorMotorControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, upstreamLength, downstreamLength, parent, description)
{
	// Initialize member variables.

	upstreamInboard_ = 0;
	upstreamOutboard_ = 0;
	downstream_ = 0;
}

BioXASMirrorMotorControl::~BioXASMirrorMotorControl()
{

}

bool BioXASMirrorMotorControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboard_->canMeasure() && upstreamOutboard_->canMeasure() && downstream_->canMeasure() );
	}

	return result;
}

bool BioXASMirrorMotorControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamInboard_->canMove() && upstreamOutboard_->canMove() && downstream_->canMove() );

	return result;
}

bool BioXASMirrorMotorControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamInboard_->canStop() && upstreamOutboard_->canStop() && downstream_->canStop() );

	return result;
}

bool BioXASMirrorMotorControl::validValue(double value) const
{
	bool result = false;

	if (value < maximumValue() && value > minimumValue()) {
		result = true;
	}

	return true;
}

bool BioXASMirrorMotorControl::validSetpoint(double value) const
{
	return validValue(value);
}

void BioXASMirrorMotorControl::setUpstreamInboardControl(BioXASMirrorMotor *newControl)
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

void BioXASMirrorMotorControl::setUpstreamOutboardControl(BioXASMirrorMotor *newControl)
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

void BioXASMirrorMotorControl::setDownstreamControl(BioXASMirrorMotor *newControl)
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

void BioXASMirrorMotorControl::updateConnected()
{
	bool isConnected = (
				upstreamInboard_ && upstreamInboard_->isConnected() &&
				upstreamOutboard_ && upstreamOutboard_->isConnected() &&
				downstream_ && downstream_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASMirrorMotorControl::updateMoving()
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

double BioXASMirrorMotorControl::calculatePitch(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = ((downstreamZ - upstreamInboardZ)*(downstreamY - upstreamOutboardY) - (downstreamZ - upstreamOutboardZ)*(downstreamY - upstreamInboardY));
	double denom = ((downstreamX - upstreamInboardX)*(downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX)*(downstreamY - upstreamInboardY));
	double pitch = atan( numerator / denom ) * 180/M_PI;

	return pitch;
}

double BioXASMirrorMotorControl::calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = (((upstreamInboardX * upstreamOutboardZ) - (upstreamOutboardX * upstreamInboardZ)) * (upstreamInboardX - downstreamX)) + (((downstreamX * upstreamInboardZ) - (upstreamInboardX * downstreamZ)) * (upstreamInboardX - upstreamOutboardX));
	double denom = (upstreamInboardX * (upstreamInboardX - downstreamX) * (upstreamOutboardY - upstreamInboardY)) + (upstreamInboardX + (upstreamInboardX - upstreamOutboardX) * (upstreamInboardY - downstreamY));
	double result = atan(numerator/denom) * 180/M_PI;

	return result;
}

double BioXASMirrorMotorControl::calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double term1 = ((downstreamZ * upstreamOutboardY) - (upstreamOutboardZ * downstreamY)) / (upstreamOutboardY - downstreamY);
	double term2Num = ((upstreamInboardZ - downstreamZ) * (upstreamOutboardY - downstreamY)) - ((downstreamZ - upstreamOutboardZ) * (downstreamY - upstreamInboardY));
	double term2Denom = ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term3 = (downstreamX * (downstreamY - upstreamOutboardY) + downstreamY * (upstreamOutboardX - downstreamX)) / (upstreamOutboardY - downstreamY);

	double height = term1 + (term2Num/term2Denom) * term3;

	return height;
}

double BioXASMirrorMotorControl::calculateUpstreamInboardZ(double upstreamInboardX, double upstreamInboardY, double pitch, double roll, double height)
{
	double term1 = height;
	double term2 = upstreamInboardX * tan(pitch * M_PI/180);
	double term3 = upstreamInboardY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}

double BioXASMirrorMotorControl::calculateUpstreamOutboardZ(double upstreamOutboardX, double upstreamOutboardY, double pitch, double roll, double height)
{
	double term1 = height;
	double term2 = upstreamOutboardX * tan(pitch * M_PI/180);
	double term3 = upstreamOutboardY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}

double BioXASMirrorMotorControl::calculateDownstreamZ(double downstreamX, double downstreamY, double pitch, double roll, double height)
{
	double term1 = height;
	double term2 = downstreamX * tan(pitch * M_PI/180);
	double term3 = downstreamY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}


