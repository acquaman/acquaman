#include "BioXASMirrorControl.h"
#include "actions3/AMListAction3.h"

BioXASMirrorControl::BioXASMirrorControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	AMPseudoMotorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = -1000000;
	maximumValue_ = 1000000;

	setAllowsMovesWhileMoving(false);
	setTolerance(0.0005);

	// Initialize member variables.

	upstreamLength_ = 0;
	downstreamLength_ = 0;
	upstreamInboard_ = 0;
	upstreamOutboard_ = 0;
	downstream_ = 0;
	stripeSelect_ = 0;
	yaw_ = 0;
	upstreamBender_ = 0;
	downstreamBender_ = 0;
}

BioXASMirrorControl::~BioXASMirrorControl()
{

}

bool BioXASMirrorControl::validLengths() const
{
	bool valid = ( upstreamLength_ != -1 && downstreamLength_ != -1 );
	return valid;
}

void BioXASMirrorControl::setUpstreamLength(double newValue)
{
	if (upstreamLength_ != newValue) {
		upstreamLength_ = newValue;

		emit upstreamLengthChanged(upstreamLength_);
	}
}

void BioXASMirrorControl::setDownstreamLength(double newValue)
{
	if (downstreamLength_ != newValue) {
		downstreamLength_ = newValue;

		emit downstreamLengthChanged(downstreamLength_);
	}
}

void BioXASMirrorControl::setUpstreamInboardMotor(BioXASMirrorMotor *newControl)
{
	if (upstreamInboard_ != newControl) {

		if (upstreamInboard_)
			removeChildControl(upstreamInboard_);

		upstreamInboard_ = newControl;

		if (upstreamInboard_)
			addChildControl(upstreamInboard_);

		emit upstreamInboardMotorChanged(upstreamInboard_);

		updateStates();
	}
}

void BioXASMirrorControl::setUpstreamOutboardMotor(BioXASMirrorMotor *newControl)
{
	if (upstreamOutboard_ != newControl) {

		if (upstreamOutboard_)
			removeChildControl(upstreamOutboard_);

		upstreamOutboard_ = newControl;

		if (upstreamOutboard_)
			addChildControl(upstreamOutboard_);

		emit upstreamOutboardMotorChanged(upstreamOutboard_);

		updateStates();
	}
}

void BioXASMirrorControl::setDownstreamMotor(BioXASMirrorMotor *newControl)
{
	if (downstream_ != newControl) {

		if (downstream_)
			removeChildControl(downstream_);

		downstream_ = newControl;

		if (downstream_)
			addChildControl(downstream_);

		emit downstreamMotorChanged(downstream_);

		updateStates();
	}
}

void BioXASMirrorControl::setStripeSelectionMotor(AMControl *newControl)
{
	if (stripeSelect_ != newControl) {

		if (stripeSelect_)
			removeChildControl(stripeSelect_);

		stripeSelect_ = newControl;

		if (stripeSelect_)
			addChildControl(stripeSelect_);

		emit stripeSelectionMotorChanged(stripeSelect_);

		updateStates();
	}
}

void BioXASMirrorControl::setYawMotor(AMControl *newControl)
{
	if (yaw_ != newControl) {

		if (yaw_)
			removeChildControl(yaw_);

		yaw_ = newControl;

		if (yaw_)
			addChildControl(yaw_);

		emit yawMotorChanged(yaw_);

		updateStates();
	}
}

void BioXASMirrorControl::setUpstreamBenderMotor(AMControl *newControl)
{
	if (upstreamBender_ != newControl) {

		if (upstreamBender_)
			removeChildControl(upstreamBender_);

		upstreamBender_ = newControl;

		if (upstreamBender_)
			addChildControl(upstreamBender_);

		emit upstreamBenderMotorChanged(upstreamBender_);

		updateStates();
	}
}

void BioXASMirrorControl::setDownstreamBenderMotor(AMControl *newControl)
{
	if (downstreamBender_ != newControl) {

		if (downstreamBender_)
			removeChildControl(downstreamBender_);

		downstreamBender_ = newControl;

		if (downstreamBender_)
			addChildControl(downstreamBender_);

		emit downstreamBenderMotorChanged(downstreamBender_);

		updateStates();
	}
}

double BioXASMirrorControl::calculatePitch(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = ((downstreamZ - upstreamInboardZ)*(downstreamY - upstreamOutboardY) - (downstreamZ - upstreamOutboardZ)*(downstreamY - upstreamInboardY));
	double denom = ((downstreamX - upstreamInboardX)*(downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX)*(downstreamY - upstreamInboardY));
	double pitch = atan( numerator / denom ) * 180/M_PI;

	return pitch;
}

double BioXASMirrorControl::calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double numerator = (((upstreamInboardX * upstreamOutboardZ) - (upstreamOutboardX * upstreamInboardZ)) * (upstreamInboardX - downstreamX)) + (((downstreamX * upstreamInboardZ) - (upstreamInboardX * downstreamZ)) * (upstreamInboardX - upstreamOutboardX));
	double denom = (upstreamInboardX * (upstreamInboardX - downstreamX) * (upstreamOutboardY - upstreamInboardY)) + (upstreamInboardX + (upstreamInboardX - upstreamOutboardX) * (upstreamInboardY - downstreamY));
	double result = atan(numerator/denom) * 180/M_PI;

	return result;
}

double BioXASMirrorControl::calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ)
{
	double term1 = ((downstreamZ * upstreamOutboardY) - (upstreamOutboardZ * downstreamY)) / (upstreamOutboardY - downstreamY);
	double term2Num = ((upstreamInboardZ - downstreamZ) * (upstreamOutboardY - downstreamY)) - ((downstreamZ - upstreamOutboardZ) * (downstreamY - upstreamInboardY));
	double term2Denom = ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term3 = (downstreamX * (downstreamY - upstreamOutboardY) + downstreamY * (upstreamOutboardX - downstreamX)) / (upstreamOutboardY - downstreamY);

	double height = term1 + (term2Num/term2Denom) * term3;

	return height;
}

double BioXASMirrorControl::calculateLateral(double upstreamLength, double downstreamLength, double lateralPosition, double yawPosition)
{
	Q_UNUSED(downstreamLength)

	double yawControlValue = calculateYaw(upstreamLength, downstreamLength, yawPosition);
	double result = lateralPosition - upstreamLength * tan(yawControlValue * M_PI/180);

	return result;
}

double BioXASMirrorControl::calculateYaw(double upstreamLength, double downstreamLength, double yawPosition)
{
	double result = atan(yawPosition / (downstreamLength - upstreamLength)) * 180/M_PI;
	return result;
}

double BioXASMirrorControl::calculateUpstreamInboardPosition(double upstreamInboardX, double upstreamInboardY, double pitch, double roll, double height)
{
	double term1 = height;
	double term2 = upstreamInboardX * tan(pitch * M_PI/180);
	double term3 = upstreamInboardY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}

double BioXASMirrorControl::calculateUpstreamOutboardPosition(double upstreamOutboardX, double upstreamOutboardY, double pitch, double roll, double height)
{
	double term1 = height;
	double term2 = upstreamOutboardX * tan(pitch * M_PI/180);
	double term3 = upstreamOutboardY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}

double BioXASMirrorControl::calculateDownstreamPosition(double downstreamX, double downstreamY, double pitch, double roll, double height)
{
	double term1 = height;
	double term2 = downstreamX * tan(pitch * M_PI/180);
	double term3 = downstreamY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}

double BioXASMirrorControl::calculateLateralPosition(double lateral, double upstreamLength, double downstreamLength, double yaw)
{
	Q_UNUSED(downstreamLength)

	double result = lateral + (upstreamLength * tan(yaw * M_PI/180));
	return result;
}

double BioXASMirrorControl::calculateYawPosition(double yaw, double upstreamLength, double downstreamLength)
{
	double result = (downstreamLength - upstreamLength) * tan(yaw * M_PI/180);
	return result;
}

double BioXASMirrorControl::calculateYawPositionFromLateral(double lateral, double upstreamLength, double downstreamLength, double lateralPosition)
{
	Q_UNUSED(downstreamLength)

	double yawControlValue = atan((lateralPosition - lateral) / upstreamLength) * 180/M_PI;
	double result = BioXASMirrorControl::calculateYawPosition(yawControlValue, upstreamLength, downstreamLength);
	return result;
}
