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

	upstreamInboardMotor_ = 0;
	upstreamOutboardMotor_ = 0;
	downstreamMotor_ = 0;
	yawMotor_ = 0;
	lateralMotor_ = 0;
	upstreamBenderMotor_ = 0;
	downstreamBenderMotor_ = 0;
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
	if (upstreamInboardMotor_ != newControl) {

		if (upstreamInboardMotor_)
			removeChildControl(upstreamInboardMotor_);

		upstreamInboardMotor_ = newControl;

		if (upstreamInboardMotor_)
			addChildControl(upstreamInboardMotor_);

		emit upstreamInboardMotorChanged(upstreamInboardMotor_);

		updateStates();
	}
}

void BioXASMirrorControl::setUpstreamOutboardMotor(BioXASMirrorMotor *newControl)
{
	if (upstreamOutboardMotor_ != newControl) {

		if (upstreamOutboardMotor_)
			removeChildControl(upstreamOutboardMotor_);

		upstreamOutboardMotor_ = newControl;

		if (upstreamOutboardMotor_)
			addChildControl(upstreamOutboardMotor_);

		emit upstreamOutboardMotorChanged(upstreamOutboardMotor_);

		updateStates();
	}
}

void BioXASMirrorControl::setDownstreamMotor(BioXASMirrorMotor *newControl)
{
	if (downstreamMotor_ != newControl) {

		if (downstreamMotor_)
			removeChildControl(downstreamMotor_);

		downstreamMotor_ = newControl;

		if (downstreamMotor_)
			addChildControl(downstreamMotor_);

		emit downstreamMotorChanged(downstreamMotor_);

		updateStates();
	}
}

void BioXASMirrorControl::setYawMotor(AMControl *newControl)
{
	if (yawMotor_ != newControl) {

		if (yawMotor_)
			removeChildControl(yawMotor_);

		yawMotor_ = newControl;

		if (yawMotor_)
			addChildControl(yawMotor_);

		emit yawMotorChanged(yawMotor_);

		updateStates();
	}
}

void BioXASMirrorControl::setLateralMotor(AMControl *newControl)
{
	if (lateralMotor_ != newControl) {

		if (lateralMotor_)
			removeChildControl(lateralMotor_);

		lateralMotor_ = newControl;

		if (lateralMotor_)
			addChildControl(lateralMotor_);

		emit lateralMotorChanged(lateralMotor_);

		updateStates();
	}
}

void BioXASMirrorControl::setUpstreamBenderMotor(AMControl *newControl)
{
	if (upstreamBenderMotor_ != newControl) {

		if (upstreamBenderMotor_)
			removeChildControl(upstreamBenderMotor_);

		upstreamBenderMotor_ = newControl;

		if (upstreamBenderMotor_)
			addChildControl(upstreamBenderMotor_);

		emit upstreamBenderMotorChanged(upstreamBenderMotor_);

		updateStates();
	}
}

void BioXASMirrorControl::setDownstreamBenderMotor(AMControl *newControl)
{
	if (downstreamBenderMotor_ != newControl) {

		if (downstreamBenderMotor_)
			removeChildControl(downstreamBenderMotor_);

		downstreamBenderMotor_ = newControl;

		if (downstreamBenderMotor_)
			addChildControl(downstreamBenderMotor_);

		emit downstreamBenderMotorChanged(downstreamBenderMotor_);

		updateStates();
	}
}

double BioXASMirrorControl::calculatePitch(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ) const
{
	double numerator = ((downstreamZ - upstreamInboardZ)*(downstreamY - upstreamOutboardY) - (downstreamZ - upstreamOutboardZ)*(downstreamY - upstreamInboardY));
	double denom = ((downstreamX - upstreamInboardX)*(downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX)*(downstreamY - upstreamInboardY));
	double pitch = atan( numerator / denom ) * 180/M_PI;

	return pitch;
}

double BioXASMirrorControl::calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ) const
{
	double numerator = (((upstreamInboardX * upstreamOutboardZ) - (upstreamOutboardX * upstreamInboardZ)) * (upstreamInboardX - downstreamX)) + (((downstreamX * upstreamInboardZ) - (upstreamInboardX * downstreamZ)) * (upstreamInboardX - upstreamOutboardX));
	double denom = (upstreamInboardX * (upstreamInboardX - downstreamX) * (upstreamOutboardY - upstreamInboardY)) + (upstreamInboardX + (upstreamInboardX - upstreamOutboardX) * (upstreamInboardY - downstreamY));
	double result = atan(numerator/denom) * 180/M_PI;

	return result;
}

double BioXASMirrorControl::calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ) const
{
	double term1 = ((downstreamZ * upstreamOutboardY) - (upstreamOutboardZ * downstreamY)) / (upstreamOutboardY - downstreamY);
	double term2Num = ((upstreamInboardZ - downstreamZ) * (upstreamOutboardY - downstreamY)) - ((downstreamZ - upstreamOutboardZ) * (downstreamY - upstreamInboardY));
	double term2Denom = ((upstreamInboardX - downstreamX) * (upstreamOutboardY - downstreamY)) + ((upstreamOutboardX - downstreamX) * (downstreamY - upstreamInboardY));
	double term3 = (downstreamX * (downstreamY - upstreamOutboardY) + downstreamY * (upstreamOutboardX - downstreamX)) / (upstreamOutboardY - downstreamY);

	double height = term1 + (term2Num/term2Denom) * term3;

	return height;
}

double BioXASMirrorControl::calculateLateral(double upstreamLength, double downstreamLength, double lateralPosition, double yawPosition) const
{
	Q_UNUSED(downstreamLength)

	double yawControlValue = calculateYaw(upstreamLength, downstreamLength, yawPosition);
	double result = lateralPosition - upstreamLength * tan(yawControlValue * M_PI/180);

	return result;
}

double BioXASMirrorControl::calculateYaw(double upstreamLength, double downstreamLength, double yawPosition) const
{
	double result = atan(yawPosition / (downstreamLength - upstreamLength)) * 180/M_PI;
	return result;
}

double BioXASMirrorControl::calculateUpstreamInboardPosition(double upstreamInboardX, double upstreamInboardY, double pitch, double roll, double height) const
{
	double term1 = height;
	double term2 = upstreamInboardX * tan(pitch * M_PI/180);
	double term3 = upstreamInboardY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}

double BioXASMirrorControl::calculateUpstreamOutboardPosition(double upstreamOutboardX, double upstreamOutboardY, double pitch, double roll, double height) const
{
	double term1 = height;
	double term2 = upstreamOutboardX * tan(pitch * M_PI/180);
	double term3 = upstreamOutboardY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}

double BioXASMirrorControl::calculateDownstreamPosition(double downstreamX, double downstreamY, double pitch, double roll, double height) const
{
	double term1 = height;
	double term2 = downstreamX * tan(pitch * M_PI/180);
	double term3 = downstreamY * tan(roll * M_PI/180);

	double result = term1 + term2 + term3;

	return result;
}

double BioXASMirrorControl::calculateYawPosition(double yaw, double upstreamLength, double downstreamLength) const
{
	double result = (downstreamLength - upstreamLength) * tan(yaw * M_PI/180);
	return result;
}

double BioXASMirrorControl::calculateYawPositionFromLateral(double lateral, double upstreamLength, double downstreamLength, double lateralPosition) const
{
	Q_UNUSED(downstreamLength)

	double yawControlValue = atan((lateralPosition - lateral) / upstreamLength) * 180/M_PI;
	double result = BioXASMirrorControl::calculateYawPosition(yawControlValue, upstreamLength, downstreamLength);
	return result;
}

double BioXASMirrorControl::calculateLateralPosition(double lateral, double upstreamLength, double downstreamLength, double yaw) const
{
	Q_UNUSED(downstreamLength)

	double result = lateral + (upstreamLength * tan(yaw * M_PI/180));
	return result;
}
